/**
 * 2024春晚魔术模拟器
 * 亲自尝试2024春晚第二个魔术
 * C++ Qt编写，MinGW-w64编译
 * 作者：Carburn Ashroom
 */

#include <QApplication>

#include "mainwindow.h"

#include "calc.h"

int main(int argc, char* argv[])
{
    Calc::init_engines();
    QApplication a {argc, argv};
    MainWindow w;
    w.show();
    return a.exec();
}
