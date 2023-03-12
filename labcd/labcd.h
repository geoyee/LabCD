#pragma once

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QSettings>
#include "widgets/labeltable.h"
#include "widgets/filelist.h"
#include "widgets/multcanvas.h"

class LabCD : public QMainWindow
{
	Q_OBJECT

private:
	QSettings* setting;
	QString savePath = "";
	QString fileName;
	bool isCN;
	const int refNewWidth = 200;
	int refNewHeight = 200;

	void openDir();
	void openBigImageFile();
	void mergeBigImage();
	void clearEmptyMask();
	inline void _clearEmptyMask(
		QStringList t1List, QStringList t2List, QStringList GTList);
	void convertMask2Json();
	inline void _convertMask2Json(QString dirPath);
	void save();
	void setCrossPenColor();

public:
	FileList* fListWidget = nullptr;  // 数据列表
	LabelTable* labTableWidget = nullptr;  // 标签列表
	MultCanvas* drawCanvas = nullptr;  // 绘图界面
	QLabel* messageState = nullptr;  // 消息框

	LabCD(QWidget* parent = nullptr);
	~LabCD();
	void closeEvent(QCloseEvent* ev);
};
