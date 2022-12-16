#include <QPainter>
#include <QScrollBar>
#include "annotationview.h"

AnnotationView::AnnotationView(
	AnnotationScence* scence, 
	QWidget *parent
)
	: QGraphicsView(scence, parent)
{
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setMouseTracking(true);
	setTransformationAnchor(QGraphicsView::NoAnchor);
	setResizeAnchor(QGraphicsView::NoAnchor);
}

AnnotationView::~AnnotationView()
{

}

double AnnotationView::limitZoom(double min, double now, double max)
{
	if (now > max) 
	{
		return max;
	}
	else if (now < min)
	{
		return min;
	}
	else
	{
		return now;
	}
}

void AnnotationView::sendSyncSignal()
{
	emit syncRequest(
		horizontalScrollBar()->value(),
		verticalScrollBar()->value(),
		transform(),
		zoomAll
	);
}

bool AnnotationView::checkZoomAll()
{
	if (zoomAll < minRange || zoomAll > maxRange)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void  AnnotationView::setZoomAll(double value)
{
	zoomAll = value;
}

double AnnotationView::getZoomAll()
{
	return zoomAll;
}

void AnnotationView::syncTranslate(int hPos, int vPos, QTransform tf, double zoom)
{
	zoomAll = zoom;
	setTransform(tf);
	horizontalScrollBar()->setValue(hPos);
	verticalScrollBar()->setValue(vPos);
}

void AnnotationView::scaleZoom(double _zoom)
{
	zoomAll *= _zoom;
	zoomAll = AnnotationView::limitZoom(minRange, zoomAll, maxRange);
	emit zoomRequest(zoomAll);
	// 限制缩放
	if (AnnotationView::checkZoomAll())
	{
		scale(_zoom, _zoom);
	}
}

void AnnotationView::wheelEvent(QWheelEvent* ev)
{
	if (ev->modifiers() && Qt::ControlModifier)
	{
		double zoom = 1.0 + ev->angleDelta().y() / 2880.0;  // 倍率
		zoomAll *= zoom;
		zoomAll = AnnotationView::limitZoom(minRange, zoomAll, maxRange);
		emit zoomRequest(zoomAll);
		// 限制缩放
		if (AnnotationView::checkZoomAll())
		{
			QPointF oldPos = mapToScene(ev->position().toPoint());
			scale(zoom, zoom);
			QPointF newPos = mapToScene(ev->position().toPoint());
			QPointF delta = newPos - oldPos;
			translate(delta.x(), delta.y());
		}
		ev->ignore();  // 忽略滚动条
	}
	else
	{
		QGraphicsView::wheelEvent(ev);
	}
}

void AnnotationView::mouseMoveEvent(QMouseEvent* ev)
{
	QPointF mousePos = QPointF(mapToScene(ev->pos()));
	emit mousePosChanged(mousePos.x(), mousePos.y());
	// 出现滚动条才能滚动
	if ((middleClicking && (horizontalScrollBar()->isVisible()) || \
		verticalScrollBar()->isVisible()))
	{
		endPos = new QPoint(ev->pos() / zoomAll - *startPos / zoomAll);
		point = new QPoint(*point + *endPos);
		startPos = new QPoint(ev->pos());
		translate(endPos->x(), endPos->y());
	}
	QGraphicsView::mouseMoveEvent(ev);
}

void AnnotationView::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::MiddleButton)
	{
		middleClicking = true;
		startPos = new QPoint(ev->pos());
	}
	QGraphicsView::mousePressEvent(ev);
}

void AnnotationView::mouseReleaseEvent(QMouseEvent* ev)
{
	middleClicking = false;
	QGraphicsView::mouseReleaseEvent(ev);
}

void AnnotationView::scale(qreal sx, qreal sy)
{
	QGraphicsView::scale(sx, sy);
	sendSyncSignal();
}

void AnnotationView::translate(qreal dx, qreal dy)
{
	QGraphicsView::translate(dx, dy);
	sendSyncSignal();
}
