#pragma once

#include <QGraphicsItem>
#include <QPainterPath>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

class LabGrid : public QGraphicsPathItem
{
private:
	const double minSize = 0.8;
	const double maxSize = 1.5;
	bool hovering = true;
	QGraphicsPolygonItem* annItem;
	QColor color;
	int index;
	int imgWidth;
	int imgHeight;
	QPainterPath circle;
	QPainterPath square;

public:
	LabGrid(
		QGraphicsPolygonItem* annItem, 
		int index, 
		QColor color, 
		int imgWidth, 
		int imgHeight
	);
	~LabGrid();
	void updateSize(int s = 2);
	void setColor(QColor c);
	// 事件
	void hoverEnterEvent(QGraphicsSceneHoverEvent* ev);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* ev);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* ev);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev);
	// 重写方法
	QVariant itemChange(
		QGraphicsItem::GraphicsItemChange change, const QVariant &value);
	QPainterPath shape();
};
