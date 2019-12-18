#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //窗口初始化
    is_correct = true;
    inv = new invalid_instruct(this);

    //绘图工具
    painter = new QPainter(this);
    board = new QPixmap(init_width, init_height);
    board->fill(Qt::white);
    painterpix = new QPainter(board);
    pen = new QPen(QColor(0, 0, 0));
    painterpix->setPen(*pen);
    //painter->setViewport(20, 40, board.width(), board.height());

    //数据成员
    mode = -1;
    mousecount = 0;
    idcount = 0;
    savepath = "D:\\";
    current = nullptr;
    center.first = -1; center.second = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent*){
    /* 如果有要移除的点 */
    if(toremove.size() > 0){
        painterpix->begin(board);
        painterpix->setPen(QColor(255, 255, 255));
        for(int i = 0; i < toremove.size(); i++)
            painterpix->drawPoint(QPoint(toremove[i].x, toremove[i].y));
        toremove.clear();
        painterpix->end();
        for(int i = 0; i < items.size(); i++)
            topaint.append(items[i]->get_points());
    }
    /* 画出topaint中的点 */
    painterpix->begin(board);
    painterpix->setPen(*pen);
    for(int i = 0; i < topaint.size(); i++){
        painterpix->setPen(topaint[i].color);
        painterpix->drawPoint(QPoint(topaint[i].x, topaint[i].y));
    }
    topaint.clear();
    painterpix->end();
    /* 将pixmap显示到主窗口上 */
    painter->begin(this);
    painter->drawPixmap(start_x, start_y, board->width(), board->height(), *board);
    painter->end();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    //qDebug() << event->x() << " " << event->y();
    if(event->button() == Qt::LeftButton){
        if(mode == 2){
            //绘制多边形
            press.push_back(pair<int, int>(event->x() - start_x, event->y() - start_y));
            int n = press.size();
            if(n > 1){
                draw_line_Bresenham(press[n-2].first, press[n-2].second, press[n-1].first, press[n-1].second);
                topaint.pop_back();
                current->save(topaint);
                repaint();
            }
        }else if(mode == 4){
            //绘制曲线
            press.push_back(pair<int, int>(event->x() - start_x, event->y() - start_y));
            int n = press.size();
            if(n == 2){
                draw_curve_Bezier(press, 100);
                current = new item(-1, Curve);
                current->save(topaint);
                repaint();
            }else if(n > 2){
                toremove.append(current->get_points());
                draw_curve_Bezier(press, 100);
                current->clear_points();
                current->save(topaint);
                repaint();
            }
        }else if(mode == 5){
            //选取图元
            int x = event->x() - start_x, y = event->y() - start_y;
            current = nullptr;
            for(int i = 0; i < items.size(); i++){
                for(int j = x - 4; j <= x + 4; j++)
                    if(items[i]->find(j, y) == true)
                        current = items[i];
                for(int j = y - 4; j <= y + 4; j++)
                    if(items[i]->find(x, j) == true)
                        current = items[i];
            }
            if(current != nullptr){
                ui->statusBar->clearMessage();
                QString type;
                switch (current->get_state()) {
                case Line: type = "直线"; break;
                case Polygon: type = "多边形"; break;
                case Ellipse: type = "椭圆"; break;
                case Curve: type = "曲线"; break;
                }
                ui->statusBar->showMessage("已选择图元 id=" + QString::number(current->get_id()) + " 类型=" + type);
            }
        }else if(mode == 7 && center.first == -1){
            //旋转变换，选取中心
            center.first = event->x() - start_x;
            center.second = event->y() - start_y;
            ui->statusBar->showMessage("已选择旋转中心：(" + QString::number(center.first) + ", " + QString::number(center.second) + ")");
        }else if(mode == 9 && center.first == -1){
            center.first = event->x() - start_x;
            center.second = event->y() - start_y;
            ui->statusBar->showMessage("已选择缩放中心：(" + QString::number(center.first) + ", " + QString::number(center.second) + ")，滑动滚轮进行缩放，右键取消");
        }else if(mode == 10){
            press.push_back(pair<int, int>(event->x() - start_x, event->y() - start_y));
            int n = press.size();
            if(n == 2){
                pair<int, int> one(press[0].first * 2 - press[1].first, press[0].second * 2 - press[1].second);
                pair<int, int> two(press[n-1].first * 2 - press[n-2].first, press[n-1].second * 2 - press[n-2].second);
                QVector<pair<int, int>> ctrlpoints;
                ctrlpoints.push_back(one);
                ctrlpoints.append(press);
                ctrlpoints.push_back(two);
                draw_curve_Bspline(ctrlpoints, 100);
                current->save(topaint);
                repaint();
            }else if(n > 2){
                toremove.append(current->get_points());
                current->clear_points();
                pair<int, int> one(press[0].first * 2 - press[1].first, press[0].second * 2 - press[1].second);
                pair<int, int> two(press[n-1].first * 2 - press[n-2].first, press[n-1].second * 2 - press[n-2].second);
                QVector<pair<int, int>> ctrlpoints;
                ctrlpoints.push_back(one);
                ctrlpoints.append(press);
                ctrlpoints.push_back(two);
                draw_curve_Bspline(ctrlpoints, 100);
                current->save(topaint);
                repaint();
            }
        }else{
            start.first = event->x() - start_x;
            start.second = event->y() - start_y;
            mousecount = 0;
        }
    }else if(event->button() == Qt::RightButton){
        if(mode == 2){
            press.push_back(pair<int, int>(event->x() - start_x, event->y() - start_y));
            int n = press.size();
            draw_line_Bresenham(press[n-2].first, press[n-2].second, press[n-1].first, press[n-1].second);
            topaint.pop_back();
            draw_line_Bresenham(press[n-1].first, press[n-1].second, press[0].first, press[0].second);
            topaint.pop_back();
            current->save(topaint);
            current->set_id(idcount++);
            current->save_color(pen->color());
            current->save_info(press);
            current->save_algorithm("Bresenham");
            items.push_back(current);
            repaint();
            mode = 0;
            current = nullptr;
            ui->statusBar->clearMessage();
            press.clear();
        }else if(mode == 4){
            toremove.append(current->get_points());
            press.push_back(pair<int, int>(event->x() - start_x, event->y() - start_y));
            draw_curve_Bezier(press, 100);
            current->set_id(idcount++);
            current->clear_points();
            current->save_color(pen->color());
            current->save_info(press);
            current->save_algorithm("Bezier");
            current->save(topaint);
            items.push_back(current);
            repaint();
            mode = 0;
            current = nullptr;
            ui->statusBar->clearMessage();
            press.clear();
        }else if(mode == 9){
            center.first = -1; center.second = -1;
            mode = 0;
            current = nullptr;
            ui->statusBar->clearMessage();
        }else if(mode == 10){
            toremove.append(current->get_points());
            press.push_back(pair<int, int>(event->x() - start_x, event->y() - start_y));
            int n = press.size();
            pair<int, int> one(press[0].first * 2 - press[1].first, press[0].second * 2 - press[1].second);
            pair<int, int> two(press[n-1].first * 2 - press[n-2].first, press[n-1].second * 2 - press[n-2].second);
            QVector<pair<int, int>> ctrlpoints;
            ctrlpoints.push_back(one);
            ctrlpoints.append(press);
            ctrlpoints.push_back(two);
            draw_curve_Bspline(ctrlpoints, 100);
            current->set_id(idcount++);
            current->clear_points();
            current->save_color(pen->color());
            current->save_info(ctrlpoints);
            current->save_algorithm("B-spline");
            current->save(topaint);
            items.push_back(current);
            repaint();
            mode = 0;
            current = nullptr;
            ui->statusBar->clearMessage();
            press.clear();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    //qDebug() << event->x() << ":" << event->y();
    mousecount++;
    if(mousecount < mouselimited)
        return;
    else
        mousecount = 0;
    end.first = event->x() - start_x;
    end.second = event->y() - start_y;
    if(mode == 1){
        toremove.append(current->get_points());
        draw_line_Bresenham(start.first, start.second, end.first, end.second);
        current->clear_points();
        current->save(topaint);
        repaint();
    }else if(mode == 3){
        toremove.append(current->get_points());
        int xc = (start.first + end.first) / 2;
        int yc = (start.second + end.second) / 2;
        int rx = abs((end.first - start.first) / 2);
        int ry = abs((end.second - start.second) / 2);
        draw_ellipse(xc, yc, rx, ry);
        current->clear_points();
        current->save(topaint);
        repaint();
    }else if(mode == 6){
        translate(current, end.first - start.first, end.second - start.second);
        start = end;
        repaint();
    }else if(mode == 7){
        double a = sqrt(pow(end.first - start.first, 2) + pow(end.second - start.second, 2));
        double b = sqrt(pow(center.first - start.first, 2) + pow(center.second - start.second, 2));
        double c = sqrt(pow(end.first - center.first, 2) + pow(end.second - center.second, 2));
        double cosa = (b * b + c * c - a * a) / 2 / b / c;
        int r = (int)(qAcos(cosa) * 180 / Pi);
        if(r >= 1){
            rotate(current, center.first, center.second, r);
            start = end;
            repaint();
        }
    }else if(mode == 8){
        //toremove.append(current->get_points());
        item* temp = new item(-1, Line);
        temp->save(current->get_points());
        temp->save_info(current->get_info());
        temp->save_color(current->get_color());
        temp->save_algorithm(current->get_algorithm());
        clip_Cohen_Sutherland(temp, start.first, start.second, end.first, end.second);
        repaint();
        toremove.append(temp->get_points());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    end.first = event->x() - start_x;
    end.second = event->y() - start_y;
    if(mode == 1){
        toremove.append(current->get_points());
        draw_line_Bresenham(start.first, start.second, end.first, end.second);
        QVector<pair<int, int>> ctrlpoints;
        ctrlpoints.push_back(start);
        ctrlpoints.push_back(end);
        current->set_id(idcount++);
        current->clear_points();
        current->save(topaint);
        current->save_color(pen->color());
        current->save_info(ctrlpoints);
        current->save_algorithm("Bresenham");
        items.push_back(current);
        repaint();
        mode = 0;
        current = nullptr;
        ui->statusBar->clearMessage();
    }else if(mode == 3){
        toremove.append(current->get_points());
        int xc = (start.first + end.first) / 2;
        int yc = (start.second + end.second) / 2;
        int rx = abs((end.first - start.first) / 2);
        int ry = abs((end.second - start.second) / 2);
        draw_ellipse(xc, yc, rx, ry);
        QVector<pair<int, int>> ctrlpoints;
        ctrlpoints.push_back(pair<int, int>(xc, yc));
        ctrlpoints.push_back(pair<int, int>(rx, ry));
        current->set_id(idcount++);
        current->clear_points();
        current->save(topaint);
        current->save_color(pen->color());
        current->save_info(ctrlpoints);
        items.push_back(current);
        repaint();
        mode = 0;
        current = nullptr;
        ui->statusBar->clearMessage();
    }else if(mode == 6){
        translate(current, end.first - start.first, end.second - start.second);
        repaint();
        mode = 0;
        current = nullptr;
        ui->statusBar->clearMessage();
    }else if(mode == 7 && event->button() == Qt::RightButton){
        double a = sqrt(pow(end.first - start.first, 2) + pow(end.second - start.second, 2));
        double b = sqrt(pow(center.first - start.first, 2) + pow(center.second - start.second, 2));
        double c = sqrt(pow(end.first - center.first, 2) + pow(end.second - center.second, 2));
        double cosa = (b * b + c * c - a * a) / 2 / b / c;
        int r = (int)(qAcos(cosa) * 180 / Pi);
        if(r >= 1){
            rotate(current, center.first, center.second, r);
            start = end;
            repaint();
        }
        center.first = -1; center.second = -1;
        mode = 0;
        current = nullptr;
        ui->statusBar->clearMessage();
    }else if(mode == 8){
        clip_Cohen_Sutherland(current, start.first, start.second, end.first, end.second);
        repaint();
        mode = 0;
        current = nullptr;
        ui->statusBar->clearMessage();
    }
    mousecount = 0;
}

void MainWindow::wheelEvent(QWheelEvent *event){
    if(mode != 9)
        return;
    if(event->delta() > 0){
        scale(current, center.first, center.second, 1.05);
        repaint();
    }else{
        scale(current, center.first, center.second, 0.95);
        repaint();
    }
}

void MainWindow::openfile(QString file, QString save){
    filename = file;
    savepath = save;
    QFile input(filename);
    if(input.open(QIODevice::ReadOnly) == true){
        QByteArray arr = input.readAll();
        QString str(arr);
        QStringList in = str.split('\n');
        for(int i = 0; i < in.size(); i++)
            if(in[i].size() > 0 && in[i].back() == '\r')
                in[i].remove('\r');
        analyze_instruct(in);
    }
}

void MainWindow::on_from_instruct_triggered()
{
    instruct = new input_instruct(this);
    instruct->setModal(true);
    connect(instruct,SIGNAL(sendInstruct(QStringList)),this,SLOT(analyze_instruct(QStringList)));
    instruct->show();
}

void MainWindow::on_from_file_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), ".", tr("文本文件(*.txt)"));
    openfile(fileName, "D:\\");
}

