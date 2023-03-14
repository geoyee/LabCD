#include <QVBoxLayout>
#include <QFileInfo>
#include <QString>
#include "filelist.h"

FileList::FileList(QWidget* parent)
	: QWidget(parent)
{
	// 布局
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	fList = new QListWidget(this);
	connect(fList, &QListWidget::itemDoubleClicked, [=](QListWidgetItem* item) {
		emit saveLastFileRequest();  // 发送信号，保存上一张图像
		gotoItem(item);  // 发送信号，当前点击的文件名打开图像
	});
	vLayout->addWidget(fList);
	progressBar = new QProgressBar(this);
	progressBar->setValue(0);
	progressBar->setFormat("0 / 0");
	progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);  // 对齐方式
	vLayout->addWidget(progressBar);
	// 加载
	setLayout(vLayout);
}

FileList::~FileList()
{

}

QString FileList::getGTJsonPath(QString t1Path)
{
	QFileInfo fileInfo(t1Path);
	QString jsonPath = t1Path.replace("A", "GT");
	jsonPath = jsonPath.replace(fileInfo.completeSuffix(), "json");
	QFileInfo jsonInfo(jsonPath);
	if (jsonInfo.isFile())
		return jsonPath;
	else
		return "";
}

void FileList::addFileNames(QStringList t1List, QStringList t2List)
{
	QStringList t1Files = t1List;
	t2Files = t2List;
	// 显示
	fList->addItems(t1Files);
	fList->setCurrentRow(t1Files.size() - 1);  // 移动位置
}

bool FileList::gotoItem(int index)
{
	if (index < 0 || index > fList->count() - 1)
		return false;
	fList->setCurrentRow(index);
	nowIndex = index;
	QString t1Path = fList->currentItem()->text();
	QString t2Path = t2Files.at(fList->currentRow());
	emit FileList::FileClickRequest(t1Path, t2Path, getGTJsonPath(t1Path));
	return true;
}

bool FileList::gotoItem(QListWidgetItem* item)
{
	try
	{
		fList->setCurrentItem(item);
		nowIndex = fList->currentRow();
		QString t1Path = item->text();
		QString t2Path = t2Files.at(fList->currentRow());
		emit FileList::FileClickRequest(t1Path, t2Path, getGTJsonPath(t1Path));
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool FileList::gotoLastItem()
{
	return gotoItem(fList->currentIndex().row() - 1);
}

bool FileList::gotoNextItem()
{
	return gotoItem(fList->currentIndex().row() + 1);
}

void FileList::finishedCurrentItem()
{
	QListWidgetItem* item = fList->item(nowIndex);
	if (item->background().color() != QColor("#7fffd4"))
	{
		item->setBackground(QBrush(QColor("#7fffd4")));
		finishedNumber++;
	}
}

void FileList::resetProgress()
{
	progressBar->setRange(0, fList->count());
	progressBar->reset();
	finishedNumber = 0;
	progressBar->setValue(0);
	progressBar->setFormat("0 / %m");
}

void FileList::progressUpAdd()
{
	progressBar->setValue(finishedNumber);
	progressBar->setFormat(QString::number(finishedNumber) + " / %m");
}
