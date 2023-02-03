#pragma once

#include <QWidget>
#include <QListWidget>
#include <QProgressBar>

class FileList : public QWidget
{
	Q_OBJECT

private:
	QListWidget* fList = nullptr;  // t1Files
	QProgressBar* progressBar = nullptr;  // 进度条
	QStringList t2Files;
	int nowIndex = 0;
	int finishedNumber = 0;

	QString getGTJsonPath(QString t1Path);

public:
	FileList(QWidget *parent = nullptr);
	~FileList();
	void addFileNames(QStringList t1List, QStringList t2List);
	bool gotoItem(int index);
	bool gotoItem(QListWidgetItem* item);
	bool gotoLastItem();
	bool gotoNextItem();
	void finishedCurrentItem();
	void resetProgress();
	void progressUpAdd();

signals:
	void FileClickRequest(QString t1Path, QString t2Path, QString jsonPath);
	void saveLastFileRequest();
};