void MainWindow::analyze_instruct(QStringList in){
    //in, 输入的指令序列
    for(int i = 0; i < in.size(); i++){
        QStringList instruct = in[i].split(' ');
        if(instruct[0].compare("resetCanvas")==0){
            //重置画布
            int width = instruct[1].toInt();
            int height = instruct[2].toInt();
            if(width >= 100 && height >= 100 && width <= 1000 && height <= 1000){
                board = new QPixmap(width, height);
                board->fill(Qt::white);
                current = nullptr;
                items.clear();
                repaint();
            }else{
                is_correct = false; continue;
            }
        }else if(instruct[0].compare("saveCanvas")==0){
            //保存画布
            board->save(savepath + instruct[1] + ".bmp", "BMP");
        }else if(instruct[0].compare("setColor")==0){
            //设置画笔颜色
            int R = instruct[1].toInt();
            int G = instruct[2].toInt();
            int B = instruct[3].toInt();
            if(R >= 0 && R <= 255 && G >= 0 && G <= 255 && B >= 0 && B <= 255){
                pen->setColor(QColor(R, G, B));
            }else{
                is_correct = false; continue;
            }
        }else if(instruct[0].compare("drawLine")==0){
            /* 绘制直线 */
            int id = instruct[1].toInt();
            int x1 = instruct[2].toInt();
            int y1 = instruct[3].toInt();
            int x2 = instruct[4].toInt();
            int y2 = instruct[5].toInt();
            QString algorithm = instruct[6];
            if(algorithm == "DDA"){
                draw_line_DDA(x1, y1, x2, y2);
            }
            else if(algorithm == "Bresenham"){
                draw_line_Bresenham(x1, y1, x2, y2);
            }else{
                is_correct = false; continue;
            }
            QVector<pair<int, int>> ctrlpoints;
            ctrlpoints.push_back(pair<int, int>(x1, y1));
            ctrlpoints.push_back(pair<int, int>(x2, y2));
            /* 存储图元信息 */
            current = new item(id, Line);
            current->save(topaint);
            current->save_color(pen->color());
            current->save_info(ctrlpoints);
            current->save_algorithm(algorithm);
            items.push_back(current);
            repaint();
        }else if(instruct[0].compare("drawPolygon")==0){
            //绘制多边形
            int id = instruct[1].toInt();
            QString algorithm = instruct[3];
            if(algorithm != "DDA" && algorithm != "Bresenham") {
                is_correct = false; continue;
            }
            i++;
            if(i >= in.size()) { is_correct = false; break; }
            QStringList nums = in[i].split(' ');
            int len = nums.size();
            int x, y;
            QVector<pair<int, int>> ctrlpoints;
            for(int j = 0; j < len; j += 2){
                x = nums[j].toInt();
                y = nums[j+1].toInt();
                ctrlpoints.push_back(pair<int, int>(x, y));
            }
            draw_polygon(ctrlpoints, algorithm);
            current = new item(id, Polygon);
            current->save(topaint);
            current->save_color(pen->color());
            current->save_info(ctrlpoints);
            current->save_algorithm(algorithm);
            items.push_back(current);
            repaint();
        }else if(instruct[0].compare("drawEllipse")==0){
            //绘制椭圆
            int id = instruct[1].toInt();
            int xc = instruct[2].toInt();
            int yc = instruct[3].toInt();
            int rx = instruct[4].toInt();
            int ry = instruct[5].toInt();
            draw_ellipse(xc, yc, rx, ry);
            QVector<pair<int, int>> ctrlpoints;
            ctrlpoints.push_back(pair<int, int>(xc, yc));
            ctrlpoints.push_back(pair<int, int>(rx, ry));

            current = new item(id, Ellipse);
            current->save(topaint);
            current->save_color(pen->color());
            current->save_info(ctrlpoints);
            items.push_back(current);
            repaint();
        }else if(instruct[0].compare("drawCurve")==0){
            //绘制曲线
            int id = instruct[1].toInt();
            int n = instruct[2].toInt();
            QString algorithm = instruct[3];
            i++;
            if(i >= in.size()) { is_correct = false; break; }
            QStringList nums = in[i].split(' ');
            QVector<pair<int, int>> ctrlpoints;//控制点数组
            for(int j = 0; j < n; j++)
                ctrlpoints.push_back(pair<int, int>(nums[2*j].toInt(), nums[2*j+1].toInt()));
            if(algorithm == "Bezier")
                draw_curve_Bezier(ctrlpoints, 100);
            else if(algorithm == "B-spline"){
                draw_curve_Bspline(ctrlpoints, 50);
            }else{
                is_correct = false; continue;
            }
            current = new item(id, Curve);
            current->save(topaint);
            current->save_color(pen->color());
            current->save_info(ctrlpoints);
            current->save_algorithm(algorithm);
            items.push_back(current);
            repaint();
        }else if(instruct[0].compare("translate")==0){
            //对图元平移
            int id = instruct[1].toInt();
            int dx = instruct[2].toInt();
            int dy = instruct[3].toInt();
            item* choose = nullptr;
            for(int i = 0; i < items.size(); i++){
                if(items[i]->get_id() == id){
                    choose = items[i];
                    translate(choose, dx, dy);
                }
            }
            if(choose == nullptr){
                is_correct = false; continue;
            }
            repaint();
        }else if(instruct[0].compare("rotate")==0){
            //对图元旋转
            int id = instruct[1].toInt();
            int x = instruct[2].toInt();
            int y = instruct[3].toInt();
            int r = instruct[4].toInt();
            item* choose = nullptr;
            for(int i = 0; i < items.size(); i++){
                if(items[i]->get_id() == id){
                    choose = items[i];
                    rotate(choose, x, y, r);
                }
            }
            if(choose == nullptr){
                is_correct = false; continue;
            }
            repaint();
        }else if(instruct[0].compare("scale")==0){
            //对图元缩放
            int id = instruct[1].toInt();
            int x = instruct[2].toInt();
            int y = instruct[3].toInt();
            double s = instruct[4].toDouble();
            item* choose = nullptr;
            for(int i = 0; i < items.size(); i++){
                if(items[i]->get_id() == id){
                    choose = items[i];
                    scale(choose, x, y, s);
                }
            }
            if(choose == nullptr){
                is_correct = false; continue;
            }
            repaint();
        }else if(instruct[0].compare("clip")==0){
            //对线段裁剪
            int id = instruct[1].toInt();
            int x1 = instruct[2].toInt();
            int y1 = instruct[3].toInt();
            int x2 = instruct[4].toInt();
            int y2 = instruct[5].toInt();
            QString algorithm = instruct[6];
            item* choose = nullptr;
            for(int i = 0; i < items.size(); i++){
                if(items[i]->get_id() == id){
                    choose = items[i];
                    if(algorithm == "Cohen-Sutherland")
                        clip_Cohen_Sutherland(choose, x1, y1, x2, y2);
                    else if(algorithm == "Liang-Barsky"){
                        clip_Liang_Barsky(choose, x1, y1, x2, y2);
                    }else{
                        is_correct = false; continue;
                    }
                }
            }
            if(choose == nullptr){
                is_correct = false; continue;
            }
            repaint();
        }else{
            //输入指令有误
            is_correct = false; continue;
        }
        if(filename.isEmpty() == false){
            /* 如果有要移除的点 */
            if(toremove.size() > 0){
                painterpix->begin(board);
                painterpix->setPen(QColor(255, 255, 255));
                for(int i = 0; i < toremove.size(); i++)
                    painterpix->drawPoint(QPoint(toremove[i].x, toremove[i].y));
                toremove.clear();
                painterpix->end();
                for(int i = 0; i < items.size(); i++)
                    topaint.append(items[i]->get_points());
            }
            /* 画出topaint中的点 */
            painterpix->begin(board);
            painterpix->setPen(*pen);
            for(int i = 0; i < topaint.size(); i++){
                painterpix->setPen(topaint[i].color);
                painterpix->drawPoint(QPoint(topaint[i].x, topaint[i].y));
            }
            topaint.clear();
            painterpix->end();
            /* 将pixmap显示到主窗口上 */
            painter->begin(this);
            painter->drawPixmap(start_x, start_y, board->width(), board->height(), *board);
            painter->end();
        }
    }
    if(filename.isEmpty() == true && is_correct == false){
        inv->show();
        is_correct = true;
    }
    current = nullptr;
}

