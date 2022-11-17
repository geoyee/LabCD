#pragma once

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QFocusEvent>
#include "labgrid.h"
#include "labline.h"

class AnnotationScence;

class LabPolygon : public QGraphicsPolygonItem
{
private:
	double opacity = 0.5;
	QColor insideColor = QColor(255, 0, 0);
	QColor halfInsideColor = QColor(255, 0, 0);
	QColor borderColor = QColor(0, 255, 0);

	QList<QPointF> getPointsNotPtr();  // 将点的指针的list转为点的list

public:
	QList<QPointF*> mPoints;
	QList<LabGrid*> mItems;
	QList<LabLine*> mLines;
	int labelIndex;
	int imgWidth;
	int imgHeight;
	bool itemHovering = false;
	bool lineHovering = false;
	bool polyHovering = false;
	bool noMove = false;
	bool lastFocse = false;

	LabPolygon(
		int _labelIndex,
		int _imgWidth,
		int _imgHeight,
		QColor _insideColor = QColor(255, 0, 0),
		QColor _borderColor = QColor(0, 255, 0),
		double _opacity = 0.5
	);
	~LabPolygon();
	int getLen();
	void setColor(QColor _insideColor, QColor _borderColor);
	QList<QPointF*> getScenePos();
	void addPointMiddle(int lineIndex, QPointF point);
	void addPointLast(QPointF point);
	void remove();
	void removeFocusPoint();
	void removeLastPoint();
	void movePoint(int index, QPointF point);
	void moveLine(int index);
	void moveItem(int index, QPointF point);
	// 事件
	void hoverEnterEvent(QGraphicsSceneHoverEvent* ev);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* ev);
	void focusInEvent(QFocusEvent* ev);
	void focusOutEvent(QFocusEvent* ev);
	// 重写
	QVariant itemChange(
		QGraphicsItem::GraphicsItemChange change, const QVariant& value);
	AnnotationScence* scene();
};
