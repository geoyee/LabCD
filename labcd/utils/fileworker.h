#pragma once

#include <QString>

class FileWorker
{
public:
	static bool openImageDir(
		QStringList* t1List,
		QStringList* t2List,
		QWidget* parent = nullptr
	);
};
