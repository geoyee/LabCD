#pragma once

#include <QGraphicsItem>
#include <QPainterPath>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

class LabPolygon;

class LabGrid : public QGraphicsPathItem
{
private:
	const double minSize = 0.8;
	const double maxSize = 1.5;
	QPainterPath* circle;
	QPainterPath* square;

public:
	LabPolygon* annItem;
	bool hovering = false;
	int index;
	QColor color;
	int imgHeight;
	int imgWidth;

	LabGrid(
		LabPolygon* _annItem,
		int _index, 
		QColor _color, 
		int _imgHeight,
		int _imgWidth
	);
	~LabGrid();
	void setColor(QColor c);
	void updateSize(int s = 2);
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
