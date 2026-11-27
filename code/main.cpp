#include "Widget.hpp"
#include <QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置Fusion样式
    a.setStyle(QStyleFactory::create("Fusion"));

    // 加载应用程序翻译文件
    QTranslator appTranslator;
    if (appTranslator.load(":/translations/zh_CN.qm")) {
        a.installTranslator(&appTranslator);
    }

    Widget w;
    w.show();
    w.resize(800,600);

    return a.exec();
}
