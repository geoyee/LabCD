#pragma once

#include <iostream>
#include <QString>
#include <QColor>

class Label
{
private:
	int index;
	QString name;
	QColor color;

public:
	Label();
	Label(int index, QString name, QColor color);
	~Label();
	void init();
	void reSet(int index, QString name, QColor color);
	void setIndex(int index);
	int getIndex();
	void setName(QString name);
	QString getName();
	void setColor(QColor color);
	QColor getColor();
};
