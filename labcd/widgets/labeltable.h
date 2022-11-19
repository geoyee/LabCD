#pragma once

#include <iostream>
#include <vector>
#include <QWidget>
#include <QTableWidget>
#include "../utils/label.h"
#include "../utils/colormap.h"

class LabelTable : public QWidget
{
	Q_OBJECT

private:
	QTableWidget* labelTable = nullptr;
	Label* nowLabel = nullptr;
	ColorMap* cMap = nullptr;

public:
	LabelTable(QWidget *parent = nullptr);
	~LabelTable();
	void addLabelItem(bool init = false);

signals:
	void labelSelected(Label* nowLabel);
	void colorChanged(int labelIndex, QColor newColor);
};
