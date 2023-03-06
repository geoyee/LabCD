#include "labcd.h"
#include <QSettings>
#include <QTranslator>
#include <QtWidgets/QApplication>

#include <QDebug>  // debug

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QSettings setting("./configs/setting.ini", QSettings::IniFormat);
    // 居中显示
    QScreen* scr = a.primaryScreen();
    int scr_w = scr->size().width();
    int scr_h = scr->size().height();
    // 加载语言
    QString langu = setting.value("language").toString();
    QTranslator translator;
    if (langu == "EN")
    {
        translator.load(":/translate/i18n/English.qm");
        a.installTranslator(&translator);
    }
    else
    {
        a.removeTranslator(&translator);
    }
    // 重加载界面和设置
    LabCD w;
    w.drawCanvas->setCrossPenColor(setting.value("cross_color").value<QColor>());
    QByteArray layoutStatus = setting.value("layout_status").toByteArray();
    w.restoreState(layoutStatus);
    w.move((scr_w - w.width()) / 2, (scr_h - w.height()) / 2);
    w.show();
    return a.exec();
}
