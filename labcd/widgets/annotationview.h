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
	double minRange = 0.02;
	double maxRange = 50;
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
	// 重写鼠标事件
	void wheelEvent(QWheelEvent* ev);
	void mouseMoveEvent(QMouseEvent* ev);
	void mousePressEvent(QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	void leaveEvent(QEvent* ev);
	// 重写变换方法，发送信号
	void scale(qreal sx, qreal sy);
	void translate(qreal dx, qreal dy);

signals:
	void mousePosChanged(QPoint point);
	void zoomRequest(double zoomAll);
	// 同步信号
	void syncRequest(int hPos, int vPos, QTransform tf, double zoom);
};
