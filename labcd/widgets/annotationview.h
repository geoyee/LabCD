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

public:
	AnnotationView(
		AnnotationScence* scence, 
		QWidget *parent = nullptr
	);
	~AnnotationView();
	bool checkZoomAll();
	void setZoomAll(double value);
	double getZoomAll();
	// 事件
	void wheelEvent(QWheelEvent* ev);
	void mouseMoveEvent(QMouseEvent* ev);
	void mousePressEvent(QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	void leaveEvent(QEvent* ev);

signals:
	void mousePosChanged(QPoint point);
	void zoomRequest(double zoomAll);
	// 同步信号
	void syncWheel(int x, int y, double zoom);
	void syncMove(int x, int y, double zoom = 1);
	void syncScroll(int hPos, int vPos);
};
