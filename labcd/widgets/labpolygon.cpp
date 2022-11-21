#include <QPen>
#include <QCursor>
#include <QGraphicsScene>
#include "annotationscence.h"
#include "labpolygon.h"
#include "opttypes.h"

LabPolygon::LabPolygon(
	AnnotationScence* _nSence,
	int _index,
	int _labelIndex,
	int _imgWidth,
	int _imgHeight,
	QColor _insideColor,
	QColor _borderColor,
	double _opacity
)
{
	// 初始化
	nSence = _nSence;
	index = _index;
	labelIndex = _labelIndex;
	imgWidth = _imgWidth;
	imgHeight = _imgHeight;
	insideColor = _insideColor;
	halfInsideColor = _insideColor;
	borderColor = _borderColor;
	opacity = _opacity;
	// 设置颜色
	setZValue(10);
	insideColor.setAlphaF(opacity);
	halfInsideColor.setAlphaF(opacity / 2);
	setBrush(halfInsideColor);
	borderColor.setAlphaF(0.8);
	setPen(QPen(borderColor));
	setAcceptHoverEvents(true);
	// 其他设置
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setCursor(QCursor(Qt::PointingHandCursor));
}

LabPolygon::~LabPolygon()
{

}

QList<QPointF> LabPolygon::getPointsNotPtr()
{
	QList<QPointF> tmpPoints;
	for (QPointF* p : mPoints)
	{
		tmpPoints.push_back(*p);
	}
	return tmpPoints;
}

int LabPolygon::getLen()
{
	return mPoints.count();
}

void LabPolygon::setColor(QColor _insideColor, QColor _borderColor)
{
	insideColor = _insideColor;
	insideColor.setAlphaF(opacity);
	halfInsideColor = QColor(insideColor);
	halfInsideColor.setAlphaF(opacity / 2);
	setBrush(halfInsideColor);
	borderColor = _borderColor;
	borderColor.setAlphaF(0.8);
	setPen(QPen(borderColor));
	for (LabGrid* item : mItems)
	{
		item->setColor(borderColor);
	}
	for (LabLine* line : mLines)
	{
		line->setColor(borderColor);
	}
}

QColor LabPolygon::getColor()
{
	return borderColor;
}

QList<QPointF*> LabPolygon::getScenePos()
{
	QList<QPointF*> points;
	for (QPointF* p : mPoints)
	{
		p = new QPointF(mapToScene(*p));
		points.push_back(p);
	}
	return points;
}

void LabPolygon::addPointMiddle(int lineIndex, QPointF point)
{
	// 添加点
	LabGrid* gripItem = new LabGrid(this, lineIndex + 1, borderColor, imgHeight, imgWidth);
	gripItem->setEnabled(false);
	gripItem->setPos(point);
	scene()->addItem(gripItem);
	gripItem->updateSize();
	gripItem->setEnabled(true);
	for (int i = lineIndex + 1; i < mItems.count(); i++)
	{
		mItems.at(i)->index += 1;
	}
	mItems.insert(static_cast<qsizetype>(lineIndex) + 1, gripItem);
	QPointF* gripPoint = new QPointF(mapFromScene(point));
	mPoints.insert(static_cast<qsizetype>(lineIndex) + 1, gripPoint);
	setPolygon(QPolygonF(getPointsNotPtr()));
	// 连线
	for (int i = lineIndex + 1; i < mLines.count(); i++)
	{
		mLines.at(i)->index += 1;
	}
	QLineF line1 = QLineF(mapToScene(*(mPoints.at(lineIndex))), point);
	mLines.at(lineIndex)->setLine(line1);
	LabLine* lineItem = new LabLine(this, lineIndex + 1, borderColor);
	QLineF line2 = QLineF(
		point, 
		mapToScene(*(mPoints.at((lineIndex + 2) % getLen())))
	);
	lineItem->setLine(line2);
	mLines.insert(static_cast<qsizetype>(lineIndex) + 1, lineItem);
	scene()->addItem(lineItem);
	lineItem->updateWidth();
}

void LabPolygon::addPointLast(QPointF point)
{
	LabGrid* grip = new LabGrid(this, getLen(), borderColor, imgHeight, imgWidth);
	scene()->addItem(grip);
	mItems.push_back(grip);
	grip->updateSize();
	grip->setPos(point);
	if (getLen() == 0)
	{
		LabLine* line = new LabLine(this, getLen(), borderColor);
		scene()->addItem(line);
		mLines.push_back(line);
		line->setLine(QLineF());
	}
	else
	{
		mLines.at(mLines.count() - 1)->setLine(
			QLineF(*mPoints.at(mPoints.count() - 1), point)
		);
		LabLine* line = new LabLine(this, getLen(), borderColor);
		scene()->addItem(line);
		mLines.push_back(line);
		line->setLine(QLineF(point, *mPoints.at(0)));
	}
	QPointF* nPoint = new QPointF(point);
	mPoints.push_back(nPoint);
	setPolygon(QPolygonF(getPointsNotPtr()));
}

void LabPolygon::remove()
{
	for (LabGrid* grip : mItems)
	{
		scene()->removeItem(grip);
	}
	for (LabLine* line : mLines)
	{
		scene()->removeItem(line);
	}
	while (mItems.count() != 0)
	{
		mItems.pop_back();
	}
	while (mLines.count() != 0)
	{
		mLines.pop_back();
	}
	scene()->polygonItems.removeAll(this);
	scene()->removeItem(this);
	delete this;
}

