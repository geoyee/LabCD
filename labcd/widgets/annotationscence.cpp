#include "annotationscence.h"

AnnotationScence::AnnotationScence()
	: QGraphicsScene()
{
	scaleRate = 1.0;
}

AnnotationScence::~AnnotationScence()
{

}

bool AnnotationScence::getItemHovering()
{
	for (LabPolygon* poly : polygonItems)
	{
		if (poly->itemHovering)
		{
			return true;
		}
	}
	return false;
}

bool AnnotationScence::getPolyHovering()
{
	for (LabPolygon* poly : polygonItems)
	{
		if (poly->polyHovering)
		{
			return true;
		}
	}
	return false;
}

bool AnnotationScence::getLineHovering()
{
	for (LabPolygon* poly : polygonItems)
	{
		if (poly->lineHovering)
		{
			return true;
		}
	}
	return false;
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
		{
			grip->updateSize();
		}
		for (LabLine* line : poly->mLines)
		{
			line->updateWidth();
		}
	}
}

void AnnotationScence::setColor(QColor _insideColor, QColor _borderColor)
{
	insideColor = _insideColor;
	borderColor = _borderColor;
}

bool AnnotationScence::hovering()
{
	if (getItemHovering() || getPolyHovering() || getLineHovering())
	{
		return true;
	}
	return false;
}

void AnnotationScence::PressedAddPoint(QPointF point)
{
	if (labelIndex != -1 && imgWidth != 0)
	{
		if (nowItem == nullptr)
		{
			nowItem = new LabPolygon(
				this, polygonItems.count(), labelIndex, \
				imgWidth, imgHeight, insideColor, borderColor, opacity
			);
			addItem(nowItem);
			polygonItems.push_back(nowItem);
		}
		nowItem->addPointLast(point);
	}
}

void AnnotationScence::rightClickedFinshPolygon()
{
	nowItem = nullptr;
}

void AnnotationScence::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
	QPointF p = ev->scenePos();
	if (!hovering())
	{
		// 左键添加
		if (ev->button() == Qt::LeftButton)
		{
			PressedAddPoint(p);
			drawing = true;
		}
		// 右键释放
		else if (ev->button() == Qt::RightButton && drawing)
		{
			rightClickedFinshPolygon();
			drawing = false;
			clearSelection();
		}
	}
	else
	{
		for (LabPolygon* poly : polygonItems)
		{
			if (poly->polyHovering)
			{
				emit focusRequest(poly->labelIndex);
			}
		}
	}
	emit mouseOptRequest(-1, -1, OptTypes::SceneMousePress, ev);
	QGraphicsScene::mousePressEvent(ev);
}

void AnnotationScence::getLabel(Label* label)
{
	labelIndex = label->getIndex();
	insideColor = label->getColor();
	borderColor = label->getColor();
}

void AnnotationScence::getImageSize(int Width, int Height)
{
	imgWidth = Width;
	imgHeight = Height;
}

