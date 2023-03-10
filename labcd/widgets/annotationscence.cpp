#include <iostream>
#include <algorithm>
#include "annotationscence.h"

AnnotationScence::AnnotationScence()
	: QGraphicsScene()
{
	scaleRate = 1.0;
	// 十字丝
	crossPen = new QPen();
	crossPen->setWidth(2);
	crossPen->setColor(QColor(0, 0, 0, 127));
	coords = nullptr;
}

AnnotationScence::~AnnotationScence()
{

}

bool AnnotationScence::getItemHovering()
{
	for (LabPolygon* poly : polygonItems)
	{
		if (poly->itemHovering)
			return true;
	}
	return false;
}

bool AnnotationScence::getPolyHovering()
{
	for (LabPolygon* poly : polygonItems)
	{
		if (poly->polyHovering)
			return true;
	}
	return false;
}

bool AnnotationScence::getLineHovering()
{
	for (LabPolygon* poly : polygonItems)
	{
		if (poly->lineHovering)
			return true;
	}
	return false;
}

void AnnotationScence::updateCrossPenSize()
{
	crossPen->setWidth(std::max(1, int(2 / scaleRate + 1e-12)));
}

void AnnotationScence::resetScence()
{
	nowItem = nullptr;
	drawing = false;
	clearFocusAndSelected();
}

void AnnotationScence::finished()
{
	if (nowItem != nullptr)
	{
		if (drawing == true && nowItem->getLen() < 3)
			nowItem->remove();
	}
	resetScence();  // 清理和置零
}

void AnnotationScence::setScaleRate(double zoomAll)
{
	scaleRate = zoomAll;
}

void AnnotationScence::updatePolygonSize()
{
	for (LabPolygon* poly : polygonItems)
	{
		for (LabGrid* grip : poly->mItems)
			grip->updateSize();
		for (LabLine* line : poly->mLines)
			line->updateWidth();
	}
}

void AnnotationScence::setColor(QColor _insideColor, QColor _borderColor)
{
	insideColor = _insideColor;
	borderColor = _borderColor;
}

QColor AnnotationScence::getCrossPenColor()
{
	return crossPen->color();
}

void AnnotationScence::setCrossPenColor(QColor color)
{
	crossPen->setColor(color);
}

bool AnnotationScence::hovering()
{
	if (getItemHovering() || getPolyHovering() || getLineHovering())
	{
		return true;
	}
	return false;
}

int AnnotationScence::findFocusPolygon()
{
	int focusIndex = -1;
	for (int i = 0; i < polygonItems.count(); ++i)
	{
		if (polygonItems.at(i)->hasFocus())
		{
			focusIndex = i;
			break;
		}
	}
	return focusIndex;
}

void AnnotationScence::delPoly(int index)
{
	if (index != -1)
		polygonItems[index]->remove();
}

void AnnotationScence::removeAllPolygons()
{
	int numPoly = polygonItems.count();
	for (int i = numPoly - 1; i >= 0; --i)
		delPoly(i);
}

void AnnotationScence::clearAllFocus()
{
	for (QGraphicsItem* obj : items())
	{
		if (obj->hasFocus())
			obj->clearFocus();
	}
}

void AnnotationScence::clearFocusAndSelected()
{
	clearAllFocus();
	clearSelection();
}

void AnnotationScence::PressedAddPoint(QPointF point)
{
	if (imgWidth != 0)
	{
		if (nowItem == nullptr)
		{
			nowItem = new LabPolygon(
				this, polygonItems.count(), labelIndex,
				imgWidth, imgHeight, insideColor, borderColor, opacity
			);
			addItem(nowItem);
			polygonItems.push_back(nowItem);
		}
		nowItem->addPointLast(point);
		// 选择
		clearFocusAndSelected();
		nowItem->mItems[nowItem->mItems.count() - 1]->setSelected(true);
	}
}

