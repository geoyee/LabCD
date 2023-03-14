#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QSizePolicy>
#include <QScrollBar>
#include <fstream>
#include <json/json.h>
#include "canvas.h"
#include "labpolygon.h"

Canvas::Canvas(QWidget* parent)
	: QScrollArea(parent)
{
	setWidgetResizable(true);
	// 绘图区
	aScene = new AnnotationScence();
	aView = new AnnotationView(aScene, parent);
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	aView->setSizePolicy(sizePolicy);
	aView->setAlignment(Qt::AlignCenter);
	aView->setAutoFillBackground(false);
	aView->setStyleSheet("background-color: White");
	connect(aView, &AnnotationView::mousePosChanged, 
		aScene, &AnnotationScence::onMouseChanged);
	// 保持滑动滑块的时候也能同步
	connect(aView->horizontalScrollBar(), &QScrollBar::valueChanged, [=](int value) {
		emit syncScroll(value, aView->verticalScrollBar()->value());});
	connect(aView->verticalScrollBar(), &QScrollBar::valueChanged, [=](int value) {
		emit syncScroll(aView->horizontalScrollBar()->value(), value);});
	// 加载
	setWidget(aView);
}

Canvas::~Canvas()
{

}

void Canvas::resetZoom(int width, int height)
{
	aScene->setSceneRect(0, 0, width, height);
	// 缩放清除
	aView->scale(1 / aView->getZoomAll(), 1 / aView->getZoomAll());  // 重置缩放
	aView->setZoomAll(1);
	// 最佳缩放
	double scrContWidth = (this->width() * optSize) / width;
	double scrContHeight = (this->height() * optSize) / height;
	if (scrContWidth * height > this->height())
		aView->setZoomAll(scrContHeight);
	else
		aView->setZoomAll(scrContWidth);
	aView->scale(aView->getZoomAll(), aView->getZoomAll());
	aScene->setScaleRate(aView->getZoomAll());
}

void Canvas::loadImageFromPixmap(QPixmap pixmap)
{
	aScene->removeAllPolygons();
	aScene->clear();
	Canvas::resetZoom(pixmap.width(), pixmap.height());
	QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem();
	pixmapItem->setPixmap(pixmap);
	aScene->addItem(pixmapItem);
}

void Canvas::loadJSONFromFile(QString jsonPath)
{
	std::ifstream ifs(jsonPath.toStdString(), std::ios::binary);
	if (!ifs.is_open())
		return;
	Json::Reader reader;
	Json::Value root;
	// 解析json内容
	if (reader.parse(ifs, root))
	{
		for (int i = 0; i < root.size(); ++i)
		{
			QColor jColor = QColor(
				root[i]["color"]["R"].asInt(),
				root[i]["color"]["G"].asInt(),
				root[i]["color"]["B"].asInt()
			);
			int jIndex = root[i]["labelIndex"].asInt();
			emit addJsonPoly(jIndex, jColor);
			int jPointNumber = root[i]["polygon"]["pointNumber"].asInt();
			// 新建多边形
			LabPolygon* nowItem = new LabPolygon(
				aScene, i, jIndex, aScene->imgWidth, aScene->imgHeight,
				jColor, jColor, aScene->opacity
			);
			aScene->addItem(nowItem);
			aScene->polygonItems.push_back(nowItem);
			// 添加点
			for (int j = 0; j < jPointNumber; ++j)
			{
				QPointF* point = new QPointF(
					root[i]["polygon"]["points"][2 * j].asFloat(),
					root[i]["polygon"]["points"][2 * j + 1].asFloat()
				);
				nowItem->addPointLast(*point);
			}
		}
	}
	ifs.close();
}

void Canvas::scroolTranslate(int hPos, int vPos)
{
	aView->horizontalScrollBar()->setValue(hPos);
	aView->verticalScrollBar()->setValue(vPos);
}
