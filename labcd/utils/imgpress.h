#pragma once

#include <opencv2/opencv.hpp>
#include <gdal/gdal_priv.h>
#include <QString>
#include "../widgets/labpolygon.h"

class ImagePress
{
private:
	static unsigned char* ImgSketch(
		float* buffer, 
		GDALRasterBand* currentBand, 
		int bandSize, 
		double noValue
	);
	static QPixmap GDALRastertoPixmap(QList<GDALRasterBand*>* imgBand);

public:
	static cv::Mat CVA(cv::Mat t1, cv::Mat t2);

	static void saveResultFromPolygon(
		QString savePath,
		int labNum,
		int imgHeight,
		int imgWidth,
		QList<LabPolygon*> polygons
	);

	static bool openImage(QString imgPath, QPixmap &img);
};