void AnnotationScence::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
	// 右键或点击多边形激活则完成
	if (ev->button() == Qt::RightButton || (hovering() && drawing))
	{
		finished();
		emit mouseOptRequest(-1, -1, OptTypes::SceneMousePress, ev);
	}
	// 其他操作
	else
	{
		QPointF p = ev->scenePos();
		if (p.x() < 0 || p.x() > imgWidth || p.y() < 0 || p.y() > imgHeight)
			return;  // 超出范围
		if (!hovering())  // 非激活状态
		{
			// 当选择有标签的时候，左键添加
			if (ev->button() == Qt::LeftButton && labelIndex != -1)
			{
				PressedAddPoint(p);
				drawing = true;
				emit mouseOptRequest(-1, -1, OptTypes::SceneMousePress, ev);
			}
		}
		else
		{
			for (LabPolygon* poly : polygonItems)
			{
				if (poly->polyHovering)
					emit focusRequest(poly->labelIndex);
			}
		}
		QGraphicsScene::mousePressEvent(ev);
	}
}

void AnnotationScence::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev)
{
	QGraphicsItem* nowObj = itemAt(ev->scenePos(), QTransform());
	switch ((int)nowObj->zValue())
	{
	case 10:  // 使双击对多边形无效
		break;
	default:
		QGraphicsScene::mouseDoubleClickEvent(ev);
		break;
	}
}

void AnnotationScence::getLabel(Label* label)
{
	// 完成之前的
	finished();
	// 新设定
	labelIndex = label->getIndex();
	insideColor = label->getColor();
	borderColor = label->getColor();
}

void AnnotationScence::getImageSize(int Width, int Height)
{
	imgWidth = Width;
	imgHeight = Height;
}