void LabPolygon::removeFocusPoint(int preFocusIndex)
{
	int focusIndex = -1;
	if (preFocusIndex == -1)
	{
		for (int i = 0; i < mItems.count(); i++)
		{
			if (mItems.at(i)->hasFocus())
			{
				focusIndex = i;
				break;
			}
		}
	}
	else
	{
		focusIndex = preFocusIndex;
	}
	if (focusIndex != -1)
	{
		if (getLen() <= 3)
		{
			remove();
		}
		else
		{
			mPoints.removeAt(focusIndex);
			setPolygon(QPolygonF(getPointsNotPtr()));
			scene()->removeItem(mItems.at(focusIndex));
			mItems.removeAt(focusIndex);
			for (int i = focusIndex; i < mItems.count(); i++)
			{
				mItems.at(i)->index -= 1;
				if (mItems.at(i)->index < 0)
				{
					mItems.at(i)->index += mItems.count();
				}
			}
			scene()->removeItem(mLines.at(focusIndex));
			mLines.removeAt(focusIndex);
			int lastIndex = focusIndex - 1;
			if (lastIndex < 0)
			{
				lastIndex += mItems.count();
			}
			QLineF line(
				mapToScene(*mPoints.at(lastIndex % getLen())),
				mapToScene(*mPoints.at(focusIndex % getLen()))
			);
			mLines.at(lastIndex % getLen())->setLine(line);
			for (int i = focusIndex; i < mLines.count(); i++)
			{
				mLines.at(i)->index -= 1;
				if (mLines.at(i)->index < 0)
				{
					mLines.at(i)->index += mLines.count();
				}
			}
		}
	}
}

void LabPolygon::movePoint(int index, QPointF point)
{
	if (0 <= index && index < mPoints.count())
	{
		mPoints[index] = new QPointF(mapFromScene(point));
		setPolygon(QPolygonF(getPointsNotPtr()));
		moveLine(index);
	}
}

void LabPolygon::moveLine(int index)
{
	if (!noMove)
	{
		QLineF line1 = QLineF(
			mapToScene(*mPoints.at(index)),
			mapToScene(*mPoints.at((index + 1) % getLen()))
		);
		mLines[index]->setLine(line1);
		int lastIndex = index - 1;
		if (lastIndex < 0)
		{
			lastIndex += mPoints.count();
		}
		QLineF line2 = QLineF(
			mapToScene(*mPoints.at(lastIndex % getLen())),
			mapToScene(*mPoints.at(index))
		);
		mLines[lastIndex % getLen()]->setLine(line2);
	}
}

void LabPolygon::moveItem(int index, QPointF point)
{
	if (0 <= index && index < mItems.count())
	{
		mItems[index]->setEnabled(false);
		mItems[index]->setPos(point);
		mItems[index]->setEnabled(true);
		moveLine(index);
	}
}

void LabPolygon::hoverEnterEvent(QGraphicsSceneHoverEvent* ev)
{
	polyHovering = true;
	setBrush(insideColor);
	emit scene()->mouseOptRequest(index, -1, OptTypes::PolyHoverEnter, ev);
	QGraphicsPolygonItem::hoverEnterEvent(ev);
}

void LabPolygon::hoverLeaveEvent(QGraphicsSceneHoverEvent* ev)
{
	polyHovering = false;
	if (!hasFocus())
	{
		setBrush(halfInsideColor);
	}
	emit scene()->mouseOptRequest(index, -1, OptTypes::PolyHoverLeave, ev);
	QGraphicsPolygonItem::hoverLeaveEvent(ev);
}

void LabPolygon::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		scene()->clearSelection();
		setSelected(true);
		emit scene()->mouseOptRequest(index, -1, OptTypes::PolyMousePress, ev);
		QGraphicsPolygonItem::mousePressEvent(ev);
	}
}

void LabPolygon::mouseReleaseEvent(QGraphicsSceneMouseEvent* ev)
{
	setSelected(false);
	emit scene()->mouseOptRequest(index, -1, OptTypes::PolyMouseRelease, ev);
	QGraphicsPolygonItem::mouseReleaseEvent(ev);
}

void LabPolygon::focusInEvent(QFocusEvent* ev)
{
	setBrush(insideColor);
	emit scene()->mouseOptRequest(index, -1, OptTypes::PolyFocusIn, ev);
	QGraphicsPolygonItem::focusInEvent(ev);
}

void LabPolygon::focusOutEvent(QFocusEvent* ev)
{
	if (!polyHovering)
	{
		setBrush(halfInsideColor);
	}
	emit scene()->mouseOptRequest(index, -1, OptTypes::PolyFocusOut, ev);
	QGraphicsPolygonItem::focusOutEvent(ev);
}

QVariant LabPolygon::itemChange(
	GraphicsItemChange change, const QVariant& value)
{
	if (change == QGraphicsItem::ItemPositionHasChanged) 
	{
		for (int i = 0; i < mPoints.count(); i++)
		{
			moveItem(i, mapToScene(*(mPoints.at(i))));
		}
	}
	return QGraphicsPolygonItem::itemChange(change, value);
}

AnnotationScence* LabPolygon::scene()
{
	return nSence;
}
