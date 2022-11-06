#pragma once

#include <QGraphicsScene>

class AnnotationScence : public QGraphicsScene
{
	Q_OBJECT

public:
	AnnotationScence(QWidget *parent = nullptr);
	~AnnotationScence();
};
