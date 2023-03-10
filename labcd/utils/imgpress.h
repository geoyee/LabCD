#pragma once

#include <iostream>
#include <variant>
#include <opencv2/opencv.hpp>
#include <gdal/gdal_priv.h>
#include <QString>
#include "../widgets/labpolygon.h"

class ImagePress
{
private:
	static bool createArr(
		void** data, GDALDataType types, int xSize, int ySize, int band);
	static unsigned char* imgSketch(
		float* buffer,
		GDALRasterBand* currentBand,
		int bandSize,
		double noValue
	);
	static QPixmap GDALRastertoPixmap(QList<GDALRasterBand*>* imgBand);
	static bool saveTiffFromGDAL(
		std::string savePath,
		void* img,
		int nImgSizeX,
		int nImgSizeY,
		int nChannel,
		GDALDataType types,
		const char* projs,
		double* trans,
		int* pBandMaps
	);
	static bool saveTiffFromCVMask(
		std::string savePath,
		cv::Mat mask,
		const char* projs,
		double* trans
	);
	static void calcWindowTrans(double trans[6], int locX, int locY);

public:
	static cv::Mat CVA(cv::Mat t1, cv::Mat t2);
	static void saveResultFromPolygon(
		QString savePath,
		int labNum,
		int imgHeight,
		int imgWidth,
		QList<LabPolygon*> polygons,
		std::string projs = "",
		double* trans = NULL
	);
	static bool openImage(
		QString imgPath,
		QPixmap& img,
		std::string& projs,
		double trans[6]
	);
	static bool splitTiff(
		QString imgPath,
		QString saveDir,
		int blockHeight = 512,
		int blockWidth = 512
	);
	static bool mergeTiff(QString imgDir);
	static cv::Mat qpixmapToCVMat(QPixmap pimg);
	static std::vector<int> calcOIF(QString hsiPath);
	static bool maskIsEmpty(QString maskPath);
};
