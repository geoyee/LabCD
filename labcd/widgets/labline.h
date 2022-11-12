#pragma once

#include <QGraphicsItem>
#include <QPainterPath>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

class LabPolygon;

class LabLine : public QGraphicsLineItem
{
private:
	const double minWidth = 0.5;
	const double maxWidth = 1;
	int index;
	LabPolygon* annItem;
	QColor color;

public:
	LabLine(
		LabPolygon* _annItem,
		int _index,
		QColor _color
	);
	~LabLine();
	void updateWidth();
	void setColor(QColor c);
	int getIndex();
	void setIndex(int index);
	QPolygonF boundingPolygon();
	// 事件
	void hoverEnterEvent(QGraphicsSceneHoverEvent* ev);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* ev);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev);
	// 重写方法
	QPainterPath shape();
};
