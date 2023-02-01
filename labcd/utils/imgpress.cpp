#include <iostream>
#include <fstream>
#include <json/json.h>
#include <QFileInfo>
#include <QImageReader>
#include "imgpress.h"

unsigned char* ImagePress::imgSketch(
	float* buffer, 
	GDALRasterBand* currentBand, 
	int bandSize, 
	double noValue
)
{
	unsigned char* resBuffer = new unsigned char[bandSize];
	double max, min;
	double minmax[2];
	currentBand->ComputeRasterMinMax(1, minmax);
	min = minmax[0];
	max = minmax[1];
	if (min <= noValue && noValue <= max)
	{
		min = 0;
	}
	for (int i = 0; i < bandSize; i++)
	{
		if (buffer[i] > max)
		{
			resBuffer[i] = 255;
		}
		else if (buffer[i] <= max && buffer[i] >= min)
		{
			resBuffer[i] = static_cast<uchar>(255 - 255 * (max - buffer[i]) / (max - min));
		}
		else
		{
			resBuffer[i] = 0;
		}
	}
	return resBuffer;
}

QPixmap ImagePress::GDALRastertoPixmap(QList<GDALRasterBand*>* imgBand)
{
	rsize_t imgWidth = imgBand->at(0)->GetXSize();
	rsize_t imgHeight = imgBand->at(0)->GetYSize();
	GDALDataType dataType = imgBand->at(0)->GetRasterDataType();
	// 首先分别读取RGB三个波段
	float* rBand = new float[imgWidth * imgHeight];
	float* gBand = new float[imgWidth * imgHeight];
	float* bBand = new float[imgWidth * imgHeight];
	unsigned char* rBandUC, * gBandUC, * bBandUC;
	imgBand->at(0)->RasterIO(
		GF_Read, 0, 0, imgWidth, imgHeight, rBand, imgWidth, imgHeight, GDT_Float32, 0, 0
	);
	imgBand->at(1)->RasterIO(
		GF_Read, 0, 0, imgWidth, imgHeight, gBand, imgWidth, imgHeight, GDT_Float32, 0, 0
	);
	imgBand->at(2)->RasterIO(
		GF_Read, 0, 0, imgWidth, imgHeight, bBand, imgWidth, imgHeight, GDT_Float32, 0, 0
	);
	// 分别拉伸每个波段并将Float转换为unsigned char
	rBandUC = imgSketch(
		rBand, imgBand->at(0), imgWidth * imgHeight, imgBand->at(0)->GetNoDataValue()
	);
	gBandUC = imgSketch(
		gBand, imgBand->at(1), imgWidth * imgHeight, imgBand->at(1)->GetNoDataValue()
	);
	bBandUC = imgSketch(
		bBand, imgBand->at(2), imgWidth * imgHeight, imgBand->at(2)->GetNoDataValue()
	);
	// 将三个波段组合起来
	int bytePerLine = (imgWidth * 24 + 31) / 8;
	unsigned char* allBandUC = new unsigned char[bytePerLine * imgHeight * 3];
	for (int h = 0; h < imgHeight; h++)
	{
		for (int w = 0; w < imgWidth; w++)
		{
			allBandUC[h * bytePerLine + w * 3 + 0] = rBandUC[h * imgWidth + w];
			allBandUC[h * bytePerLine + w * 3 + 1] = gBandUC[h * imgWidth + w];
			allBandUC[h * bytePerLine + w * 3 + 2] = bBandUC[h * imgWidth + w];
		}
	}
	return QPixmap::fromImage(QImage(allBandUC, imgWidth, imgHeight, bytePerLine, QImage::Format_RGB888));
}

bool ImagePress::saveTiffFromCVMat(
	std::string savePath, 
	cv::Mat mask, 
	const char* projs, 
	double* trans
)
{
	int nImgSizeX = mask.cols;
	int nImgSizeY = mask.rows;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	GDALDriver* pDriverMEM = GetGDALDriverManager()->GetDriverByName("GTiff");
	if (!pDriverMEM) 
	{
		return false;
	}
	GDALDataset* poDataset = pDriverMEM->Create(
		savePath.c_str(), nImgSizeX, nImgSizeY, 1, GDT_Byte, NULL
	);
	poDataset->SetProjection(projs);
	poDataset->SetGeoTransform(trans);
	if (!poDataset) 
	{
		return false;
	}
	poDataset->GetRasterBand(1)->RasterIO(
		GF_Write, 0, 0, nImgSizeX, nImgSizeY, mask.data, nImgSizeX, nImgSizeY, GDT_Byte, 0, 0
	);
	GDALClose(poDataset);
	GDALDestroyDriverManager();
	return true;
}

cv::Mat ImagePress::CVA(cv::Mat t1, cv::Mat t2)
{
	cv::cvtColor(t1, t1, cv::COLOR_BGR2GRAY);
	cv::cvtColor(t2, t1, cv::COLOR_BGR2GRAY);
	cv::Mat diff = t1 - t1;
	cv::Mat deltaV;
	cv::pow(diff, 2, deltaV);
	cv::Scalar sumdeltaV = cv::sum(deltaV);
	cv::pow(deltaV, 0.5, deltaV);
	diff = diff / (cv::abs(deltaV) + 1e-12);
	double max, min;
	cv::minMaxIdx(diff, &min, &max);
	cv::Mat cva = (diff - min) / (max - min + 1e-12);
	return cva;
}

