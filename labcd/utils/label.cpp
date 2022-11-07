#include "label.h"

Label::Label()
{
	Label::init();
}

Label::Label(int index, QString name, QColor color)
{
	Label::reSet(index, name, color);
}

Label::~Label()
{

}

void Label::init()
{
	this->index = -1;
	this->name = "";
	this->color = QColor();
}

void Label::reSet(int index, QString name, QColor color)
{
	this->index = index;
	this->name = name;
	this->color = color;
}

void Label::setIndex(int index)
{
	this->index = index;
}

int Label::getIndex()
{
	return this->index;
}

void Label::setName(QString name)
{
	this->name = name;
}

QString Label::getName()
{
	return this->name;
}

void Label::setColor(QColor color)
{
	this->color = color;
}

QColor Label::getColor()
{
	return this->color;
}
