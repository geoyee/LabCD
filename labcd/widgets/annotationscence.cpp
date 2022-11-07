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
	/*for (LabPolygon &poly : polygonItems)
	{
		poly.updateSize();
	}*/
}

void AnnotationScence::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
	QGraphicsScene::mousePressEvent(ev);
}

void AnnotationScence::mouseMoveEvent(QGraphicsSceneMouseEvent* ev)
{
	QGraphicsScene::mouseMoveEvent(ev);
}
