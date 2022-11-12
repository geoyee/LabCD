#pragma once

#include <QWidget>
#include "canvas.h"
#include "../utils/label.h"

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

signals:
	void getLabel(Label* label);
	void getImageSize(int imgWidth, int imgHeight);
};
