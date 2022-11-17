﻿#include "annotationscence.h"

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
				this, labelIndex, imgWidth, imgHeight, insideColor, borderColor, opacity
			);
			addItem(nowItem);
		}
		nowItem->addPointLast(point);
	}
}

void AnnotationScence::rightClickedFinshPolygon()
{
	// 右键释放
	polygonItems.push_back(nowItem);
	nowItem = nullptr;
	drawing = false;
}

void AnnotationScence::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
	QPointF p = ev->scenePos();
	if (!hovering())
	{
		if (ev->button() == Qt::LeftButton)
		{
			PressedAddPoint(p);
			drawing = true;
		}
		else if (ev->button() == Qt::RightButton && drawing)
		{
			rightClickedFinshPolygon();
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
	emit mouseOpt("mousePress", ev);
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

void AnnotationScence::copyMouseOpt(QString type, QGraphicsSceneMouseEvent* ev)
{
	if (type == "mousePress")
	{
		blockSignals(true);
		mousePressEvent(ev);
		blockSignals(false);
	}
}
