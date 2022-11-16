#include <QPen>
#include <QCursor>
#include "labpolygon.h"
#include "labgrid.h"

LabGrid::LabGrid(
	LabPolygon* _annItem,
	int _index,
	QColor _color,
	int _imgHeight,
	int _imgWidth
)
{
	// 初始化
	annItem = _annItem;
	index = _index;
	color = _color;
	color.setAlphaF(1.0);
	imgHeight = _imgHeight;
	imgWidth = _imgWidth;
	// 设置
	updateSize();
	setPath(*circle);
	setBrush(color);
	setPen(QPen(color, 1));
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setAcceptHoverEvents(true);
	setZValue(12);
	setCursor(QCursor(Qt::PointingHandCursor));
}

LabGrid::~LabGrid()
{

}

void LabGrid::setColor(QColor c)
{
	setBrush(c);
	setPen(QPen(c, 1));
	color = c;
}

void LabGrid::updateSize(int s)
{
	double size = minSize;
	circle = new QPainterPath();
	circle->addEllipse(QRectF(-size, -size, size * s, size * s));
	square = new QPainterPath();
	square->addRect(QRectF(-size, -size, size * s, size * s));
	if (hovering)
	{
		setPath(*square);
	}
	else
	{
		setPath(*circle);
	}
}

void LabGrid::hoverEnterEvent(QGraphicsSceneHoverEvent* ev)
{
	setPath(*square);
	setBrush(QColor(0, 0, 0, 0));
	annItem->itemHovering = true;
	hovering = true;
	QGraphicsPathItem::hoverEnterEvent(ev);
}

void LabGrid::hoverLeaveEvent(QGraphicsSceneHoverEvent* ev)
{
	setPath(*circle);
	setBrush(color);
	annItem->itemHovering = false;
	hovering = false;
	QGraphicsPathItem::hoverLeaveEvent(ev);
}

void LabGrid::mouseReleaseEvent(QGraphicsSceneMouseEvent* ev)
{
	setSelected(false);
	QGraphicsPathItem::mouseReleaseEvent(ev);
}

void LabGrid::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev)
{
	annItem->removeFocusPoint();
}

QVariant LabGrid::itemChange(
	QGraphicsItem::GraphicsItemChange change, const QVariant& value
)
{
	double x, y;
	QPointF valuePoint = value.toPointF();
	QPointF tmpVal = valuePoint;
	if (change == QGraphicsItem::ItemPositionChange && isEnabled())
	{
		if (valuePoint.x() > imgHeight) {
			x = imgHeight;
		}
		else if (valuePoint.x() < 0)
		{
			x = 0;
		}
		else
		{
			x = valuePoint.x();
		}
		if (valuePoint.y() > imgWidth)
		{
			y = imgWidth;
		}
		else if (valuePoint.y() < 0)
		{
			y = 0;
		}
		else
		{
			y = valuePoint.y();
		}
		tmpVal = QPointF(x, y);
		annItem->movePoint(index, tmpVal);
		// annItem->setDirty(true);
	}
	return QGraphicsPathItem::itemChange(change, (QVariant)tmpVal);
}

QPainterPath LabGrid::shape()
{
	QPainterPath path;
	double x, y;
	QPointF p = mapFromScene(pos());
	x = p.x();
	y = p.y();
	double size = minSize;
	path.addEllipse(p, size + minSize, size + minSize);
	return path;
}
