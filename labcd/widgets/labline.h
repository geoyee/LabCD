#pragma once

#include <QGraphicsItem>
#include <QPainterPath>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

class LabLine : public QGraphicsLineItem
{
private:
	const double minWidth = 0.5;
	const double maxWidth = 1;
	QGraphicsPolygonItem* annItem;
	QColor color;
	int index;

public:
	LabLine(
		QGraphicsPolygonItem* annItem,
		int index,
		QColor color
	);
	~LabLine();
	void updateWidth();
	void setColor(QColor c);
	QPolygonF boundingPolygon();
	// 事件
	void hoverEnterEvent(QGraphicsSceneHoverEvent* ev);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* ev);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev);
	// 重写方法
	QPainterPath shape();
};
