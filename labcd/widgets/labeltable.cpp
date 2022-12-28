#include <QVBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QColorDialog>
#include <QLabel>
#include <fstream>
#include <json/json.h>
#include "labeltable.h"

LabelTable::LabelTable(QWidget *parent)
	: QWidget(parent)
{
	nowLabel = new Label();
	// 颜色表
	cMap = new ColorMap();
	// 布局
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	// 标签列表
	labelTable = new QTableWidget(this);
	labelTable->setRowCount(0);
	labelTable->setColumnCount(4);
	labelTable->setColumnWidth(2, 50);
	auto labTabHHeader = labelTable->horizontalHeader();
	labTabHHeader->hide();
	labTabHHeader->setSectionResizeMode(QHeaderView::Stretch);  //铺满
	// 仅文字框宽度自适应
	labTabHHeader->setDefaultSectionSize(30);
	labTabHHeader->setSectionResizeMode(0, QHeaderView::Fixed);
	labTabHHeader->setSectionResizeMode(2, QHeaderView::Fixed);
	labTabHHeader->setSectionResizeMode(3, QHeaderView::Fixed);
	auto labTabVHeader = labelTable->verticalHeader();
	labTabVHeader->hide();
	// 选择、切换颜色和删除
	connect(labelTable, &QTableWidget::cellClicked, this, &LabelTable::clickItem);
	// 标签改变文本
	connect(labelTable, &QTableWidget::cellChanged, [=](int row, int column) {
		if (column == 1)
		{
			nowLabel->setName(labelTable->item(row, 1)->text());
		}
	});
	// 初始标签
	addLabelItem(true);
	vLayout->addWidget(labelTable);
	// 添加标签按钮
	QPushButton* btnAddLabel = new QPushButton(tr("添加标签"), this);
	btnAddLabel->setIcon(QIcon(":/docks/resources/AddLabel.png"));
	connect(btnAddLabel, &QPushButton::clicked, this, &LabelTable::addLabelItem);
	vLayout->addWidget(btnAddLabel);
	// 加载
	setLayout(vLayout);
}

LabelTable::~LabelTable()
{

}

void LabelTable::clickItem(int row, int column)
{
	int nowIndex = labelTable->item(row, 0)->text().toInt();
	QString nowName = labelTable->item(row, 1)->text();
	QColor nowColor = labelTable->item(row, 2)->background().color();
	nowLabel->reSet(nowIndex, nowName, nowColor);
	if (column == 2 && row != 0)  // 换颜色
	{
		QColor newColor = QColorDialog::getColor(
			nowColor, this, tr("标签颜色选择"));
		if (newColor.isValid())
		{
			labelTable->item(row, 2)->setBackground(newColor);
			nowLabel->setColor(newColor);
			emit colorChanged(row, newColor);
		}
	}
	else if (column == 3 && row != 0)  // 删除
	{
		if (nowLabel->getIndex() == row)
		{
			nowLabel->init();
		}
		labelTable->removeRow(row);
		for (int r = 0; r < labelTable->rowCount(); r++)
		{
			labelTable->item(r, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			labelTable->item(r, 0)->setText(QString::number(r));
			labelTable->item(r, 0)->setFlags(
				labelTable->item(r, 0)->flags() & ~Qt::ItemIsEditable
			);
		}
	}
	else  // 点击特效
	{
		for (int col = 0; col < 2; col++)
		{
			for (int r = 0; r < labelTable->rowCount(); r++)
			{
				labelTable->item(r, col)->setBackground(
					QColor(255, 255, 255));
			}
			labelTable->item(row, col)->setBackground(
				QColor(48, 140, 198));
			labelTable->item(row, 0)->setSelected(true);
		}
		emit labelSelected(nowLabel);
	}
}

void LabelTable::createLabelItem(int _index, QString _name, QColor _color)
{
	labelTable->insertRow(labelTable->rowCount());  // 插入行
	QTableWidgetItem* indexItem = new QTableWidgetItem();
	indexItem->setText(QString::number(_index));
	indexItem->setTextAlignment(Qt::AlignCenter);
	indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEditable);
	labelTable->setItem(_index, 0, indexItem);  // 序号
	QTableWidgetItem* nameItem = new QTableWidgetItem();
	nameItem->setText(_name);
	nameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	labelTable->setItem(_index, 1, nameItem);  // 名称
	QTableWidgetItem* colorItem = new QTableWidgetItem();
	colorItem->setBackground(_color);
	colorItem->setFlags(Qt::ItemIsEnabled);
	labelTable->setItem(_index, 2, colorItem);  // 颜色
	QTableWidgetItem* delItem = new QTableWidgetItem();
	delItem->setIcon(QIcon(":/docks/resources/Delete.png"));
	delItem->setFlags(Qt::ItemIsEnabled);
	labelTable->setItem(_index, 3, delItem);  // 删除按钮
}

int LabelTable::getLen()
{
	return labelTable->rowCount();
}

void LabelTable::addLabelItem(bool init)
{	
	if (init)
	{
		createLabelItem(0, tr("背景"), QColor(0, 0, 0));
		labelTable->item(0, 1)->setFlags(labelTable->item(0, 1)->flags() & ~Qt::ItemIsEditable);
		labelTable->item(0, 3)->setIcon(QIcon(":/docks/resources/CantDelete.png"));
	}
	else
	{
		int idx = labelTable->rowCount();
		createLabelItem(idx, "", cMap->getColor());
	}
}

void LabelTable::exportLabelToFile(QString path)
{
	Json::StyledWriter writer;
	Json::Value root;
	std::ofstream os;
	os.open(path.toStdString());
	// 写入json内容
	for (int i = 1; i < labelTable->rowCount(); i++)
	{
		Json::Value leaf;
		leaf["index"] = labelTable->item(i, 0)->text().toInt();
		leaf["classname"] = labelTable->item(i, 1)->text().toStdString();
		QColor c = labelTable->item(i, 2)->background().color();
		leaf["color"]["R"] = c.red();
		leaf["color"]["G"] = c.green();
		leaf["color"]["B"] = c.blue();
		root.append(leaf);
	}
	os << writer.write(root);
	os.close();
}

bool LabelTable::importLabelFromFile(QString path)
{
	std::ifstream ifs(path.toStdString(), std::ios::binary);
	if (!ifs.is_open())
	{
		return false;
	}
	Json::Reader reader;
	Json::Value root;
	// 解析json内容
	if (reader.parse(ifs, root))
	{
		for (int i = 0; i < root.size(); i++)
		{
			int jIndex = root[i]["index"].asInt();
			QString jClassName = QString(root[i]["classname"].asCString());
			QColor jColor = QColor(
				root[i]["color"]["R"].asInt(),
				root[i]["color"]["G"].asInt(),
				root[i]["color"]["B"].asInt()
			);
			createLabelItem(jIndex, jClassName, jColor);  // 恢复标签
		}
	}
	ifs.close();
	cMap->setIndex(root.size());  // 移动色表
	return true;
}