void MainWindow::draw_line_DDA(int x1, int y1, int x2, int y2){
    double delta_x, delta_y, x, y;
    int dx, dy, steps;
    dx = x2 - x1;
    dy = y2 - y1;
    if(abs(dx)>abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);
    delta_x = (double)dx / (double)steps;
    delta_y = (double)dy / (double)steps;
    x = x1; y = y1;
    for(int i = 1; i <= steps + 1; i++){
        topaint.push_back(Point((int)(x + 0.5), (int)(y + 0.5), pen->color()));
        x += delta_x;
        y += delta_y;
    }
}

void MainWindow::draw_line_Bresenham(int x1, int y1, int x2, int y2){
    int dx = x2 - x1, dy = y2 - y1;
    int step_x = dx > 0 ? 1 : -1;
    int step_y = dy > 0 ? 1 : -1;
    dx = abs(dx); dy = abs(dy);
    int x = x1, y = y1;
    if (dx > dy) {
        //斜率绝对值<1
        int p = 2 * dy - dx;
        for (x = x1; x != x2; x += step_x) {
            topaint.push_back(Point(x, y, pen->color()));
            if (p > 0) {
                y += step_y;
                p = p + (dy - dx) * 2;
            }
            else
                p = p + dy * 2;
        }
    }
    else {
        //斜率绝对值>1
        int p = 2 * dx - dy;
        for (y = y1; y != y2; y += step_y) {
            topaint.push_back(Point(x, y, pen->color()));
            if (p > 0) {
                x += step_x;
                p = p + (dx - dy) * 2;
            }
            else
                p = p + dx * 2;
        }
    }
    topaint.push_back(Point(x, y, pen->color()));
}

