﻿#include <QGridLayout>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include "multcanvas.h"
#include "../utils/imgpress.h"

MultCanvas::MultCanvas(QWidget* parent)
	: QWidget(parent)
{
	QGridLayout* gLayout = new QGridLayout(this);
	t1Canva = new Canvas(this);
	gLayout->addWidget(t1Canva, 0, 0);
	t2Canva = new Canvas(this);
	gLayout->addWidget(t2Canva, 0, 1);
	/* 操作关联 */
	// 同步鼠标移动缩放
	connect(
		t1Canva->aView, &AnnotationView::syncRequest,
		t2Canva->aView, &AnnotationView::syncTranslate
	);
	connect(
		t2Canva->aView, &AnnotationView::syncRequest,
		t1Canva->aView, &AnnotationView::syncTranslate
	);
	// 同步垂直和水平滑动条位置
	connect(t1Canva, &Canvas::syncScroll, t2Canva, &Canvas::scroolTranslate);
	connect(t2Canva, &Canvas::syncScroll, t1Canva, &Canvas::scroolTranslate);
	// 加载标签和图像大小的同步
	connect(this, &MultCanvas::labelSelected, [=](Label* label) {
		t1Canva->aScene->getLabel(label);
		t2Canva->aScene->getLabel(label);
	});
	connect(this, &MultCanvas::imageLoaded, [=](int imgWidth, int imgHeight) {
		t1Canva->aScene->getImageSize(imgWidth, imgHeight);
		t2Canva->aScene->getImageSize(imgWidth, imgHeight);
	});
	// 同步画图
	connect(
		t1Canva->aScene, &AnnotationScence::mouseOptRequest,
		t2Canva->aScene, &AnnotationScence::copyMouseOpt
	);
	connect(
		t2Canva->aScene, &AnnotationScence::mouseOptRequest,
		t1Canva->aScene, &AnnotationScence::copyMouseOpt
	);
	// 同步十字丝
	connect(
		t1Canva->aView, &AnnotationView::mousePosChanged,
		t2Canva->aScene, &AnnotationScence::onMouseChanged
	);
	connect(
		t2Canva->aView, &AnnotationView::mousePosChanged,
		t1Canva->aScene, &AnnotationScence::onMouseChanged
	);
	setLayout(gLayout);
}

MultCanvas::~MultCanvas()
{

}

void MultCanvas::loadImages(QString t1Path, QString t2Path, QString jsonPath)
{
	QPixmap t1;
	QPixmap t2;
	bool t1Succ = ImagePress::openImage(t1Path, t1, projs, trans);
	bool t2Succ = ImagePress::openImage(t2Path, t2, projs, trans);
	if (!t1Succ || !t2Succ)
		QMessageBox::critical(
			this,
			tr("错误"),
			tr("无法打开图像文件。")
		);
	else
	{
		if (t1.width() != t2.width() || t1.height() != t2.height())
			QMessageBox::critical(
				this,
				tr("错误"),
				tr("两个时段的数据大小不一致。")
			);
		else
		{
			t1Canva->loadImageFromPixmap(t1);
			t2Canva->loadImageFromPixmap(t2);
			imageWidth = t1.width();
			imageHeight = t1.height();
			emit imageLoaded(imageWidth, imageHeight);  // 发送大小
			// 加载已标注过的数据
			if (jsonPath != "")
			{
				t1Canva->loadJSONFromFile(jsonPath);
				t2Canva->loadJSONFromFile(jsonPath);
			}
			t1Canva->aScene->resetScence();
			t2Canva->aScene->resetScence();
			// CVA变化图
			cv::Mat imgT1 = ImagePress::qpixmapToCVMat(t1);
			cv::Mat imgT2 = ImagePress::qpixmapToCVMat(t2);
			cv::Mat imgDiff = ImagePress::CVA(imgT1, imgT2);
			emit addimgDiff(imgDiff);
		}
	}
}

void MultCanvas::clearFocusAndSelected()
{
	t1Canva->aScene->clearFocusAndSelected();
	t2Canva->aScene->clearFocusAndSelected();
}

void MultCanvas::finished()
{
	t1Canva->aScene->finished();
	t1Canva->aScene->finished();
}

QColor MultCanvas::getCrossPenColor()
{
	return t1Canva->aScene->getCrossPenColor();
}

void MultCanvas::setCrossPenColor(QColor color)
{
	t1Canva->aScene->setCrossPenColor(color);
	t2Canva->aScene->setCrossPenColor(color);
}
