#pragma once

#include <QtWidgets/QMainWindow>
#include "widgets/labeltable.h"
#include "widgets/filelist.h"

class LabCD : public QMainWindow
{
    Q_OBJECT

private:
    void openDir();

public:
    FileList* fListWidget;  // 数据列表
    LabelTable* labTableWidget;  // 标签列表

    LabCD(QWidget *parent = nullptr);
    ~LabCD();
};
