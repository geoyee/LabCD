#include "annotationscence.h"

AnnotationScence::AnnotationScence()
	: QGraphicsScene()
{
	scaleRate = 1.0;
}

AnnotationScence::~AnnotationScence()
{

}

void AnnotationScence::finished()
{
	if (drawing == true)
	{
		if (nowItem->getLen() >= 3)
		{
			rightClickedFinshPolygon();
		}
		else  // 三个点以下不构成多边形
		{
			nowItem->remove();
			nowItem = nullptr;
		}
		drawing = false;
	}
	clearFocusAndSelected();
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

int AnnotationScence::findFocusPolygon()
{
	int focusIndex = -1;
	for (int i = 0; i < polygonItems.count(); i++)
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
	{
		polygonItems[index]->remove();
	}
	update();
}

void AnnotationScence::removeAllPolygons()
{
	int numPoly = polygonItems.count();
	for (int i = numPoly - 1; i >= 0; i--)
	{
		delPoly(i);
	}
}

void AnnotationScence::clearAllFocus()
{
	for (QGraphicsItem* obj : items())
	{
		if (obj->hasFocus())
		{
			obj->clearFocus();
		}
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
				this, polygonItems.count(), labelIndex, \
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

void AnnotationScence::rightClickedFinshPolygon()
{
	nowItem = nullptr;
}

void AnnotationScence::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
	// 没有选择标签视为无效点击
	if (labelIndex == -1)
	{
		return;
	}
	// 右键一定清理
	if (ev->button() == Qt::RightButton)
	{
		clearFocusAndSelected();
		emit mouseOptRequest(-1, -1, OptTypes::SceneMousePress, ev);
	}
	// 进一步操作
	QPointF p = ev->scenePos();
	if (!hovering())
	{
		// 左键添加
		if (ev->button() == Qt::LeftButton)
		{
			PressedAddPoint(p);
			drawing = true;
			emit mouseOptRequest(-1, -1, OptTypes::SceneMousePress, ev);
		}
		// 右键释放
		else if ((ev->button() == Qt::RightButton) && drawing)
		{
			finished();
			if (drawing)  // 避免无效同步释放
			{
				return;
			}
			emit mouseOptRequest(-1, -1, OptTypes::SceneMousePress, ev);
		}
		else  // 基础操作
		{
			QGraphicsScene::mousePressEvent(ev);
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
		// 避免一边创建点一边激活多边形引起的顺序问题，目前将在激活多边形时强制结束绘制状态
		if (drawing)
		{
			finished();
			if (drawing)  // 避免无效同步释放
			{
				return;
			}
			emit mouseOptRequest(-1, -1, OptTypes::SceneMousePress, ev);
		}
		else
		{
			QGraphicsScene::mousePressEvent(ev);
		}
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
	// 清理
	clearFocusAndSelected();
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
	case OptTypes::SceneMouseDoubleClick:
		mouseDoubleClickEvent((QGraphicsSceneMouseEvent*)ev);
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
