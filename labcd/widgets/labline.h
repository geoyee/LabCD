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
	
public:
	LabPolygon* annItem;
	int index;
	QColor color;

	LabLine(
		LabPolygon* _annItem,
		int _index,
		QColor _color
	);
	~LabLine();
	void setColor(QColor c);
	void updateWidth();
	QPolygonF boundingPolygon();
	// 事件
	void hoverEnterEvent(QGraphicsSceneHoverEvent* ev);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* ev);
	void mousePressEvent(QGraphicsSceneMouseEvent* ev);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* ev);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev);
	// 重写方法
	QPainterPath shape();
};
