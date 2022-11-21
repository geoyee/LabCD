#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "labpolygon.h"
#include "opttypes.h"
#include "../utils/label.h"

class AnnotationScence : public QGraphicsScene
{
	Q_OBJECT

private:
	double scaleRate;
	bool drawing = false;
	int labelIndex = -1;
	int imgWidth = 0;
	int imgHeight = 0;
	QColor insideColor;
	QColor borderColor;
	double opacity = 0.5;

	bool getItemHovering();
	bool getPolyHovering();
	bool getLineHovering();

public:
	QList<LabPolygon*> polygonItems;
	LabPolygon* nowItem = nullptr;

	AnnotationScence();
	~AnnotationScence();
	void setScaleRate(double zoomAll);
	void updatePolygonSize();
	void setColor(QColor _insideColor, QColor _borderColor);
	bool hovering();
	void removeFocusPolygon(int preFocusIndex = -1);
	void removeAllPolygons();
	// 同步方法
	void PressedAddPoint(QPointF point);
	void rightClickedFinshPolygon();
	// 鼠标事件
	void mousePressEvent(QGraphicsSceneMouseEvent* ev);

signals:
	void focusRequest(int labelIndex);
	void mouseOptRequest(int polyIndex, int subIndex, OptTypes optType, QEvent* ev);
	void delPolyRequest(int preFocusIndex);

public slots:
	void getLabel(Label* label);
	void getImageSize(int Width, int Height);
	void syncDelPoly(int preFocusIndex);
	void copyMouseOpt(int polyIndex, int subIndex, OptTypes optType, QEvent* ev);
};
