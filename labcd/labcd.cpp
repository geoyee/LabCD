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
#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QtConcurrent/qtconcurrentrun.h>
#include <opencv2/opencv.hpp>
#include "labcd.h"
#include "utils/fileworker.h"
#include "utils/imgpress.h"
#include "widgets/annotationview.h"

LabCD::LabCD(QWidget *parent)
    : QMainWindow(parent)
{
    setting = new QSettings("./configs/setting.ini", QSettings::IniFormat);
    isCN = setting->value("language").toString() != "EN";

    /* 状态栏 */
    QStatusBar* lcdStatusBar = statusBar();
    messageState = new QLabel("", this);  // 用于显示消息
    QLabel* messageLocal = new QLabel("", this);  // 用于显示坐标
    lcdStatusBar->addWidget(messageState);
    lcdStatusBar->addPermanentWidget(messageLocal);

    /* 菜单栏 */
    QMenuBar* lcdMenuBar = menuBar();
    QMenu* fileMenu = new QMenu(tr("文件"), this);
    QAction* opensAct = fileMenu->addAction(
        QIcon(":/menu/resources/Folder.png"), tr("打开文件夹"));
    opensAct->setShortcut(QKeySequence("Ctrl+O"));
    connect(opensAct, &QAction::triggered, this, &LabCD::openDir);
    fileMenu->addSeparator();
    QAction* splitAct = fileMenu->addAction(
        QIcon(":/menu/resources/Split.png"), tr("切分大图"));
    splitAct->setShortcut(QKeySequence("Ctrl+B"));
    connect(splitAct, &QAction::triggered, this, &LabCD::openBigImageFile);
    QAction* mergeAct = fileMenu->addAction(
        QIcon(":/menu/resources/Merge.png"), tr("合并大图"));
    mergeAct->setShortcut(QKeySequence("Ctrl+M"));
    connect(mergeAct, &QAction::triggered, this, &LabCD::mergeBigImage);
    lcdMenuBar->addMenu(fileMenu);
    QMenu* aboutMenu = new QMenu(tr("关于"), this);
    QAction* githubAct = aboutMenu->addAction(
        QIcon(":/menu/resources/Github.png"), tr("github主页"));
    connect(githubAct, &QAction::triggered, [=]() {
        QDesktopServices::openUrl(QUrl("https://github.com/geoyee/LabCD"));
    });
    QAction* helpAct = aboutMenu->addAction(
        QIcon(":/menu/resources/Help.png"), tr("使用帮助"));
    connect(helpAct, &QAction::triggered, [=]() {
        QDesktopServices::openUrl(QUrl(
            "https://github.com/geoyee/LabCD/wiki/%E5%BF%AB%E6%8D%B7%E9%94%AE%E5%88%97%E8%A1%A8"
        ));
    });
    helpAct->setShortcut(QKeySequence("Ctrl+H"));
    lcdMenuBar->addMenu(aboutMenu);
    QMenu* languMenu = new QMenu(tr("语言"), this);
    QAction* setLangeAct;
    if (isCN)
    {
        setLangeAct = languMenu->addAction(
            QIcon(":/menu/resources/English.png"), tr("英文"));
    }
    else
    {
        setLangeAct = languMenu->addAction(
            QIcon(":/menu/resources/Chinese.png"), tr("中文"));
    }
    connect(setLangeAct, &QAction::triggered, [=]() {
        if (isCN)
        {
            setLangeAct->setIcon(QIcon(":/menu/resources/Chinese.png"));
            setLangeAct->setText(tr("中文"));
            setting->setValue("language", "EN");
        }
        else
        {
            setLangeAct->setIcon(QIcon(":/menu/resources/English.png"));
            setLangeAct->setText(tr("英文"));
            setting->setValue("language", "CN");
        }
        isCN = !isCN;
        QMessageBox::information(
            parent,
            QObject::tr("提示"),
            QObject::tr("重启软件后更新语言设置。")
        );
    });
    lcdMenuBar->addMenu(languMenu);

    /* 绘图界面 */
    drawCanvas = new MultCanvas(this);
    connect(drawCanvas->t1Canva->aView, &AnnotationView::mousePosChanged, [=](double x, double y) {
        messageLocal->setText(
            tr("当前坐标：") + \
            QString::fromStdString(std::to_string(x)) + ", " + \
            QString::fromStdString(std::to_string(y))
        );
    });
    connect(drawCanvas->t2Canva->aView, &AnnotationView::mousePosChanged, [=](double x, double y) {
        messageLocal->setText(
            tr("当前坐标：") + \
            QString::fromStdString(std::to_string(x)) + ", " + \
            QString::fromStdString(std::to_string(y))
        );
    });
    setCentralWidget(drawCanvas);

    /* 图像文件列表 */
    QDockWidget* filesDock = new QDockWidget(tr("数据列表"), this);
    filesDock->setMinimumWidth(200);
    filesDock->setAllowedAreas(Qt::RightDockWidgetArea);
    fListWidget = new FileList(this);
    // 保存图像
    connect(fListWidget, &FileList::saveLastFileRequest, this, &LabCD::save);
    connect(fListWidget, &FileList::FileClickRequest, 
        [=](QString t1Path, QString t2Path, QString jsonPath) {
            drawCanvas->loadImages(t1Path, t2Path, jsonPath);
            QFileInfo fileInfo(t1Path);
            fileName = fileInfo.fileName();
            messageState->setText(tr("加载图像：") + t1Path);
    });  // 加载图像
    filesDock->setWidget(fListWidget);
    addDockWidget(Qt::RightDockWidgetArea, filesDock);

    /* 标签列表 */
    QDockWidget* labelsDock = new QDockWidget(tr("标签列表"), this);
    labelsDock->setMinimumWidth(200);
    labelsDock->setAllowedAreas(Qt::RightDockWidgetArea);
    labTableWidget = new LabelTable(this);
    labelsDock->setWidget(labTableWidget);
    connect(labTableWidget, &LabelTable::labelSelected, [=](Label* nowLabel) {
        drawCanvas->labelSelected(nowLabel);
        messageState->setText(tr("当前标签：[") + \
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
        QIcon(":/tools/resources/Save.png"), tr("保存"));
    saveAct->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveAct, &QAction::triggered, this, &LabCD::save);
    lcdToolBar->addSeparator();
    QAction* lastAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Last.png"), tr("上一张"));
    connect(lastAct, &QAction::triggered, [=]() {
        save();
        fListWidget->gotoLastItem();
    });
    lastAct->setShortcut(QKeySequence("S"));
    QAction* nextAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Next.png"), tr("下一张"));
    nextAct->setShortcut(QKeySequence("F"));
    connect(nextAct, &QAction::triggered, [=]() {
        save();
        fListWidget->gotoNextItem();
    });
    lcdToolBar->addSeparator();
    QAction* enlargeAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Enlarge.png"), tr("放大"));
    connect(enlargeAct, &QAction::triggered, [=]() {
        drawCanvas->t1Canva->aView->scaleZoom(1.1);  // 自动同步t2
    });
    QAction* narrowAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Narrow.png"), tr("缩小"));
    connect(narrowAct, &QAction::triggered, [=]() {
        drawCanvas->t1Canva->aView->scaleZoom(0.9);  // 自动同步t2
    });
    QAction* fullAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Full.png"), tr("全幅缩放"));
    fullAct->setShortcut(QKeySequence("Ctrl+F"));
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
        QIcon(":/tools/resources/DeletePolygon.png"), tr("删除多边形"));
    connect(delPolyAct, &QAction::triggered, [=]() {
        int f1Index = drawCanvas->t1Canva->aScene->findFocusPolygon();
        int f2Index = drawCanvas->t2Canva->aScene->findFocusPolygon();
        int delIndex = f1Index > f2Index ? f1Index : f2Index;
        drawCanvas->t1Canva->aScene->delPoly(delIndex);
        drawCanvas->t2Canva->aScene->delPoly(delIndex);
    });
    delPolyAct->setShortcut(QKeySequence("Backspace"));
    QAction* delAllPolysAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/DeleteAllPolygons.png"), tr("删除所有多边形"));
    connect(delAllPolysAct, &QAction::triggered, [=]() {
        drawCanvas->t1Canva->aScene->removeAllPolygons();
        drawCanvas->t2Canva->aScene->removeAllPolygons();
    });
    delAllPolysAct->setShortcut(QKeySequence("Delete"));
    lcdToolBar->addSeparator();
    QAction* crossColorAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Color.png"), tr("设置十字丝颜色"));
    connect(crossColorAct, &QAction::triggered, this, &LabCD::setCrossPenColor);
    lcdToolBar->addSeparator();
    QAction* isCVAAct = lcdToolBar->addAction(
        QIcon(":/tools/resources/Reference.png"), tr("打开变化参考图"));
    isCVAAct->setCheckable(true);
    // 完成
    lcdToolBar->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, lcdToolBar);

    /* 变化参考图 */
    QDockWidget* refDock = new QDockWidget(tr("光谱变化向量强度参考图"), this);
    refDock->setAllowedAreas(Qt::NoDockWidgetArea);
    QLabel* imgRef = new QLabel(this);
    refDock->setFloating(true);
    refDock->hide();
    connect(drawCanvas, &MultCanvas::addimgDiff, [=](cv::Mat imgDiff) {
        if (isCVAAct->isChecked())
        {
            refNewHeight = refNewWidth * imgDiff.rows / imgDiff.cols;
            cv::cvtColor(imgDiff, imgDiff, cv::COLOR_RGB2BGR);
            cv::resize(imgDiff, imgDiff, cv::Size(refNewWidth, refNewHeight));
            QImage qimg = QImage(
                (const uchar*)(imgDiff.data), imgDiff.cols, imgDiff.rows,
                imgDiff.cols * imgDiff.channels(), QImage::Format_RGB888
            );
            imgRef->setPixmap(QPixmap::fromImage(qimg));
            refDock->setFixedSize(refNewWidth, refNewHeight);
            refDock->show();
        } 
    });
    refDock->setWidget(imgRef);
    addDockWidget(Qt::NoDockWidgetArea, refDock);
    
    /* 界面设置 */
    resize(1200, 600);
    setWindowTitle(tr("LabCD - 遥感变化检测标注工具"));
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
        // 加载总进度
        fListWidget->resetProgress();
    }
}

