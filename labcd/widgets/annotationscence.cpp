#include "annotationscence.h"

AnnotationScence::AnnotationScence()
	: QGraphicsScene()
{
	scaleRate = 1.0;
}

AnnotationScence::~AnnotationScence()
{

}

void AnnotationScence::setScaleRate(double zoomAll)
{
	scaleRate = zoomAll;
}

void AnnotationScence::updatePolygonSize()
{
	for (LabPolygon* poly : polygonItems)
	{
		for (LabGrid* grip : poly->getItems())
		{
			grip->updateSize();
		}
		for (LabLine* line : poly->getLines())
		{
			line->updateWidth();
		}
	}
}

void AnnotationScence::PressedAddPoint(QPointF point)
{
	if (labelIndex != -1 && imgWidth != 0)
	{
		if (nowItem == nullptr)
		{
			nowItem = new LabPolygon(
				labelIndex, imgWidth, imgHeight, insideColor, borderColor, opacity);
		}
		this->addItem(nowItem);
		nowItem->addPointLast(point);
	}
}

void AnnotationScence::doubleClickedFinshPolygon()
{
	// 双击释放
	polygonItems.push_back(nowItem);
	nowItem = nullptr;
}

void AnnotationScence::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
	QPointF p = ev->scenePos();
	emit iPressed(p);
	PressedAddPoint(p);
	QGraphicsScene::mousePressEvent(ev);
}

void AnnotationScence::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev)
{
	emit iDoubleClicked();
	doubleClickedFinshPolygon();
	QGraphicsScene::mousePressEvent(ev);
}

void AnnotationScence::mouseMoveEvent(QGraphicsSceneMouseEvent* ev)
{
	// polygonItem->movePoint(polygonItems.count() - 1, ev->scenePos());
	QGraphicsScene::mouseMoveEvent(ev);
}

void AnnotationScence::getNewLabel(Label* label)
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
