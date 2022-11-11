#include <QPen>
#include <QCursor>
#include <QGraphicsScene>
#include "labpolygon.h"

LabPolygon::LabPolygon(
	int labelIndex,
	int imgWidth,
	int imgHeight,
	QColor insideColor,
	QColor borderColor,
	double opacity
)
{
	// 初始化
	labelIndex = labelIndex;
	imgWidth = imgWidth;
	imgHeight = imgHeight;
	insideColor = insideColor;
	borderColor = borderColor;
	opacity = opacity;
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

QList<QPoint> LabPolygon::getPoints()
{
	QList<QPoint> tmpPoints;
	for (QPoint* p : mPoints)
	{
		tmpPoints.push_back(*p);
	}
	return tmpPoints;
}

int LabPolygon::getLen()
{
	return mPoints.count();
}

void LabPolygon::setColor(QColor insideColor, QColor borderColor)
{
	insideColor = insideColor;
	insideColor.setAlphaF(opacity);
	halfInsideColor = QColor(insideColor);
	halfInsideColor.setAlphaF(opacity / 2);
	this->setBrush(halfInsideColor);
	borderColor = borderColor;
	borderColor.setAlphaF(0.8);
	this->setPen(QPen(borderColor));
	for (LabGrid* item : mItems)
	{
		item->setColor(borderColor);
	}
	for (LabLine* line : mLines)
	{
		line->setColor(borderColor);
	}
}

QList<QPoint*> LabPolygon::getScenePos()
{
	QList<QPoint*> points;
	for (QPoint* p : mPoints)
	{
		p = new QPoint(this->mapToScene(*p).toPoint());
		points.push_back(p);
	}
	return points;
}

void LabPolygon::addPointMiddle(int lineIndex, QPointF point)
{
	// 添加点
	LabGrid* gripItem = new LabGrid(this, lineIndex, borderColor, imgHeight, imgWidth);
	gripItem->setEnabled(false);
	gripItem->setPos(point);
	this->scene()->addItem(gripItem);
	gripItem->updateSize();
	gripItem->setEnabled(true);
	for (int i = lineIndex + 1; i < mItems.count(); i++)
	{
		mItems.at(i)->setIndex(mItems.at(i)->getIndex() + 1);
	}
	mItems.insert(lineIndex + 1, gripItem);
	QPoint* gripPoint = new QPoint(this->mapToScene(point).toPoint());
	mPoints.insert(lineIndex + 1, gripPoint);
	this->setPolygon(QPolygonF(this->getPoints()));
	// 连线
	for (int i = lineIndex + 1; i < mLines.count(); i++)
	{
		mLines.at(i)->setIndex(mLines.at(i)->getIndex() + 1);
	}
	QLineF line1 = QLineF(this->mapToScene(*(mPoints.at(lineIndex))), point);
	mLines.at(lineIndex)->setLine(line1);
	LabLine* lineItem = new LabLine(this, lineIndex + 1, borderColor);
	QLineF line2 = QLineF(
		point, 
		this->mapToScene(*(mPoints.at((lineIndex + 2) % this->getLen())))
	);
	lineItem->setLine(line2);
	mLines.insert(lineIndex + 1, lineItem);
	this->scene()->addItem(lineItem);
	lineItem->updateWidth();
}

void LabPolygon::addPointLast(QPointF point)
{
	LabGrid* grip = new LabGrid(this, this->getLen(), borderColor, imgHeight, imgWidth);
	this->scene()->addItem(grip);
	mItems.push_back(grip);
	grip->updateSize();
	grip->setPos(point);
	if (this->getLen() == 0)
	{
		LabLine* line = new LabLine(this, this->getLen(), borderColor);
		this->scene()->addItem(line);
		mLines.push_back(line);
		line->setLine(QLineF(point, point));
	}
	else
	{
		mLines.at(mLines.count() - 1)->setLine(
			QLineF(*mPoints.at(mPoints.count() - 1), point));
		LabLine* line = new LabLine(this, this->getLen(), borderColor);
		this->scene()->addItem(line);
		mLines.push_back(line);
		line->setLine(QLineF(point, *mPoints.at(0)));
	}
	QPoint* nPoint = new QPoint(point.toPoint());
	mPoints.push_back(nPoint);
	this->setPolygon(QPolygonF(this->getPoints()));
}

void LabPolygon::remove()
{
	for (LabGrid* grip : mItems)
	{
		this->scene()->removeItem(grip);
	}
	for (LabLine* line : mLines)
	{
		this->scene()->removeItem(line);
	}
	while (mItems.count() != 0)
	{
		mItems.pop_back();
	}
	while (mLines.count() != 0)
	{
		mLines.pop_back();
	}
	// this->scene()->polygonItems.remove(this);  // TODO: 需要在哪儿设置下scene为自定义
	this->scene()->removeItem(this);
	delete this;
}

void LabPolygon::removeFocusPoint()
{
	int focusIndex = -1;
	for (int i = 0; i < mItems.count(); i++)
	{
		if (mItems.at(i)->hasFocus())
		{
			focusIndex = i;
			break;
		}
	}
	if (focusIndex != -1)
	{
		if (this->getLen() <= 3)
		{
			// this->delPolygon(this);  // TODO: 外部传入的删除多边形事件
			return;
		}
		delete mPoints.at(focusIndex);
		this->setPolygon(QPolygonF(this->getPoints()));
		this->scene()->removeItem(mItems.at(focusIndex));
		delete mItems.at(focusIndex);
		for (int i = focusIndex; i < mItems.count(); i++)
		{
			mItems.at(i)->setIndex(mItems.at(i)->getIndex() - 1);
		}
		this->scene()->removeItem(mLines.at(focusIndex));
		delete mLines.at(focusIndex);
		QLineF* line = new QLineF(
			this->mapToScene(mPoints.at((focusIndex - 1) % this->getLen())->toPointF()),
			this->mapToScene(mPoints.at(focusIndex % this->getLen())->toPointF())
		);
		mLines.at((focusIndex - 1) % this->getLen())->setLine(*line);
		for (int i = focusIndex; i < mLines.count(); i++)
		{
			mLines.at(i)->setIndex(mLines.at(i)->getIndex() - 1);
		}
	}
}

void LabPolygon::removeLastPoint()
{
	// 创建的时候需要删除line
	if (this->getLen() == 0)
	{
		mPoints.pop_back();
		this->setPolygon(QPolygonF(this->getPoints()));
		LabGrid* it = mItems.at(mItems.count() - 1);
		mItems.pop_back();
		this->scene()->removeItem(it);
		delete it;
	}
}

void LabPolygon::movePoint(int index, QPointF point)
{
	for (QPoint* iPoint : mPoints)
	{
		QPoint p = this->mapFromScene(point).toPoint();
		iPoint = new QPoint(p);
		this->setPolygon(QPolygonF(this->getPoints()));
		this->moveLine(index);
	}
}

void LabPolygon::moveLine(int index)
{
	if (!noMove)
	{
		QLineF line1 = QLineF(
			*mPoints.at(index),
			*mPoints.at((index + 1) % this->getLen())
		);
		mLines.at(index)->setLine(line1);
		QLineF line2 = QLineF(
			*mPoints.at((index - 1) % this->getLen()),
			*mPoints.at(index)
		);
		mLines.at((index - 1))->setLine(line2);
	}
}

void LabPolygon::moveItem(int index, QPointF point)
{
	for (LabGrid* item : mItems)
	{
		item->setEnabled(false);
		item->setPos(point);
		item->setEnabled(true);
		this->moveLine(index);
	}
}

void LabPolygon::hoverEnterEvent(QGraphicsSceneHoverEvent* ev)
{
	PolyHovering = true;
	this->setBrush(insideColor);
	QGraphicsItem::hoverEnterEvent(ev);
}

void LabPolygon::hoverLeaveEvent(QGraphicsSceneHoverEvent* ev)
{
	PolyHovering = false;
	if (!this->hasFocus())
	{
		this->setBrush(halfInsideColor);
	}
	QGraphicsItem::hoverLeaveEvent(ev);
}

void LabPolygon::focusInEvent(QFocusEvent* ev)
{
	this->setBrush(insideColor);
	QGraphicsItem::focusInEvent(ev);
}

void LabPolygon::focusOutEvent(QFocusEvent* ev)
{
	if (!PolyHovering)
	{
		this->setBrush(halfInsideColor);
	}
	QGraphicsItem::focusOutEvent(ev);
}

QVariant LabPolygon::itemChange(
	QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
	if (change == QGraphicsItem::ItemPositionHasChanged) 
	{
		for (int i = 0; i < mPoints.count(); i++)
		{
			this->moveItem(i, this->mapToScene(*(mPoints.at(i))));
		}
	}
	return QGraphicsItem::itemChange(change, value);
}
