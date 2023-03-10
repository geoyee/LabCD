#pragma once

#include <QTranslator>

class FileWorker
{
public:
	static bool openImageDir(
		QStringList* t1List,
		QStringList* t2List,
		QStringList* GTList = nullptr,
		QWidget* parent = nullptr
	);

	static bool createFolder(QString path);
};
