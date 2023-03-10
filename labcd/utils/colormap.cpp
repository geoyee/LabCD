#include <QFile>
#include "colormap.h"

int* ColorMap::strToColor(std::string& str)
{
	int* color = new int[3];
	std::size_t startIdx = 0;
	std::size_t endIdx = 0;
	std::string strs = str + ",";
	for (int i = 0; i < 3; i++)
	{
		endIdx = strs.find(",", startIdx);
		std::string cItem = strs.substr(startIdx, endIdx - startIdx);
		startIdx = endIdx + 1;
		color[i] = atoi(cItem.c_str());
	}
	return color;
}

ColorMap::ColorMap()
{
	index = 0;
	QFile file(":/configs/configs/colormap.txt");
	file.open(QIODevice::ReadOnly);
	while (!file.atEnd())
	{
		QByteArray data = file.readLine();
		std::string buff = QString(data).toStdString();
		int* c = ColorMap::strToColor(buff);
		colorList.push_back(c);
	}
	file.close();
}

ColorMap::~ColorMap()
{

}

void ColorMap::setIndex(int _index)
{
	index = _index;
}

QColor ColorMap::getColor()
{
	if (index >= colorList.size())
		index = 0;
	int* c = colorList[index];
	index++;
	return QColor(c[0], c[1], c[2]);
}
