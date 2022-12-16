#pragma once

#include <QWidget>
#include <QListWidget>

class FileList : public QWidget
{
	Q_OBJECT

private:
	QListWidget* fList = nullptr;  // t1Files
	QStringList t2Files;

	QString getGTJsonPath(QString t1Path);

public:
	FileList(QWidget *parent = nullptr);
	~FileList();
	void addFileNames(QStringList t1List, QStringList t2List);
	bool gotoItem(int index);
	bool gotoLastItem();
	bool gotoNextItem();

signals:
	void FileClickRequest(QString t1Path, QString t2Path, QString jsonPath);
};
