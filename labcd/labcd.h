#pragma once

#include <QtWidgets/QMainWindow>
#include "widgets/labeltable.h"
#include "widgets/filelist.h"
#include "widgets/multcanvas.h"

class LabCD : public QMainWindow
{
    Q_OBJECT

private:
    QString savePath;
    QString fileName;

    void openDir();

public:
    FileList* fListWidget = nullptr;  // 数据列表
    LabelTable* labTableWidget = nullptr;  // 标签列表
    MultCanvas* drawCanvas = nullptr;  // 绘图界面

    LabCD(QWidget *parent = nullptr);
    ~LabCD();
    void closeEvent(QCloseEvent* ev);
};