void MainWindow::draw_polygon(const QVector<pair<int, int> > &ctrlpoints, QString algorithm){
    int n = ctrlpoints.size();
    for(int i = 0; i < n; i++){
        int x1 = ctrlpoints[i % n].first;
        int y1 = ctrlpoints[i % n].second;
        int x2 = ctrlpoints[(i+1) % n].first;
        int y2 = ctrlpoints[(i+1) % n].second;
        if(algorithm == "DDA")
            draw_line_DDA(x1, y1, x2, y2);
        else if(algorithm == "Bresenham")
            draw_line_Bresenham(x1, y1, x2, y2);
        topaint.pop_back();
    }
}

void MainWindow::draw_ellipse(int xc, int yc, int rx, int ry){
    topaint.clear();
    long long int rx2 = rx * rx, ry2 = ry * ry;
    int x = 0, y = ry;
    long long int px = 0, py = 2 * rx2 * y; //px = 2 * ry2 * x, 因为x=0所以px=0
    long long int p = ry2 - rx2 * ry + (int)(0.25 * (double)rx2 + 0.5);
    topaint.push_back(Point(x + xc, y + yc, pen->color()));//先push第一个点(0, ry)
    /* Region 1 斜率绝对值<1 */
    while(px < py){
        x++;
        px += 2 * ry2;
        if(p < 0)
            p += ry2 + px;
        else{
            y--;
            py -= 2 * rx2;
            p += rx2 + px - py;
        }
        topaint.push_back(Point(x + xc, y + yc, pen->color()));
    }
    /* Region 2 斜率绝对值>1 */
    p = (long long int)(ry2 * pow(x + 0.5, 2) + rx2 * pow(y - 1, 2) - rx2 * ry2 + 0.5);
    while(y > 0){
        y--;
        py -= 2 * rx2;
        if(p > 0)
            p += rx2 - py;
        else{
            x++;
            px += 2 * ry2;
            p += rx2 - py + px;
        }
        topaint.push_back(Point(x + xc, y + yc, pen->color()));
    }
    /* 扩展到四个象限 */
    for(int i = topaint.size() - 2; i >= 0 ; i--)
        topaint.push_back(Point(topaint[i].x, 2 * yc - topaint[i].y, pen->color()));
    for(int i = topaint.size() - 2; i > 0 ; i--)
        topaint.push_back(Point(2 * xc - topaint[i].x, topaint[i].y, pen->color()));
}

