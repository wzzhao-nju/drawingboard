#ifndef ITEM_H
#define ITEM_H

#include <QColor>
#include <cmath>
#include <QtMath>
using namespace std;

const int init_width = 780;
const int init_height = 520;
const int start_x = 10;
const int start_y = 60;
const double Pi = 3.1415926;

struct Point{
    //点的信息
    int x, y; //坐标
    QColor color; //颜色信息 0<=R, G, B<=255
    Point(int xx, int yy, QColor c) { x = xx; y = yy; color = c; }
};

enum itemState {Line, Polygon, Ellipse, Curve};

class item{
private:
    int id;
    itemState state;
    QVector<Point> points;//组成图元的所有点
    QVector<pair<int, int>> info;//关键点信息
    QColor clr;//图元颜色
    QString algorithm;//绘图时使用的算法，如果有的话
public:
    item(int i, itemState s) { id = i; state = s; }
    int get_id() { return id; }
    QColor get_color() { return clr; }
    QString get_algorithm() { return algorithm; }
    void set_id(int i) { id = i; }
    void clear_points() { points.clear(); }
    itemState get_state() { return state; }
    QVector<Point> get_points() { return points; }
    QVector<pair<int, int>> get_info() { return info; }
    void save(const QVector<Point>& pts); //保存点阵
    void save_info(const QVector<pair<int, int>>& p);//保存关键点位置
    void save_color(QColor c) { clr = c; }//保存颜色信息
    void save_algorithm(QString str) { algorithm = str; }
    void translate(int dx, int dy); //平移变换
    void scale(int x, int y, double s);//缩放变换
    void rotate(int x, int y, int r);//旋转变换
    int encode(int x, int y, int xmin, int ymin, int xmax, int ymax);
    bool clip_Cohen_Sutherland(int x1, int y1, int x2, int y2);//裁剪变换
    bool clip_Liang_Barsky(int x1, int y1, int x2, int y2);//裁剪变换
    bool find(int x, int y);//查找这个点是否存在
    ~item() { points.clear(); }
};

#endif // ITEM_H
