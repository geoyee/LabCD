﻿#include "labcd.h"
#include <QSettings>
#include <QTranslator>
#include <QtWidgets/QApplication>

#include <QDebug>  // debug

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QSettings setting("./configs/setting.ini", QSettings::IniFormat);

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
    w.show();
    return a.exec();
}
