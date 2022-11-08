#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "labpolygon.h"

class AnnotationScence : public QGraphicsScene
{
	Q_OBJECT

private:
	double scaleRate;

public:
	QList<LabPolygon> polygonItems;

	AnnotationScence();
	~AnnotationScence();
	void setScaleRate(double zoomAll);
	void updatePolygonSize();
	// 鼠标事件
	void mousePressEvent(QGraphicsSceneMouseEvent* ev);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* ev);
};
