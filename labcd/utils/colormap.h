#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <QColor>

class ColorMap
{
private:
	int index;
	std::vector<int*> colorList;

	int* strToColor(std::string &str);

public:
	ColorMap();
	~ColorMap();
	void setIndex(int _index);
	QColor getColor();
};