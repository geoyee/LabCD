#pragma once

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QFocusEvent>
#include "labgrid.h"
#include "labline.h"

class LabPolygon : public QGraphicsPolygonItem
{
private:
    int labelIndex;
	int imgWidth;
	int imgHeight;
	double opacity = 0.5;
	bool noMove = false;
	bool lastFocse = false;
	QColor insideColor = QColor(255, 0, 0);
	QColor halfInsideColor = QColor(255, 0, 0);
	QColor borderColor = QColor(0, 255, 0);
	// 部件
	QList<QPoint*> mPoints;
	QList<LabGrid*> mItems;
	QList<LabLine*> mLines;

	QList<QPoint> getPoints();  // 将点的指针的list转为点的list

public:
	bool itemHovering = false;
	bool lineHovering = false;
	bool PolyHovering = false;

	LabPolygon(
		int labelIndex,
		int imgWidth,
		int imgHeight,
		QColor insideColor = QColor(255, 0, 0),
		QColor borderColor = QColor(0, 255, 0),
		double opacity = 0.5
	);
	~LabPolygon();
	int getLen();
	void setColor(QColor insideColor, QColor borderColor);
	QList<QPoint*> getScenePos();
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
};