void LabCD::openBigImageFile()
{
    // 获取文件路径
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("打开大图像"), 
        QString(), 
        tr("栅格图像文件 (*.tif *.tiff)")
    );
    if (fileName == "")
    {
        return;
    }
    QString saveDir = QFileInfo(fileName).absolutePath() + QDir::separator() + "split_output";
    saveDir = saveDir.replace("\\", "/");
    FileWorker::createFolder(saveDir);
    // 获取切分大小
    bool blockOk = false;
    int blockSize = QInputDialog::getInt(
        this,
        tr("设置"),
        tr("设置切块大小"),
        512, 1, 2048, 1,
        &blockOk
    );
    if (!blockOk)
    {
        return;
    }
    QtConcurrent::run([=]() {
        if (ImagePress::splitTiff(fileName, saveDir, blockSize, blockSize))
        {
            messageState->setText(tr("切分完成，保存至：") + saveDir);
        }
        else
        {
            messageState->setText(tr("切分失败，可能是不支持的类型或超出范围的切块大小"));
        }
    });
}

void LabCD::mergeBigImage()
{
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        QObject::tr("打开图像文件夹"),
        QString(),
        QFileDialog::ShowDirsOnly
    );
    if (dirPath.isEmpty())
    {
        return;
    }
    QtConcurrent::run([=]() {
        if (ImagePress::mergeTiff(dirPath))
        {
            messageState->setText(
                tr("合并完成，保存至：") + dirPath + "/merge.tif");
        }
        else
        {
            messageState->setText(tr("合并失败"));
        }
    });
}