void AnnotationScence::copyMouseOpt(int polyIndex, int subIndex, OptTypes optType, QEvent* ev)
{
	blockSignals(true);
	switch (optType)
	{
	case OptTypes::SceneMousePress:
		mousePressEvent((QGraphicsSceneMouseEvent*)ev);
		break;
	case OptTypes::PolyHoverEnter:
		if (nowItem != nullptr)
		{
			nowItem->hoverEnterEvent((QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			polygonItems[polyIndex % polygonItems.count()]->hoverEnterEvent(
				(QGraphicsSceneHoverEvent*)ev
			);
		}
		break;
	case OptTypes::PolyHoverLeave:
		if (nowItem != nullptr)
		{
			nowItem->hoverLeaveEvent((QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			polygonItems[polyIndex % polygonItems.count()]->hoverLeaveEvent(
				(QGraphicsSceneHoverEvent*)ev
			);
		}
		break;
	case OptTypes::PolyMousePress:
		if (nowItem != nullptr)
		{
			nowItem->mousePressEvent((QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			polygonItems[polyIndex % polygonItems.count()]->mousePressEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	case OptTypes::PolyMouseRelease:
		if (nowItem != nullptr)
		{
			nowItem->mouseReleaseEvent((QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			polygonItems[polyIndex % polygonItems.count()]->mouseReleaseEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	case OptTypes::PolyFocusIn:
		if (nowItem != nullptr)
		{
			nowItem->focusInEvent((QFocusEvent*)ev);
		}
		else
		{
			polygonItems[polyIndex % polygonItems.count()]->focusInEvent((QFocusEvent*)ev);
		}
		break;
	case OptTypes::PolyFocusOut:
		if (nowItem != nullptr)
		{
			nowItem->focusOutEvent((QFocusEvent*)ev);
		}
		else
		{
			polygonItems[polyIndex % polygonItems.count()]->focusOutEvent((QFocusEvent*)ev);
		}
		break;
	case OptTypes::LineHoverEnter:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->hoverEnterEvent((QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mLines.count();
			polygonItems[polyIndex % polygonItems.count()]->mLines[subIndex % lens]->hoverEnterEvent(
				(QGraphicsSceneHoverEvent*)ev
			);
		}
		break;
	case OptTypes::LineHoverLeave:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->hoverLeaveEvent(
				(QGraphicsSceneHoverEvent*)ev
			);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mLines.count();
			polygonItems[polyIndex % polygonItems.count()]->mLines[subIndex % lens]->hoverLeaveEvent(
				(QGraphicsSceneHoverEvent*)ev
			);
		}
		break;
	case OptTypes::LineMousePress:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->mousePressEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mLines.count();
			polygonItems[polyIndex % polygonItems.count()]->mLines[subIndex % lens]->mousePressEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	case OptTypes::LineMouseRelease:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->mouseReleaseEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mLines.count();
			polygonItems[polyIndex % polygonItems.count()]->mLines[subIndex % lens]->mouseReleaseEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	case OptTypes::LineMouseDoubleClick:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mLines.count();
			nowItem->mLines[subIndex % lens]->mouseDoubleClickEvent((QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mLines.count();
			polygonItems[polyIndex % polygonItems.count()]->mLines[subIndex % lens]->mouseDoubleClickEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	case OptTypes::GridHoverEnter:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->hoverEnterEvent((QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mItems.count();
			polygonItems[polyIndex % polygonItems.count()]->mItems[subIndex % lens]->hoverEnterEvent(
				(QGraphicsSceneHoverEvent*)ev
			);
		}
		break;
	case OptTypes::GridHoverLeave:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->hoverLeaveEvent((QGraphicsSceneHoverEvent*)ev);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mItems.count();
			polygonItems[polyIndex % polygonItems.count()]->mItems[subIndex % lens]->hoverLeaveEvent(
				(QGraphicsSceneHoverEvent*)ev
			);
		}
		break;
	case OptTypes::GridMousePress:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->mousePressEvent((QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mItems.count();
			polygonItems[polyIndex % polygonItems.count()]->mItems[subIndex % lens]->mousePressEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	case OptTypes::GridMouseRelease:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->mouseReleaseEvent((QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mItems.count();
			polygonItems[polyIndex % polygonItems.count()]->mItems[subIndex % lens]->mouseReleaseEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	case OptTypes::GridMouseMove:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->mouseMoveEvent((QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mItems.count();
			polygonItems[polyIndex % polygonItems.count()]->mItems[subIndex % lens]->mouseMoveEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	case OptTypes::GridMouseDoubleClick:
		if (nowItem != nullptr)
		{
			int lens = nowItem->mItems.count();
			nowItem->mItems[subIndex % lens]->mouseDoubleClickEvent((QGraphicsSceneMouseEvent*)ev);
		}
		else
		{
			int lens = polygonItems[polyIndex % polygonItems.count()]->mItems.count();
			polygonItems[polyIndex % polygonItems.count()]->mItems[subIndex % lens]->mouseDoubleClickEvent(
				(QGraphicsSceneMouseEvent*)ev
			);
		}
		break;
	default:
		break;
	}
	blockSignals(false);
}
