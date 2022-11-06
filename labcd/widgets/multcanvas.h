#pragma once

#include <QWidget>
#include "canvas.h"

class MultCanvas : public QWidget
{
	Q_OBJECT

private:
	Canvas* t1Canva = nullptr;
	Canvas* t2Canva = nullptr;

public:
	MultCanvas(QWidget *parent = nullptr);
	~MultCanvas();
	void loadImages(QString t1Path, QString t2Path);
};
