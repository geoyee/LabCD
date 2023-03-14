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

	void clickItem(int row, int column);
	void createLabelItem(int _index, QString _name, QColor _color);

public:
	LabelTable(QWidget* parent = nullptr);
	~LabelTable();
	int getLen();
	QColor getColorByIndex(int index);
	void addLabelItem(bool init = false);
	void exportLabelToFile(QString path);
	bool importLabelFromFile(QString path);
	void changeLabelDuotoAddPolyJson(int index, QColor color);

signals:
	void labelSelected(Label* nowLabel);
	void colorChanged(int labelIndex, QColor newColor);
};
