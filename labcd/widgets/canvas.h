#pragma once

#include <QWidget>
#include <QScrollArea>
#include <opencv2/opencv.hpp>
#include "annotationview.h"
#include "annotationscence.h"
#include "../utils/label.h"

class Canvas : public QScrollArea
{
	Q_OBJECT

private:
	double optSize = 0.98;  // 最佳缩放


public:
	AnnotationView* aView = nullptr;
	AnnotationScence* aScene = nullptr;

	Canvas(QWidget* parent = nullptr);
	~Canvas();
	void resetZoom(int width, int height);
	void loadImageFromPixmap(QPixmap pixmap);
	void loadJSONFromFile(QString jsonPath);
	void scroolTranslate(int hPos, int vPos);

signals:
	void syncScroll(int hPos, int vPos);  // 滑块控制
	void addJsonPoly(int index, QColor color);
};