void ImagePress::saveResultFromPolygon(
		QString savePath,
		int labNum,
		int imgHeight,
		int imgWidth,
		QList<LabPolygon*> polygons,
		std::string projs,
		double* trans
)
{
	// 处理保存路径
	QStringList pathAndName = savePath.split(".");
	std::string iPath = pathAndName[0].toStdString();
	std::string iExt = pathAndName[1].toStdString();
	cv::String pseudoSavaPath = iPath + "_pseudo.png";  // 保存伪彩色
	std::string jsonSavePath = iPath + ".json";// 保存json
	// 新建保存的图像
	cv::Mat pseudoResult = cv::Mat::zeros(cv::Size(imgWidth, imgHeight), CV_8UC3);
	cv::Mat result = cv::Mat::zeros(cv::Size(imgWidth, imgHeight), CV_8UC1);
	// 新建保存的json
	Json::StyledWriter writer;
	Json::Value polyList;
	std::ofstream os;
	os.open(jsonSavePath);
	// 标号小的覆盖标号大的
	for (int labIndex = labNum - 1; labIndex >= 0; labIndex--)
	{
		for (LabPolygon* poly : polygons)
		{
			if (poly->labelIndex == labIndex)
			{
				Json::Value polygons;
				polygons["labelIndex"] = poly->labelIndex;
				QColor c = poly->getColor();
				cv::Scalar color = cv::Scalar(c.blue(), c.green(), c.red());
				polygons["color"]["R"] = c.red();
				polygons["color"]["G"] = c.green();
				polygons["color"]["B"] = c.blue();
				const int numPoint = poly->mPoints.count();
				polygons["polygon"]["pointNumber"] = numPoint;
				cv::Point** cvPoints = new cv::Point * [1];
				cvPoints[0] = new cv::Point[numPoint];
				Json::Value points;
				for (int i = 0; i < numPoint; i++)
				{
					cvPoints[0][i] = cv::Point(
						poly->mPoints.at(i)->x(), poly->mPoints.at(i)->y()
					);
					points.append(poly->mPoints.at(i)->x());
					points.append(poly->mPoints.at(i)->y());
				}
				const cv::Point* ppt[1] = { cvPoints[0] };
				int npt[] = { numPoint };
				cv::fillPoly(pseudoResult, ppt, npt, 1, color);
				cv::fillPoly(result, ppt, npt, 1, cv::Scalar(labIndex));
				polygons["polygon"]["points"] = points;
				polyList.append(polygons);
				// 清除数组
				delete[] cvPoints[0];
				delete[] cvPoints;
			}
		}
	}
	// 保存
	cv::imwrite(pseudoSavaPath, pseudoResult);
	if (iExt == "jpg" || iExt == "jpeg" || iExt == "png")
	{
		cv::String baseSavaPath = iPath + ".bmp";
		cv::imwrite(baseSavaPath, result);
	}
	else
	{
		std::string baseSavaPath = iPath + ".tif";
		saveTiffFromCVMat(baseSavaPath, result, projs.c_str(), trans);
	}
	os << writer.write(polyList);
	os.close();  // 关闭json
}

bool ImagePress::openImage(
	QString imgPath,
	QPixmap &img,
	std::string &projs,
	double trans[6]
)
{
	QFileInfo fileInfo(imgPath);
	QString ext = fileInfo.completeSuffix();
	QList<QByteArray> qtSupporExts = QImageReader::supportedImageFormats();
	if (qtSupporExts.contains(ext.toLocal8Bit()))
	{
		img.load(imgPath);
		return true;
	}
	else  // 使用gdal打开
	{
		GDALAllRegister();  // 注册所有的驱动
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");  // 设置支持中文路径和文件名
		GDALDataset* poDataset = (GDALDataset*)GDALOpen(imgPath.toStdString().c_str(), GA_ReadOnly);
		if (poDataset == NULL)
		{
			return false;
		}
		int bandCount = poDataset->GetRasterCount();
		QList<GDALRasterBand*> bandList;
		if (bandCount == 3)
		{
			bandList.append(poDataset->GetRasterBand(1));
			bandList.append(poDataset->GetRasterBand(2));
			bandList.append(poDataset->GetRasterBand(3));
		}
		else
		{
			bandList.append(poDataset->GetRasterBand(1));
			bandList.append(poDataset->GetRasterBand(1));
			bandList.append(poDataset->GetRasterBand(1));
		}
		projs = poDataset->GetProjectionRef();
		poDataset->GetGeoTransform(trans);
		img = QPixmap(GDALRastertoPixmap(&bandList));
		// 注销驱动
		GDALClose(poDataset);
		GDALDestroyDriverManager();
		return true;
	}
}
