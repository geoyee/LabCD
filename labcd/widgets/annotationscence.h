#pragma once

#include <iostream>
#include <vector>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "labpolygon.h"

class AnnotationScence : public QGraphicsScene
{
	Q_OBJECT

private:
	std::vector<LabPolygon> polygonItems;
	double scaleRate;

public:
	AnnotationScence();
	~AnnotationScence();
	void setScaleRate(double zoomAll);
	void updatePolygonSize();
	// 鼠标事件
	void mousePressEvent(QGraphicsSceneMouseEvent* ev);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* ev);
};
