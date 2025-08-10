#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    QApplication::setApplicationName("Markdown Converter");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("Your Company");
    
    MainWindow w;
    w.show();
    return a.exec();
}