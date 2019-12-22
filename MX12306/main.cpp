#include "mainwindow.h"

#include <QApplication>

#include "_mx_qt_include.h"
#include "miniblinkwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mxtoolkit::MiniBlinkWidget::wkeInit();

    MainWindow w;
    w.show();

    int ret = a.exec();

    mxtoolkit::MiniBlinkWidget::wkeFinal();

    return a.exec();
}
