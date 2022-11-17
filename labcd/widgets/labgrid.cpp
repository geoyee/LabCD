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
	setPath(circlePath);
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

void LabGrid::updateSize(double s)
{
	double size = minSize;
	circlePath.addEllipse(QRectF(-size, -size, size * s, size * s));
	squarePath.addRect(QRectF(-size, -size, size * s, size * s));
	if (hovering)
	{
		setPath(squarePath);
	}
	else
	{
		setPath(circlePath);
	}
}

void LabGrid::hoverEnterEvent(QGraphicsSceneHoverEvent* ev)
{
	setPath(squarePath);
	setBrush(QColor(0, 0, 0, 0));
	annItem->itemHovering = true;
	hovering = true;
	QGraphicsPathItem::hoverEnterEvent(ev);
}

void LabGrid::hoverLeaveEvent(QGraphicsSceneHoverEvent* ev)
{
	setPath(circlePath);
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
	QGraphicsPathItem::mouseDoubleClickEvent(ev);
}

QVariant LabGrid::itemChange(
	GraphicsItemChange change, const QVariant& value
)
{
	double x, y;
	QVariant tmpVal = value;
	if (change == QGraphicsItem::ItemPositionChange && isEnabled())
	{
		if (tmpVal.toPointF().x() > imgHeight) {
			x = imgHeight;
		}
		else if (tmpVal.toPointF().x() < 0)
		{
			x = 0;
		}
		else
		{
			x = tmpVal.toPointF().x();
		}
		if (tmpVal.toPointF().y() > imgWidth)
		{
			y = imgWidth;
		}
		else if (tmpVal.toPointF().y() < 0)
		{
			y = 0;
		}
		else
		{
			y = tmpVal.toPointF().y();
		}
		tmpVal = QPointF(x, y);
		annItem->movePoint(index, tmpVal.toPointF());
	}
	return QGraphicsPathItem::itemChange(change, tmpVal);
}

QPainterPath LabGrid::shape()
{
	qDebug() << "set shape";
	QPainterPath newPath;
	QPointF p = mapFromScene(pos());
	newPath.addEllipse(p, 2 * minSize, 2 * minSize);
	return newPath;
}
