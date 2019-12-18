#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cmath>
#include <QMainWindow>
#include <QMouseEvent>
#include <QDebug>
#include <QString>
#include <QPainter>
#include <QPen>
#include <QVector>
#include <QFile>
#include <QFileDialog>
#include <QColorDialog>
#include <QDesktopServices>
#include "input_instruct.h"
#include "invalid_instruct.h"
#include "reset.h"
#include "item.h"
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const int mouselimited = 1;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //paintevent
    void paintEvent(QPaintEvent*);
    //读取文件
    void openfile(QString file, QString save);
    //绘图函数
    void draw_line_DDA(int x1, int y1, int x2, int y2);
    void draw_line_Bresenham(int x1, int y1, int x2, int y2);
    void draw_polygon(const QVector<pair<int, int>>& ctrlpoints, QString algorithm);
    void draw_ellipse(int xc, int yc, int rx, int ry);
    void draw_curve_Bezier(QVector<pair<int, int>> ctrlpoints, int precision);
    double B_F03(double t);
    double B_F13(double t);
    double B_F23(double t);
    double B_F33(double t);
    void draw_curve_Bspline(QVector<pair<int, int>> ctrlpoints, int precision);
    void translate(item* choose, int dx, int dy);
    void scale(item* choose, int x, int y, double s);
    void rotate(item* choose, int x, int y, int r);
    void clip_Cohen_Sutherland(item* choose, int x1, int y1, int x2, int y2);
    void clip_Liang_Barsky(item* choose, int x1, int y1, int x2, int y2);
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
private slots:
    void on_from_instruct_triggered();
    void analyze_instruct(QStringList in);//解析命令
    void on_line_clicked();
    void on_polygon_clicked();
    void on_ellipse_clicked();
    void on_curve_clicked();
    void on_choose_clicked();
    void on_translate_clicked();
    void on_rotate_clicked();
    void on_remove_clicked();
    void on_clip_clicked();
    void on_scale_clicked();
    void on_from_file_triggered();
    void on_action_clear_triggered();
    void resetCanvas(int w, int h);
    void on_action_line_triggered();
    void on_action_ellipse_triggered();
    void on_action_polygon_triggered();
    void on_action_curve_Bezier_triggered();
    void on_action_choose_triggered();
    void on_action_translate_triggered();
    void on_action_rotate_triggered();
    void on_action_scale_triggered();
    void on_action_clip_triggered();
    void on_action_quit_triggered();
    void on_action_save_triggered();
    void on_palette_clicked();
    void on_action_curve_Bspline_triggered();
    void on_action_viewcode_triggered();

private:
    Ui::MainWindow *ui;//ui
    //参数
    QString filename;//指令文件名
    QString savepath;//存储路径

    //画布
    QPixmap* board;
    //画笔
    QPainter* painter;//用于调用drawPixmap
    QPainter* painterpix;//Pixmap的画笔
    QPen* pen;//设置painterpix

    //数据成员
    int mode;//绘图模式
    /*1. 画直线 2. 画多边形
     *3. 画椭圆 4. 画曲线(Bezier)
     *5. 选择图元 6. 平移
     *7. 旋转 8. 裁剪
     *9. 缩放 10. 画曲线(Bspline)
     */
    int mousecount;//鼠标拖动计数
    int idcount;
    pair<int, int> center;
    pair<int, int> start;
    pair<int, int> end;
    QVector<pair<int, int>> press;//鼠标按下的点，多边形专用
    item* current;//正在编辑的图元
    QVector<item*> items;//图元集合
    QVector<Point> topaint;//要绘制的点
    QVector<Point> toremove;//要移除的点
    QVector<Point> temp;

    //弹出对话框
    bool is_correct;//输入的指令是否正确
    input_instruct *instruct;
    invalid_instruct *inv;
    class reset *res;



};
#endif // MAINWINDOW_H
