#include <QVBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QColorDialog>
#include <QLabel>
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
	// 初始标签
	addLabelItem(true);
	// 切换颜色和删除事件
	connect(labelTable, &QTableWidget::cellClicked, [=](int row, int column) {
		int nowIndex = labelTable->item(row, 0)->text().toInt();
		QString nowName = labelTable->item(row, 1)->text();
		QColor nowColor = labelTable->item(row, 2)->background().color();
		nowLabel->reSet(nowIndex, nowName, nowColor);
		if (column == 2 && row != 0)  // 换颜色
		{
			QColor newColor = QColorDialog::getColor(
				nowColor, this, "标签颜色选择");
			if (newColor.isValid())
			{
				labelTable->item(row, 2)->setBackground(newColor);
				nowLabel->setColor(newColor);
				emit colorChanged(newColor);
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
				labelTable->item(r, 0)->setText(QString::number(r + 1));
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
	});
	// 标签改变事件
	connect(labelTable, &QTableWidget::cellChanged, [=](int row, int column) {
		if (column == 1)
		{
			nowLabel->setName(labelTable->item(row, 1)->text());
		}
	});
	vLayout->addWidget(labelTable);
	// 添加标签按钮
	QPushButton* btnAddLabel = new QPushButton("添加标签", this);
	btnAddLabel->setIcon(QIcon(":/docks/resources/AddLabel.png"));
	connect(btnAddLabel, &QPushButton::clicked, this, &LabelTable::addLabelItem);
	vLayout->addWidget(btnAddLabel);
	// 加载
	setLayout(vLayout);
}

LabelTable::~LabelTable()
{

}

void LabelTable::addLabelItem(bool init)
{	
	int idx = labelTable->rowCount();
	
	// 添加行
	labelTable->insertRow(labelTable->rowCount());  // 插入行
	QTableWidgetItem* indexItem = new QTableWidgetItem();
	indexItem->setText(QString::number(idx));
	indexItem->setTextAlignment(Qt::AlignCenter);
	labelTable->setItem(idx, 0, indexItem);  // 序号
	QTableWidgetItem* nameItem = new QTableWidgetItem();
	labelTable->setItem(idx, 1, nameItem);  // 名称
	QTableWidgetItem* colorItem = new QTableWidgetItem();
	colorItem->setFlags(Qt::ItemIsEnabled);
	labelTable->setItem(idx, 2, colorItem);  // 颜色
	QTableWidgetItem* delItem = new QTableWidgetItem();
	delItem->setFlags(Qt::ItemIsEnabled);
	labelTable->setItem(idx, 3, delItem);  // 删除按钮
	if (!init)
	{
		nameItem->setText("");
		nameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		colorItem->setBackground(cMap->getColor());
		delItem->setIcon(QIcon(":/docks/resources/Delete.png"));
	}
	else
	{
		nameItem->setText("背景");
		nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
		colorItem->setBackground(QColor(0, 0, 0));
		delItem->setIcon(QIcon(":/docks/resources/CantDelete.png"));
	}
}
