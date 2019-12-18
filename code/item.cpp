#include "item.h"

void item::save(const QVector<Point> &pts){
    for(int i = 0; i < pts.size(); i++)
        points.push_back(pts[i]);
}

void item::save_info(const QVector<pair<int, int>> &p){
    for(int i = 0; i < p.size(); i++)
        info.push_back(p[i]);
}

void item::translate(int dx, int dy){
    for(int i = 0; i < points.size(); i++){
        points[i].x += dx;
        points[i].y += dy;
    }
    for(int i = 0; i < info.size(); i++){
        if(state == Ellipse && i == info.size() - 1)
            break;
        info[i].first += dx;
        info[i].second += dy;
    }
}

void item::scale(int x, int y, double s){
    if(state == Ellipse){
        info[0].first = (int)((info[0].first - x) * s) + x;
        info[0].second = (int)((info[0].second - y) * s) + y;
        info[1].first = (int)(info[1].first * s);
        info[1].second = (int)(info[1].second * s);
        points.clear();
        return;
    }
    for(int i = 0; i < info.size(); i++){
        info[i].first = (int)((info[i].first - x) * s) + x;
        info[i].second = (int)((info[i].second - y) * s) + y;
    }
    points.clear();
}

void item::rotate(int x, int y, int r){
    for(int i = 0; i < info.size(); i++){
        int x0 = info[i].first, y0 = info[i].second;
        info[i].first = (int)((x0-x) * cos(r*Pi/180) - (y0-y) * sin(r*Pi/180) + x + 0.5);
        info[i].second = (int)((x0-x) * sin(r*Pi/180) + (y0-y) * cos(r*Pi/180) + y + 0.5);
    }
    points.clear();
}

int item::encode(int x, int y, int xmin, int ymin, int xmax, int ymax){
    int rst = 0;
    if(x < xmin) rst += 1;
    if(x > xmax) rst += 2;
    if(y < ymin) rst += 4;
    if(y > ymax) rst += 8;
    return rst;
}

bool item::clip_Cohen_Sutherland(int x1, int y1, int x2, int y2){
    //LEFT 1
    //RIGHT 2
    //BOTTOM 4
    //TOP 8
    points.clear();
    pair<int, int> start = info[0];
    pair<int, int> end = info[1];
    int code1 = encode(start.first, start.second, x1, y1, x2, y2);
    int code2 = encode(end.first, end.second, x1, y1, x2, y2);
    int code, x = -1, y = -1;
    while(code1 != 0 || code2 != 0){
        if((code1 & code2) != 0)//两个点均在窗口外
            return false;
        code = code1 == 0 ? code2: code1;
        if(code & 1){
            x = x1;
            y = start.second + (end.second - start.second)*(x1 - start.first)/(end.first - start.first);
        }else if(code & 2){
            x = x2;
            y = start.second + (end.second - start.second)*(x2 - start.first)/(end.first - start.first);
        }else if(code & 4){
            y = y1;
            x = start.first + (end.first - start.first)*(y1 - start.second)/(end.second - start.second);
        }else if(code & 8){
            y = y2;
            x = start.first + (end.first - start.first)*(y2 - start.second)/(end.second - start.second);
        }
        if(code == code1){
            start.first = x; start.second = y; code1 = encode(start.first, start.second, x1, y1, x2, y2);
        }else{
            end.first = x; end.second = y; code2 = encode(end.first, end.second, x1, y1, x2, y2);
        }
    }
    info[0] = start; info[1] = end;
    return true;
}

bool item::clip_Liang_Barsky(int x1, int y1, int x2, int y2){
    points.clear();
    pair<int, int> start = info[0];
    pair<int, int> end = info[1];
    double r;
    double u1 = 0, u2 = 1;
    int p[4], q[4];
    p[0] = start.first - end.first;
    p[1] = end.first - start.first;
    p[2] = start.second - end.second;
    p[3] = end.second - start.second;

    q[0] = start.first - x1;
    q[1] = x2 - start.first;
    q[2] = start.second - y1;
    q[3] = y2 - start.second;

    for(int i = 0; i < 4; i++){
        if(p[i] == 0 && q[i] < 0)
            return false;
        r = (double)q[i] / (double)p[i];
        if(p[i] < 0)
            u1 = max(u1, r);
        if(p[i] > 0)
            u2 = min(u2, r);
        if(u1 > u2)
            return false;
    }
    info[0].first = start.first + (int)(u1 * (end.first - start.first) + 0.5);
    info[0].second = start.second + (int)(u1 * (end.second - start.second) + 0.5);
    info[1].first = start.first + (int)(u2 * (end.first - start.first) + 0.5);
    info[1].second = start.second + (int)(u2 * (end.second - start.second) + 0.5);
    return true;
}

bool item::find(int x, int y){
    for(int i = 0; i < points.size(); i++)
        if(points[i].x == x && points[i].y == y)
            return true;
    return false;
}