void AnnotationScence::copyMouseOpt(
	int polyIndex, int subIndex, OptTypes optType, QEvent* ev)
{
	blockSignals(true);
	switch (optType)
	{
	case OptTypes::SceneMousePress:
		mousePressEvent((QGraphicsSceneMouseEvent*)ev);
		break;
	case OptTypes::SceneMouseDoubleClick:
		mouseDoubleClickEvent((QGraphicsSceneMouseEvent*)ev);
		break;
	case OptTypes::PolyHoverEnter:
		if (nowItem != nullptr)
			nowItem->hoverEnterEvent((QGraphicsSceneHoverEvent*)ev);
		else
		{
			if (polygonItems.count() != 0)
				polygonItems[polyIndex % polygonItems.count()]->hoverEnterEvent(
					(QGraphicsSceneHoverEvent*)ev);
		}
		break;
	case OptTypes::PolyHoverLeave:
		if (nowItem != nullptr)
			nowItem->hoverLeaveEvent((QGraphicsSceneHoverEvent*)ev);
		else
		{
			if (polygonItems.count() != 0)
				polygonItems[polyIndex % polygonItems.count()]->hoverLeaveEvent(
					(QGraphicsSceneHoverEvent*)ev);
		}
		break;
	case OptTypes::PolyMousePress:
		if (nowItem != nullptr)
			nowItem->mousePressEvent((QGraphicsSceneMouseEvent*)ev);
		else
		{
			if (polygonItems.count() != 0)
				polygonItems[polyIndex % polygonItems.count()]->mousePressEvent(
					(QGraphicsSceneMouseEvent*)ev);
		}
		break;
	case OptTypes::PolyMouseRelease:
		if (nowItem != nullptr)
			nowItem->mouseReleaseEvent((QGraphicsSceneMouseEvent*)ev);
		else
		{
			if (polygonItems.count() != 0)
				polygonItems[polyIndex % polygonItems.count()]->mouseReleaseEvent(
					(QGraphicsSceneMouseEvent*)ev);
		}
		break;
	case OptTypes::PolyFocusIn:
		if (nowItem != nullptr)
			nowItem->focusInEvent((QFocusEvent*)ev);
		else
		{
			if (polygonItems.count() != 0)
				polygonItems[polyIndex % polygonItems.count()]->focusInEvent(
					(QFocusEvent*)ev);
		}
		break;
	case OptTypes::PolyFocusOut:
		if (nowItem != nullptr)
			nowItem->focusOutEvent((QFocusEvent*)ev);
		else
		{
			if (polygonItems.count() != 0)
				polygonItems[polyIndex % polygonItems.count()]->focusOutEvent(
					(QFocusEvent*)ev);
		}
		break;
	case OptTypes::LineHoverEnter:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->hoverEnterEvent(
				(QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mLines.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mLines[subIndex % lens]-> \
					hoverEnterEvent((QGraphicsSceneHoverEvent*)ev);
			}
		}
		break;
	case OptTypes::LineHoverLeave:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->hoverLeaveEvent(
				(QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mLines.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mLines[subIndex % lens]-> \
					hoverLeaveEvent((QGraphicsSceneHoverEvent*)ev);
			}
		}
		break;
	case OptTypes::LineMousePress:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->mousePressEvent(
				(QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mLines.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mLines[subIndex % lens]-> \
					mousePressEvent((QGraphicsSceneMouseEvent*)ev);
			}
		}
		break;
	case OptTypes::LineMouseRelease:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->mouseReleaseEvent(
				(QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mLines.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mLines[subIndex % lens]-> \
					mouseReleaseEvent((QGraphicsSceneMouseEvent*)ev);
			}
		}
		break;
	case OptTypes::LineMouseDoubleClick:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->mouseDoubleClickEvent(
				(QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mLines.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mLines[subIndex % lens]-> \
					mouseDoubleClickEvent((QGraphicsSceneMouseEvent*)ev);
			}
		}
		break;
	case OptTypes::GridHoverEnter:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->hoverEnterEvent(
				(QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mItems.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mItems[subIndex % lens]-> \
					hoverEnterEvent((QGraphicsSceneHoverEvent*)ev);
			}
		}
		break;
	case OptTypes::GridHoverLeave:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->hoverLeaveEvent(
				(QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mItems.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mItems[subIndex % lens]-> \
					hoverLeaveEvent((QGraphicsSceneHoverEvent*)ev);
			}
		}
		break;
	case OptTypes::GridMousePress:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->mousePressEvent(
				(QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mItems.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mItems[subIndex % lens]-> \
					mousePressEvent((QGraphicsSceneMouseEvent*)ev);
			}
		}
		break;
	case OptTypes::GridMouseRelease:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->mouseReleaseEvent(
				(QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mItems.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mItems[subIndex % lens]-> \
					mouseReleaseEvent((QGraphicsSceneMouseEvent*)ev);
			}
		}
		break;
	case OptTypes::GridMouseMove:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->mouseMoveEvent(
				(QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mItems.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mItems[subIndex % lens]-> \
					mouseMoveEvent((QGraphicsSceneMouseEvent*)ev);
			}
		}
		break;
	case OptTypes::GridMouseDoubleClick:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->mouseDoubleClickEvent(
				(QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			if (polygonItems.count() != 0)
			{
				int lens = polygonItems[polyIndex % polygonItems.count()]-> \
					mItems.count();
				polygonItems[polyIndex % polygonItems.count()]-> \
					mItems[subIndex % lens]-> \
					mouseDoubleClickEvent((QGraphicsSceneMouseEvent*)ev);
			}
		}
		break;
	default:
		break;
	}
	blockSignals(false);
}

void AnnotationScence::drawForeground(QPainter* painter, const QRectF& rect)
{
	if (coords != nullptr && *coords != QPointF(-1, -1))
	{
		painter->setClipRect(rect);
		painter->setPen(*crossPen);
		painter->drawLine(
			int(coords->x()),
			int(rect.top()), int(coords->x()), int(rect.bottom() + 1)
		);
		painter->drawLine(
			int(rect.left()),
			int(coords->y()),
			int(rect.right() + 1), int(coords->y())
		);
	}
}

void AnnotationScence::onMouseChanged(double x, double y)
{
	coords = new QPointF(x, y);
	invalidate();
}
