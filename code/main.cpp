#include "Widget.hpp"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置Fusion样式
    a.setStyle(QStyleFactory::create("Fusion"));

    Widget w;
    w.show();
    w.resize(800,600);

    return a.exec();
}