void MainWindow::draw_curve_Bezier(QVector<pair<int, int>> ctrlpoints, int precision){
    QVector<pair<int, int>> index;//计算出来的曲线上的点
    int n = ctrlpoints.size();
    /* 计算参数序列 */
    int *c = new int[n];
    for(int i = 0; i < n; i++){
        c[i] = 1;
        for(int j = n - 1; j > i; j--)
            c[i] *= j;
        for(int j = n - 1 - i; j > 0; j--)
            c[i] /= j;
    }
    double t;//t ∈ [0, 1]
    for(int i = 0; i <= precision; i++){
        t = (double)i / (double)precision;
        double bezier = 0;//存储贝塞尔函数的计算结果
        double xtemp = 0, ytemp = 0;
        for(int j = 0; j < n; j++){
            bezier = c[j] * pow(t, j) * pow(1 - t, n - 1 - j);
            xtemp += bezier * (double)ctrlpoints[j].first;
            ytemp += bezier * (double)ctrlpoints[j].second;
        }
        int x = (int)(xtemp + 0.5), y = (int)(ytemp + 0.5);
        index.push_back(pair<int, int>(x, y));
    }
    for(int i = 0; i < index.size() - 1; i++){
        draw_line_Bresenham(index[i].first, index[i].second, index[i+1].first, index[i+1].second);
        if(i != index.size() - 1)
            topaint.pop_back();
    }
}