void LabCD::save()
{
    if (drawCanvas->imageWidth != 0 && drawCanvas->imageHeight != 0)
    {
        QString saveImgPath = savePath + "/GT/" + fileName;
        int labNum = labTableWidget->getLen();
        drawCanvas->finished();
        ImagePress::saveResultFromPolygon(
            saveImgPath,
            labNum,
            drawCanvas->imageHeight,
            drawCanvas->imageWidth,
            drawCanvas->t1Canva->aScene->polygonItems,
            drawCanvas->projs,
            drawCanvas->trans
        );
        fListWidget->finishedCurrentItem();
        messageState->setText(tr("保存图像：") + saveImgPath);
        fListWidget->progressUpAdd();
    }
}

void LabCD::setCrossPenColor()
{
    QColor nowCrossColor = drawCanvas->getCrossPenColor();
    QColor color = QColorDialog::getColor(
        nowCrossColor, this, tr("设置十字丝颜色"), QColorDialog::ShowAlphaChannel
    );
    setting->setValue("cross_color", color);
    drawCanvas->setCrossPenColor(color);
}

void LabCD::closeEvent(QCloseEvent* ev)
{
    // 保存标签
    if (savePath != "")
    {
        QString jsonPath = savePath + "/label.json";
        labTableWidget->exportLabelToFile(jsonPath);
    }
    // 保存界面
    setting->setValue("layout_status", QByteArray(saveState()));
    QMainWindow::closeEvent(ev);
}
