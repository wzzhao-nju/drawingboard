#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.openfile("D:\\大学\\计算机图形学\\大作业\\样例\\input.txt", "D:\\大学\\计算机图形学\\大作业\\样例\\output\\");
    if(argc == 3){
        QString filename = argv[1];
        QString savepath = argv[2];
        w.openfile(filename, savepath);
        return 0;
    }
    w.show();
    return a.exec();
}
