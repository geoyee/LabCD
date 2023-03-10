#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QPainter>
#include "labpolygon.h"
#include "opttypes.h"
#include "../utils/label.h"

class AnnotationScence : public QGraphicsScene
{
	Q_OBJECT

private:
	double scaleRate;
	QColor insideColor;
	QColor borderColor;
	QPen* crossPen;
	QPointF* coords;

	bool getItemHovering();
	bool getPolyHovering();
	bool getLineHovering();
	void updateCrossPenSize();

public:
	int labelIndex = -1;
	int imgWidth = 0;
	int imgHeight = 0;
	double opacity = 0.5;
	bool drawing = false;
	QList<LabPolygon*> polygonItems;
	LabPolygon* nowItem = nullptr;

	AnnotationScence();
	~AnnotationScence();
	void resetScence();
	void finished();
	void setScaleRate(double zoomAll);
	void updatePolygonSize();
	void setColor(QColor _insideColor, QColor _borderColor);
	QColor getCrossPenColor();
	void setCrossPenColor(QColor color);
	bool hovering();
	int findFocusPolygon();
	void delPoly(int preFocusIndex);
	void removeAllPolygons();
	void clearAllFocus();
	void clearFocusAndSelected();
	// 同步方法
	void PressedAddPoint(QPointF point);
	// 鼠标事件
	void mousePressEvent(QGraphicsSceneMouseEvent* ev);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev);
	void drawForeground(QPainter* painter, const QRectF& rect);
	void onMouseChanged(double x, double y);

signals:
	void focusRequest(int labelIndex);
	void mouseOptRequest(int polyIndex, int subIndex, OptTypes optType, QEvent* ev);

public slots:
	void getLabel(Label* label);
	void getImageSize(int Width, int Height);
	void copyMouseOpt(int polyIndex, int subIndex, OptTypes optType, QEvent* ev);
};
