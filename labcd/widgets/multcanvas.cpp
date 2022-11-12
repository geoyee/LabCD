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
	connect(t1Canva, &Canvas::syncViewRequest, t2Canva, &Canvas::syncViewTranslate);
	connect(t2Canva, &Canvas::syncViewRequest, t1Canva, &Canvas::syncViewTranslate);
	// 同步垂直和水平滑动条位置
	connect(t1Canva, &Canvas::syncScroll, t2Canva, &Canvas::scroolTranslate);
	connect(t2Canva, &Canvas::syncScroll, t1Canva, &Canvas::scroolTranslate);
	// 加载标签和图像大小的同步
	connect(this, &MultCanvas::getLabel, [=](Label* label) {
		t1Canva->setLabel(label);
		t2Canva->setLabel(label);
	});
	connect(this, &MultCanvas::getImageSize, [=](int imgWidth, int imgHeight) {
		t1Canva->setImageSize(imgWidth, imgHeight);
		t2Canva->setImageSize(imgWidth, imgHeight);
	});
	// 同步画图
	connect(
		t1Canva->aScene, &AnnotationScence::iPressed,
		t2Canva->aScene, &AnnotationScence::PressedAddPoint);
	connect(
		t1Canva->aScene, &AnnotationScence::iDoubleClicked,
		t2Canva->aScene, &AnnotationScence::doubleClickedFinshPolygon);
	connect(
		t2Canva->aScene, &AnnotationScence::iPressed,
		t1Canva->aScene, &AnnotationScence::PressedAddPoint);
	connect(
		t2Canva->aScene, &AnnotationScence::iDoubleClicked,
		t1Canva->aScene, &AnnotationScence::doubleClickedFinshPolygon);
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
		emit getImageSize(t1.width(), t1.height());  // 发送大小
		// cv::Mat imgDiff = ImagePress::CVA(imgT1, imgT2);  // 鹰眼图
	}
}
