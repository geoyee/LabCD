#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QSizePolicy>
#include <QScrollBar>
#include "canvas.h"

Canvas::Canvas(QWidget *parent)
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
	// 保持滑动滑块的时候也能同步
	connect(aView->horizontalScrollBar(), &QScrollBar::valueChanged, [=](int value) {
		emit syncScroll(value, aView->verticalScrollBar()->value());
	});
	connect(aView->verticalScrollBar(), &QScrollBar::valueChanged, [=](int value) {
		emit syncScroll(aView->horizontalScrollBar()->value(), value);
	});
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
	{
		aView->setZoomAll(scrContHeight);
	}
	else
	{
		aView->setZoomAll(scrContWidth);
	}
	aView->scale(aView->getZoomAll(), aView->getZoomAll());
	aScene->setScaleRate(aView->getZoomAll());
}

void Canvas::loadImageFromPixmap(QPixmap pixmap)
{
	Canvas::resetZoom(pixmap.width(), pixmap.height());
	QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem();
	pixmapItem->setPixmap(pixmap);
	aScene->addItem(pixmapItem);
}

void Canvas::scroolTranslate(int hPos, int vPos)
{
	aView->horizontalScrollBar()->setValue(hPos);
	aView->verticalScrollBar()->setValue(vPos);
}
