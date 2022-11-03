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
	QTableWidget* labelTable;
	Label* nowLabel;
	ColorMap* cMap;

public:
	LabelTable(QWidget *parent = nullptr);
	~LabelTable();
	void addLabelItem();
};
