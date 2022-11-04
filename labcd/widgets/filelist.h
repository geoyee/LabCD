#pragma once

#include <QWidget>
#include <QListWidget>

class FileList : public QWidget
{
	Q_OBJECT

private:
	QListWidget* fList;
	QStringList t1Files;
	QStringList t2Files;

public:
	FileList(QWidget *parent = nullptr);
	~FileList();
	void addFileNames(QStringList t1List, QStringList t2List);
};
