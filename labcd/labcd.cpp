#include <iostream>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QGraphicsView>
#include <QAction>
#include <QLabel>
#include <QString>
#include <QDesktopServices>
#include "labcd.h"
#include "utils/fileworker.h"
#include "widgets/annotationview.h"

LabCD::LabCD(QWidget *parent)
    : QMainWindow(parent)
{
    /* 状态栏 */
    QStatusBar* lcdStatusBar = statusBar();
    QLabel* messageState = new QLabel("", this);  // 用于显示消息
    lcdStatusBar->addWidget(messageState);

    /* 菜单栏 */
    QMenuBar* lcdMenuBar = menuBar();
    QMenu* fileMenu = new QMenu("文件", this);
    QAction* opensAct = fileMenu->addAction(
        QIcon(":/menu/resources/Folder.png"), "打开文件夹");
    connect(opensAct, &QAction::triggered, this, &LabCD::openDir);
    lcdMenuBar->addMenu(fileMenu);
    QMenu* aboutMenu = new QMenu("关于", this);
    QAction* githubAct = aboutMenu->addAction(
        QIcon(":/menu/resources/Github.png"), "github主页");
    connect(githubAct, &QAction::triggered, [=]() {
        QDesktopServices::openUrl(QUrl("https://github.com/geoyee/LabCD"));
    });
    lcdMenuBar->addMenu(aboutMenu);

    /* 工具栏 */
    QToolBar* lcdToolBar = new QToolBar(this);
    QAction* saveAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Save.png"), "保存");
    lcdToolBar->addSeparator();
    QAction* lastAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Last.png"), "上一张");
    QAction* nextAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Next.png"), "下一张");
    lcdToolBar->addSeparator();
    QAction* enlargeAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Enlarge.png"), "放大");
    QAction* narrowAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Narrow.png"), "缩小");
    QAction* fullAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Full.png"), "全幅缩放");
    lcdToolBar->addSeparator();
    QAction* undoAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Undo.png"), "撤销");
    QAction* redoAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Redo.png"), "重做");
    lcdToolBar->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, lcdToolBar);

    /* 绘图界面 */
    drawCanvas = new MultCanvas(this);
    connect(drawCanvas->t1Canva->aView, &AnnotationView::mousePosChanged, [=](double x, double y) {
        messageState->setText(
            "当前坐标：" + \
            QString::fromStdString(std::to_string(x)) + ", " + \
            QString::fromStdString(std::to_string(y))
        );
    });
    connect(drawCanvas->t2Canva->aView, &AnnotationView::mousePosChanged, [=](double x, double y) {
        messageState->setText(
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
    connect(fListWidget, &FileList::FileClickRequest, \
        [=](QString t1Path, QString t2Path) {drawCanvas->loadImages(t1Path, t2Path); }
    );  // 加载图像
    filesDock->setWidget(fListWidget);
    addDockWidget(Qt::RightDockWidgetArea, filesDock);

    /* 标签列表 */
    QDockWidget* labelsDock = new QDockWidget("标签列表", this);
    labelsDock->setMinimumWidth(200);
    labelsDock->setAllowedAreas(Qt::RightDockWidgetArea);
    labTableWidget = new LabelTable(this);
    labelsDock->setWidget(labTableWidget);
    addDockWidget(Qt::RightDockWidgetArea, labelsDock);
    connect(
        labTableWidget, &LabelTable::labelSelected, drawCanvas, &MultCanvas::labelSelected);
    
    /* 界面设置 */
    resize(1000, 600);
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
    }
}