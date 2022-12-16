#include <iostream>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QGraphicsView>
#include <QAction>
#include <QString>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMessageBox>
#include "labcd.h"
#include "utils/fileworker.h"
#include "utils/imgpress.h"
#include "widgets/annotationview.h"

LabCD::LabCD(QWidget *parent)
    : QMainWindow(parent)
{
    /* 状态栏 */
    QStatusBar* lcdStatusBar = statusBar();
    messageState = new QLabel("", this);  // 用于显示消息
    QLabel* messageLocal = new QLabel("", this);  // 用于显示坐标
    lcdStatusBar->addWidget(messageState);
    lcdStatusBar->addPermanentWidget(messageLocal);

    /* 菜单栏 */
    QMenuBar* lcdMenuBar = menuBar();
    QMenu* fileMenu = new QMenu("文件", this);
    QAction* opensAct = fileMenu->addAction(
        QIcon(":/menu/resources/Folder.png"), "打开文件夹");
    opensAct->setShortcut(tr("Ctrl+O"));
    connect(opensAct, &QAction::triggered, this, &LabCD::openDir);
    lcdMenuBar->addMenu(fileMenu);
    QMenu* aboutMenu = new QMenu("关于", this);
    QAction* githubAct = aboutMenu->addAction(
        QIcon(":/menu/resources/Github.png"), "github主页");
    connect(githubAct, &QAction::triggered, [=]() {
        QDesktopServices::openUrl(QUrl("https://github.com/geoyee/LabCD"));
    });
    QAction* helpAct = aboutMenu->addAction(
        QIcon(":/menu/resources/Help.png"), "使用帮助");
    connect(helpAct, &QAction::triggered, [=]() {
        QDesktopServices::openUrl(QUrl(
            "https://github.com/geoyee/LabCD/wiki/%E5%BF%AB%E6%8D%B7%E9%94%AE%E5%88%97%E8%A1%A8"
        ));
    });
    helpAct->setShortcut(tr("Ctrl+H"));
    lcdMenuBar->addMenu(aboutMenu);

    /* 绘图界面 */
    drawCanvas = new MultCanvas(this);
    connect(drawCanvas->t1Canva->aView, &AnnotationView::mousePosChanged, [=](double x, double y) {
        messageLocal->setText(
            "当前坐标：" + \
            QString::fromStdString(std::to_string(x)) + ", " + \
            QString::fromStdString(std::to_string(y))
        );
    });
    connect(drawCanvas->t2Canva->aView, &AnnotationView::mousePosChanged, [=](double x, double y) {
        messageLocal->setText(
            "当前坐标：" + \
            QString::fromStdString(std::to_string(x)) + ", " + \
            QString::fromStdString(std::to_string(y))
        );
    });
    setCentralWidget(drawCanvas);

    /* 图像文件列表 */
    QDockWidget* filesDock = new QDockWidget("数据列表", this);
    filesDock->setMinimumWidth(200);
    filesDock->setAllowedAreas(Qt::RightDockWidgetArea);
    fListWidget = new FileList(this);
    connect(fListWidget, &FileList::FileClickRequest, 
        [=](QString t1Path, QString t2Path, QString jsonPath) {
            save();
            drawCanvas->loadImages(t1Path, t2Path, jsonPath);
            QFileInfo fileInfo(t1Path);
            fileName = fileInfo.fileName();
            messageState->setText("加载图像：" + t1Path);
    });  // 加载图像
    filesDock->setWidget(fListWidget);
    addDockWidget(Qt::RightDockWidgetArea, filesDock);

    /* 标签列表 */
    QDockWidget* labelsDock = new QDockWidget("标签列表", this);
    labelsDock->setMinimumWidth(200);
    labelsDock->setAllowedAreas(Qt::RightDockWidgetArea);
    labTableWidget = new LabelTable(this);
    labelsDock->setWidget(labTableWidget);
    connect(labTableWidget, &LabelTable::labelSelected, [=](Label* nowLabel) {
        drawCanvas->labelSelected(nowLabel);
        messageState->setText("当前标签：[" + \
            QString::fromStdString(std::to_string(nowLabel->getIndex())) + \
            "] " + nowLabel->getName());
    });
    connect(labTableWidget, &LabelTable::colorChanged, [=](int labelIndex, QColor newColor) {
        // 更新界面上的多边形
        for (int i = 0; i < drawCanvas->t1Canva->aScene->polygonItems.count(); i++)
        {
            if (drawCanvas->t1Canva->aScene->polygonItems[i]->labelIndex == labelIndex)
            {
                drawCanvas->t1Canva->aScene->polygonItems[i]->setColor(newColor, newColor);
                drawCanvas->t2Canva->aScene->polygonItems[i]->setColor(newColor, newColor);
            }
            drawCanvas->t1Canva->aScene->setColor(newColor, newColor);
            drawCanvas->t2Canva->aScene->setColor(newColor, newColor);
        }
    });
    addDockWidget(Qt::RightDockWidgetArea, labelsDock);

    /* 工具栏 */
    QToolBar* lcdToolBar = new QToolBar(this);
    QAction* saveAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Save.png"), "保存");
    saveAct->setShortcut(tr("Ctrl+S"));
    connect(saveAct, &QAction::triggered, this, &LabCD::save);
    lcdToolBar->addSeparator();
    QAction* lastAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Last.png"), "上一张");
    connect(lastAct, &QAction::triggered, [=]() {
        save();
        fListWidget->gotoLastItem();
    });
    lastAct->setShortcut(tr("S"));
    QAction* nextAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Next.png"), "下一张");
    nextAct->setShortcut(tr("F"));
    connect(nextAct, &QAction::triggered, [=]() {
        save();
        fListWidget->gotoNextItem();
    });
    lcdToolBar->addSeparator();
    QAction* enlargeAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Enlarge.png"), "放大");
    connect(enlargeAct, &QAction::triggered, [=]() {
        drawCanvas->t1Canva->aView->scaleZoom(1.1);  // 自动同步t2
    });
    QAction* narrowAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Narrow.png"), "缩小");
    connect(narrowAct, &QAction::triggered, [=]() {
        drawCanvas->t1Canva->aView->scaleZoom(0.9);  // 自动同步t2
    });
    QAction* fullAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Full.png"), "全幅缩放");
    fullAct->setShortcut(tr("Ctrl+F"));
    connect(fullAct, &QAction::triggered, [=]() {
        if (drawCanvas->imageWidth != 0 && drawCanvas->imageHeight != 0)
        {
            drawCanvas->t1Canva->resetZoom(
                drawCanvas->imageWidth, drawCanvas->imageHeight
            );  // 自动同步t2
        }
    });
    lcdToolBar->addSeparator();
    QAction* delPolyAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/DeletePolygon.png"), "删除多边形");
    connect(delPolyAct, &QAction::triggered, [=]() {
        int f1Index = drawCanvas->t1Canva->aScene->findFocusPolygon();
        int f2Index = drawCanvas->t2Canva->aScene->findFocusPolygon();
        int delIndex = f1Index > f2Index ? f1Index : f2Index;
        drawCanvas->t1Canva->aScene->delPoly(delIndex);
        drawCanvas->t2Canva->aScene->delPoly(delIndex);
    });
    delPolyAct->setShortcut(tr("Backspace"));
    QAction* delAllPolysAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/DeleteAllPolygons.png"), "删除所有多边形");
    connect(delAllPolysAct, &QAction::triggered, [=]() {
        drawCanvas->t1Canva->aScene->removeAllPolygons();
        drawCanvas->t2Canva->aScene->removeAllPolygons();
    });
    delAllPolysAct->setShortcut(tr("Delete"));
    lcdToolBar->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, lcdToolBar);
    
    /* 界面设置 */
    resize(1200, 600);
    setWindowTitle("LabCD - 遥感变化检测标注工具");
    setWindowIcon(QIcon(":/main/resources/Icon.png"));
}

