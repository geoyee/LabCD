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
	QPainterPath circlePath;
	QPainterPath squarePath;

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
	void updateSize(double s = 2.0);
	// 事件
	void hoverEnterEvent(QGraphicsSceneHoverEvent* ev);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* ev);
	void mousePressEvent(QGraphicsSceneMouseEvent* ev);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* ev);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* ev);
	// 重写方法
	QVariant itemChange(
		GraphicsItemChange change, const QVariant& value);
	QPainterPath shape();
};