double MainWindow::B_F03(double t) { return 1.0 / 6 * (-pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1); }
double MainWindow::B_F13(double t) { return 1.0 / 6 * (3 * pow(t, 3) - 6 * pow(t, 2) + 4); }
double MainWindow::B_F23(double t) { return 1.0 / 6 * (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1); }
double MainWindow::B_F33(double t) { return 1.0 / 6 * pow(t, 3); }

void MainWindow::draw_curve_Bspline(QVector<pair<int, int> > ctrlpoints, int precision){
    //todo 历史遗留问题
    QVector<pair<int, int>> index;//计算出来的曲线上的点
    int n = ctrlpoints.size();
    const int k = 3;
    double *u = new double[n+k+1];
    double step = 1.0 / (double)(n+k);
    for(int i = 0; i < n+k+1; i++)
        u[i] = step * i;
    step = 1.0 / (double)precision;
    double x, y, t = u[k];
    for(int i = 0; i < n - k; i++){
        // Pi = ctrlpoints[i]
        for(t = 0; t <= 1; t += step){
            //qDebug() << B_F03(t) << " " << B_F13(t) << " " << B_F23(t) << " " << B_F33(t);
            x = B_F03(t) * ctrlpoints[i].first + B_F13(t) * ctrlpoints[i+1].first + B_F23(t) * ctrlpoints[i+2].first + B_F33(t) * ctrlpoints[i+3].first;
            y = B_F03(t) * ctrlpoints[i].second + B_F13(t) * ctrlpoints[i+1].second + B_F23(t) * ctrlpoints[i+2].second + B_F33(t) * ctrlpoints[i+3].second;
            index.push_back(pair<int, int>((int)(x + 0.5), (int)(y + 0.5)));
        }
    }
    for(int i = 0; i < index.size() - 1; i++){
        draw_line_Bresenham(index[i].first, index[i].second, index[i+1].first, index[i+1].second);
        if(i != index.size() - 1)
            topaint.pop_back();
    }
}