LabCD::~LabCD()
{

}

void LabCD::openDir()
{
    QStringList t1List;
    QStringList t2List;
    if (FileWorker::openImageDir(&t1List, &t2List, this))
    {
        fListWidget->addFileNames(t1List, t2List);
        fListWidget->gotoItem(0);
        // 新建保存目录
        QFileInfo fileInfo(t1List.at(0));
        savePath = fileInfo.path();
        savePath = savePath.replace("\\", "/");
        savePath = savePath.section("/", 0, -2);
        QString saveImgPath = savePath + "/GT";
        FileWorker::createFolder(saveImgPath);
        // 加载已有标签
        QString jsonPath = savePath + "/label.json";
        QFileInfo jsonFileInfo(jsonPath);
        if (jsonFileInfo.isFile())
        {
            labTableWidget->importLabelFromFile(jsonPath);
        }
    }
}

void LabCD::save()
{
    if (drawCanvas->imageWidth != 0 && drawCanvas->imageHeight != 0)
    {
        QString saveImgPath = savePath + "/GT/" + fileName;
        int labNum = labTableWidget->getLen();
        drawCanvas->clearFocusAndSelected();
        ImagePress::saveResultFromPolygon(
            saveImgPath,
            labNum,
            drawCanvas->imageHeight,
            drawCanvas->imageWidth,
            drawCanvas->t1Canva->aScene->polygonItems
        );
        messageState->setText("保存图像：" + saveImgPath);
    }
}

void LabCD::closeEvent(QCloseEvent* ev)
{
    // 保存标签
    QString jsonPath = savePath + "/label.json";
    labTableWidget->exportLabelToFile(jsonPath);
    QMainWindow::closeEvent(ev);
}
