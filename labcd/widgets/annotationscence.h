#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "labpolygon.h"
#include "../utils/label.h"

class AnnotationScence : public QGraphicsScene
{
	Q_OBJECT

private:
	double scaleRate;
	// 多边形
	int labelIndex = -1;
	int imgWidth = 0;
	int imgHeight = 0;
	QColor insideColor;
	QColor borderColor;
	double opacity = 0.5;

public:
	QList<LabPolygon*> polygonItems;
	LabPolygon* nowItem = nullptr;

	AnnotationScence();
	~AnnotationScence();
	void setScaleRate(double zoomAll);
	void updatePolygonSize();
	// 同步方法
	void PressedAddPoint(QPointF point);
	void doubleClickedFinshPolygon();
	// 鼠标事件
	void mousePressEvent(QGraphicsSceneMouseEvent* ev);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* ev);

signals:
	void iPressed(QPointF point);
	void iDoubleClicked();

public slots:
	void getNewLabel(Label* label);
	void getImageSize(int Width, int Height);
};
