﻿#include <QPixmap>
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
	connect(aView, &AnnotationView::syncWheel, this, &Canvas::syncCanvas);
	connect(aView, &AnnotationView::syncMove, this, &Canvas::syncCanvas);
	connect(aView, &AnnotationView::syncScroll, this, &Canvas::syncScroll);
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
	// aScene.scale = aView->getZoomAll();
}

void Canvas::loadImageFromPixmap(QPixmap pixmap)
{
	Canvas::resetZoom(pixmap.width(), pixmap.height());
	QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem();
	pixmapItem->setPixmap(pixmap);
	aScene->addItem(pixmapItem);
}

void Canvas::viewTranslate(int x, int y, double zoom)
{
	aView->setZoomAll(aView->getZoomAll() * zoom);
	aView->scale(zoom, zoom);
	aView->translate(x, y);
}

void Canvas::scroolTranslate(int hPos, int vPos)
{
	aView->horizontalScrollBar()->setValue(hPos);
	aView->verticalScrollBar()->setValue(vPos);
}
