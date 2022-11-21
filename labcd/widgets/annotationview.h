#pragma once

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QEvent>
#include "annotationscence.h"

class AnnotationView : public QGraphicsView
{
	Q_OBJECT

private:
	QPoint* point = new QPoint(0, 0);
	QPoint* startPos = new QPoint(0, 0);
	QPoint* endPos = new QPoint(0, 0);
	bool middleClicking = false;  // 中间用于移动
	const double minRange = 0.02;
	const double maxRange = 50;
	double zoomAll = 1;
	
	double limitZoom(double min, double now, double max);
	void sendSyncSignal();

public:
	AnnotationView(
		AnnotationScence* scence, 
		QWidget *parent = nullptr
	);
	~AnnotationView();
	bool checkZoomAll();
	void setZoomAll(double value);
	double getZoomAll();
	void syncTranslate(int hPos, int vPos, QTransform tf, double zoom);
	void scaleZoom(double _zoom);
	// 重写鼠标事件
	void wheelEvent(QWheelEvent* ev);
	void mouseMoveEvent(QMouseEvent* ev);
	void mousePressEvent(QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	// 重写变换方法，发送信号
	void scale(qreal sx, qreal sy);
	void translate(qreal dx, qreal dy);

signals:
	void zoomRequest(double zoomAll);
	void mousePosChanged(double x, double y);
	// 同步信号给另一个Canvas
	void syncRequest(int hPos, int vPos, QTransform tf, double zoom);
};