void MainWindow::translate(item *choose, int dx, int dy){
    toremove.append(choose->get_points());
    choose->translate(dx, dy);
}

void MainWindow::scale(item *choose, int x, int y, double s){
    toremove.append(choose->get_points());
    choose->scale(x, y, s);
    QVector<pair<int, int>> ctrlpoints = choose->get_info();
    QColor temp = pen->color();
    pen->setColor(choose->get_color());
    if(choose->get_state() == Line){
        if(choose->get_algorithm() == "DDA")
            draw_line_DDA(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
        else if(choose->get_algorithm() == "Bresenham")
            draw_line_Bresenham(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
    }else if(choose->get_state() == Polygon){
        draw_polygon(ctrlpoints, choose->get_algorithm());
    }else if(choose->get_state() == Ellipse){
        draw_ellipse(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
    }else if(choose->get_state() == Curve){
        if(choose->get_algorithm() == "Bezier"){
            draw_curve_Bezier(ctrlpoints, 100);
        }else if(choose->get_algorithm() == "B-spline"){
            draw_curve_Bspline(ctrlpoints, 100);
        }
    }
    choose->save(topaint);
    pen->setColor(temp);
}

void MainWindow::rotate(item *choose, int x, int y, int r){
    if(choose->get_state() == Ellipse)
        return;
    toremove.append(choose->get_points());
    choose->rotate(x, y, r);
    QVector<pair<int, int>> ctrlpoints = choose->get_info();
    QColor temp = pen->color();
    pen->setColor(choose->get_color());
    if(choose->get_state() == Line){
        if(choose->get_algorithm() == "DDA")
            draw_line_DDA(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
        else if(choose->get_algorithm() == "Bresenham")
            draw_line_Bresenham(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
    }else if(choose->get_state() == Polygon){
        draw_polygon(ctrlpoints, choose->get_algorithm());
    }else if(choose->get_state() == Curve){
        if(choose->get_algorithm() == "Bezier"){
            draw_curve_Bezier(ctrlpoints, 100);
        }else if(choose->get_algorithm() == "B-spline"){
            draw_curve_Bspline(ctrlpoints, 100);
        }
    }
    choose->save(topaint);
    pen->setColor(temp);
}

void MainWindow::clip_Cohen_Sutherland(item *choose, int x1, int y1, int x2, int y2){
    if(choose->get_state() != Line)
        return;
    toremove.append(choose->get_points());
    if(choose->clip_Cohen_Sutherland(x1, y1, x2, y2) == false)
        return;
    QVector<pair<int, int>> ctrlpoints = choose->get_info();
    QColor temp = pen->color();
    pen->setColor(choose->get_color());
    if(choose->get_algorithm() == "DDA")
        draw_line_DDA(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
    else if(choose->get_algorithm() == "Bresenham")
        draw_line_Bresenham(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
    choose->save(topaint);
    pen->setColor(temp);
}

void MainWindow::clip_Liang_Barsky(item *choose, int x1, int y1, int x2, int y2){
    if(choose->get_state() != Line)
        return;
    toremove.append(choose->get_points());
    if(choose->clip_Liang_Barsky(x1, y1, x2, y2) == false)
        return;
    QVector<pair<int, int>> ctrlpoints = choose->get_info();
    QColor temp = pen->color();
    pen->setColor(choose->get_color());
    if(choose->get_algorithm() == "DDA")
        draw_line_DDA(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
    else if(choose->get_algorithm() == "Bresenham")
        draw_line_Bresenham(ctrlpoints[0].first, ctrlpoints[0].second, ctrlpoints[1].first, ctrlpoints[1].second);
    choose->save(topaint);
    pen->setColor(temp);
}

void MainWindow::on_line_clicked()
{
    mode = 1;
    current = new item(-1, Line);
    ui->statusBar->showMessage("就绪-画直线");
}

void MainWindow::on_polygon_clicked()
{
    mode = 2;
    current = new item(-1, Polygon);
    ui->statusBar->showMessage("就绪-画多边形");
}

void MainWindow::on_ellipse_clicked()
{
    mode = 3;
    current = new item(-1, Ellipse);
    ui->statusBar->showMessage("就绪-画椭圆");
}

void MainWindow::on_curve_clicked()
{
    mode = 4;
    current = new item(-1, Curve);
    ui->statusBar->showMessage("就绪-画曲线(贝塞尔)");
}

void MainWindow::on_choose_clicked()
{
    mode = 5;
    ui->statusBar->showMessage("就绪-选择图元");
}

void MainWindow::on_translate_clicked()
{
    if(current == nullptr){
        ui->statusBar->showMessage("请先选择图元", 2000);
        return;
    }
    mode = 6;
    ui->statusBar->showMessage("就绪-平移变换");
}

void MainWindow::on_rotate_clicked()
{
    if(current == nullptr){
        ui->statusBar->showMessage("请先选择图元", 2000);
        return;
    }
    if(current->get_state() == Ellipse){
        ui->statusBar->showMessage("椭圆不支持旋转", 2000);
        current = nullptr;
        return;
    }
    mode = 7;
    ui->statusBar->showMessage("就绪-旋转变换，请先选择旋转中心");
}

void MainWindow::on_remove_clicked()
{
    if(current == nullptr){
        ui->statusBar->showMessage("请先选择图元", 2000);
        return;
    }
    toremove.append(current->get_points());
    current->clear_points();
    ui->statusBar->showMessage("已删除", 2000);
    repaint();
}

void MainWindow::on_clip_clicked()
{
    if(current == nullptr){
        ui->statusBar->showMessage("请先选择图元", 2000);
        return;
    }
    if(current->get_state() != Line){
        ui->statusBar->showMessage("仅能裁剪直线", 2000);
        current = nullptr;
        return;
    }
    mode = 8;
    ui->statusBar->showMessage("就绪，拖动以裁剪");
}

void MainWindow::on_scale_clicked()
{
    if(current == nullptr){
        ui->statusBar->showMessage("请先选择图元", 2000);
        return;
    }
    mode = 9;
    ui->statusBar->showMessage("就绪-缩放，请先选择缩放中心");
}

void MainWindow::on_action_clear_triggered()
{
    res = new class reset(this);
    res->setModal(true);
    res->show();
    connect(res, SIGNAL(sendInfo(int, int)), this, SLOT(resetCanvas(int, int)));
}

void MainWindow::resetCanvas(int w, int h){
    board = new QPixmap(w, h);
    board->fill(Qt::white);
    current = nullptr;
    items.clear();
    repaint();
}

void MainWindow::on_action_line_triggered(){ on_line_clicked(); }
void MainWindow::on_action_ellipse_triggered(){ on_ellipse_clicked(); }
void MainWindow::on_action_polygon_triggered(){ on_polygon_clicked(); }
void MainWindow::on_action_curve_Bezier_triggered(){ on_curve_clicked(); }
void MainWindow::on_action_choose_triggered(){ on_choose_clicked(); }
void MainWindow::on_action_translate_triggered(){ on_translate_clicked(); }
void MainWindow::on_action_rotate_triggered(){ on_rotate_clicked(); }
void MainWindow::on_action_scale_triggered(){ on_scale_clicked(); }
void MainWindow::on_action_clip_triggered(){ on_clip_clicked(); }
void MainWindow::on_action_quit_triggered(){ this->close(); }

void MainWindow::on_action_save_triggered()
{
    QString savePath = QFileDialog::getSaveFileName(this, tr("保存文件"), ".", tr("图片文件(*.bmp)"));
    board->save(savePath, "BMP");
}

void MainWindow::on_palette_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white, this, tr("选择颜色"), QColorDialog::ShowAlphaChannel);
    pen->setColor(color);
}

void MainWindow::on_action_curve_Bspline_triggered()
{
    mode = 10;
    current = new item(-1, Curve);
    ui->statusBar->showMessage("就绪-画曲线(B样条)");
}

void MainWindow::on_action_viewcode_triggered()
{
    QDesktopServices::openUrl(QUrl(QString("https://github.com/stnanzhao/drawingboard")));
}
