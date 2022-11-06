#pragma once

#include <QWidget>
#include <QScrollArea>
#include <opencv2/opencv.hpp>
#include "annotationview.h"
#include "annotationscence.h"

class Canvas : public QScrollArea
{
	Q_OBJECT

private:
	double optSize = 0.98;  // 最佳缩放
	AnnotationView* aView = nullptr;
	AnnotationScence* aScene = nullptr;

public:
	Canvas(QWidget *parent = nullptr);
	~Canvas();
	void resetZoom(int width, int height);
	void loadImageFromPixmap(QPixmap pixmap);
	void viewTranslate(int x, int y, double zoom = 1);
	void scroolTranslate(int hPos, int vPos);

signals:
	void syncCanvas(int x, int y, double zoom);
	void syncScroll(int hValue, int vValue);
};
