#pragma once

#include <QWidget>
#include "canvas.h"
#include "../utils/label.h"

class MultCanvas : public QWidget
{
	Q_OBJECT

public:
	Canvas* t1Canva = nullptr;
	Canvas* t2Canva = nullptr;
	int imageWidth = 0;
	int imageHeight = 0;
	std::string projs = "";
	double trans[6] = {0, 1, 0, 0, 0, 1};

	MultCanvas(QWidget *parent = nullptr);
	~MultCanvas();
	void loadImages(QString t1Path, QString t2Path, QString jsonPath);
	void clearFocusAndSelected();
	void finished();
	QColor getCrossPenColor();
	void setCrossPenColor(QColor color);

signals:
	void labelSelected(Label* label);
	void imageLoaded(int imgWidth, int imgHeight);
};
