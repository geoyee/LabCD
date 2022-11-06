#include <QGridLayout>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include "multcanvas.h"
#include "../utils/imgpress.h"

MultCanvas::MultCanvas(QWidget *parent)
	: QWidget(parent)
{
	QGridLayout* gLayout = new QGridLayout(this);
	t1Canva = new Canvas(this);
	gLayout->addWidget(t1Canva, 0, 0);
	t2Canva = new Canvas(this);
	gLayout->addWidget(t2Canva, 0, 1);
	/* 操作关联 */
	// 同步鼠标移动缩放
	connect(t1Canva, &Canvas::syncCanvas, t2Canva, &Canvas::viewTranslate);
	connect(t2Canva, &Canvas::syncCanvas, t1Canva, &Canvas::viewTranslate);
	// 同步垂直和水平滑动条位置
	connect(t1Canva, &Canvas::syncScroll, t2Canva, &Canvas::scroolTranslate);
	connect(t2Canva, &Canvas::syncScroll, t1Canva, &Canvas::scroolTranslate);
	setLayout(gLayout);
}

MultCanvas::~MultCanvas()
{

}

void MultCanvas::loadImages(QString t1Path, QString t2Path)
{
	QPixmap t1;
	t1.load(t1Path);
	QPixmap t2;
	t2.load(t2Path);
	if (t1.width() != t2.width() || t1.height() != t2.height())
	{
		QMessageBox::critical(
			this,
			"错误",
			"两个时段的数据大小不一致。"
		);
	}
	else
	{
		t1Canva->loadImageFromPixmap(t1);
		t2Canva->loadImageFromPixmap(t2);
		// cv::Mat imgDiff = ImagePress::CVA(imgT1, imgT2);  // 鹰眼图
	}
}
