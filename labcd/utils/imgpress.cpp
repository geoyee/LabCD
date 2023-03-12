#include <fstream>
#include <json/json.h>
#include <Eigen/Dense>
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QtAlgorithms>
#include "colormap.h"
#include "imgpress.h"

bool ImagePress::createArr(
	void** data, GDALDataType types, int xSize, int ySize, int band)
{
	switch (types)
	{
	case GDT_Byte:
		*data = new unsigned char[xSize * ySize * band];
		break;
	case GDT_UInt16:
		*data = new unsigned short[xSize * ySize * band];
		break;
	case GDT_Int16:
		*data = new short[xSize * ySize * band];
		break;
	case GDT_UInt32:
		*data = new unsigned long[xSize * ySize * band];
		break;
	case GDT_Int32:
		*data = new long[xSize * ySize * band];
		break;
	case GDT_Float32:
		*data = new float[xSize * ySize * band];
		break;
	case GDT_Float64:
		*data = new double[xSize * ySize * band];
		break;
	default:
		return false;
	}
	return true;
}

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
		min = 0;
	for (int i = 0; i < bandSize; ++i)
	{
		if (buffer[i] > max)
			resBuffer[i] = 255;
		else if (buffer[i] <= max && buffer[i] >= min)
			resBuffer[i] = static_cast<uchar>(
				255 - 255 * (max - buffer[i]) / (max - min));
		else
			resBuffer[i] = 0;
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
		GF_Read, 0, 0, imgWidth, imgHeight, rBand,
		imgWidth, imgHeight, GDT_Float32, 0, 0
	);
	imgBand->at(1)->RasterIO(
		GF_Read, 0, 0, imgWidth, imgHeight, gBand,
		imgWidth, imgHeight, GDT_Float32, 0, 0
	);
	imgBand->at(2)->RasterIO(
		GF_Read, 0, 0, imgWidth, imgHeight, bBand,
		imgWidth, imgHeight, GDT_Float32, 0, 0
	);
	// 分别拉伸每个波段并将Float转换为unsigned char
	rBandUC = imgSketch(
		rBand, imgBand->at(0), imgWidth * imgHeight,
		imgBand->at(0)->GetNoDataValue()
	);
	gBandUC = imgSketch(
		gBand, imgBand->at(1), imgWidth * imgHeight,
		imgBand->at(1)->GetNoDataValue()
	);
	bBandUC = imgSketch(
		bBand, imgBand->at(2), imgWidth * imgHeight,
		imgBand->at(2)->GetNoDataValue()
	);
	// 将三个波段组合起来
	int bytePerLine = (imgWidth * 24 + 31) / 8;
	unsigned char* allBandUC = new unsigned char[bytePerLine * imgHeight * 3];
	for (int h = 0; h < imgHeight; ++h)
	{
		for (int w = 0; w < imgWidth; ++w)
		{
			allBandUC[h * bytePerLine + w * 3 + 0] = rBandUC[h * imgWidth + w];
			allBandUC[h * bytePerLine + w * 3 + 1] = gBandUC[h * imgWidth + w];
			allBandUC[h * bytePerLine + w * 3 + 2] = bBandUC[h * imgWidth + w];
		}
	}
	return QPixmap::fromImage(
		QImage(allBandUC, imgWidth, imgHeight, bytePerLine, QImage::Format_RGB888));
}

bool ImagePress::saveTiffFromGDAL(
	std::string savePath,
	void* img,
	int nImgSizeX,
	int nImgSizeY,
	int nChannel,
	GDALDataType types,
	const char* projs,
	double* trans,
	int* pBandMaps
)
{
	GDALDriver* pDriverMEM = GetGDALDriverManager()->GetDriverByName("GTiff");
	if (!pDriverMEM)
	{
		GDALDestroyDriverManager();
		return false;
	}
	GDALDataset* poDataset = pDriverMEM->Create(
		savePath.c_str(), nImgSizeX, nImgSizeY, nChannel, types, NULL);
	poDataset->SetProjection(projs);
	poDataset->SetGeoTransform(trans);
	if (!poDataset)
	{
		GDALClose(poDataset);
		GDALDestroyDriverManager();
		return false;
	}
	poDataset->RasterIO(
		GF_Write, 0, 0, nImgSizeX, nImgSizeY, img, nImgSizeX, nImgSizeY,
		types, nChannel, pBandMaps, 0, 0, 0, NULL
	);
	GDALClose(poDataset);
	return true;
}

bool ImagePress::saveTiffFromCVMask(
	std::string savePath,
	cv::Mat mask,
	const char* projs,
	double* trans
)
{
	int nImgSizeX = mask.cols;
	int nImgSizeY = mask.rows;
	GDALDriver* pDriverMEM = GetGDALDriverManager()->GetDriverByName("GTiff");
	if (!pDriverMEM)
	{
		GDALDestroyDriverManager();
		return false;
	}
	GDALDataset* poDataset = pDriverMEM->Create(
		savePath.c_str(), nImgSizeX, nImgSizeY, 1, GDT_Byte, NULL);
	poDataset->SetProjection(projs);
	poDataset->SetGeoTransform(trans);
	if (!poDataset)
	{
		GDALClose(poDataset);
		GDALDestroyDriverManager();
		return false;
	}
	poDataset->GetRasterBand(1)->RasterIO(
		GF_Write, 0, 0, nImgSizeX, nImgSizeY, mask.data,
		nImgSizeX, nImgSizeY, GDT_Byte, 0, 0, NULL
	);
	GDALClose(poDataset);
	return true;
}

void ImagePress::calcWindowTrans(double trans[6], int locX, int locY)
{
	trans[0] += locX * trans[1];
	trans[3] += locY * trans[5];
}

cv::Mat ImagePress::CVA(cv::Mat t1, cv::Mat t2)
{
	float eps = 1e-12;
	std::vector<cv::Mat> t1Channels;
	cv::split(t1, t1Channels);
	std::vector<cv::Mat> t2Channels;
	cv::split(t2, t2Channels);
	int m = t1.rows;
	int n = t1.cols;
	cv::Mat intensity = cv::Mat::zeros(m, n, CV_64FC1);
	for (int i = 0; i < t1Channels.size(); ++i)
	{
		cv::Mat diff = t1Channels[i] - t2Channels[i];
		cv::pow(diff, 2, diff);
		intensity += diff;
	}
	cv::pow(intensity, 0.5, intensity);
	// 变化强度
	double amin = 0, amax = 0;
	cv::Point minPt, maxPt;
	minMaxLoc(intensity, &amin, &amax, &minPt, &maxPt);
	intensity = 255.0f * (intensity - amin) / (amax - amin);
	// 伪彩色渲染
	intensity.convertTo(intensity, CV_8UC1);
	cv::applyColorMap(intensity, intensity, cv::COLORMAP_HOT);
	return intensity;
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
	std::string jsonSavePath = iPath + ".json";  // 保存json
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
				for (int i = 0; i < numPoint; ++i)
				{
					cvPoints[0][i] = cv::Point(
						poly->mPoints.at(i)->x(), poly->mPoints.at(i)->y());
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
		GDALAllRegister();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
		saveTiffFromCVMask(baseSavaPath, result, projs.c_str(), trans);
		GDALDestroyDriverManager();
	}
	os << writer.write(polyList);
	os.close();  // 关闭json
}

std::vector<uint8_t> ImagePress::calcUnique(cv::Mat mask)
{
	auto begin = mask.begin<uint8_t>(), end = mask.end<uint8_t>();
	auto last = std::unique(begin, end);
	std::sort(begin, last);
	last = std::unique(begin, last);
	return std::vector<uint8_t>(begin, last);
}

void ImagePress::savePolygonFromMask(QString maskPath)
{
	// 新建保存的json
	QStringList pathAndName = maskPath.split(".");
	std::string iPath = pathAndName[0].toStdString();
	std::string jsonSavePath = iPath + ".json";
	Json::StyledWriter writer;
	Json::Value polyList;
	std::ofstream os;
	os.open(jsonSavePath);
	// 读取图像并保存json
	cv::Mat mat = cv::imread(maskPath.toStdString());
	cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
	cv::Mat uMat;
	mat.copyTo(uMat);
	std::vector<uint8_t> unique = ImagePress::calcUnique(uMat);
	// 多类处理
	ColorMap cMap;
	QColor rgb;
	// TODO: 背景类有父轮廓的最后来绘制
	for (int i = 1; i < unique.size(); ++i)
	{
		rgb = cMap.getColor();
		int clas = static_cast<int>(unique.at(i));
		// 创建一个全为0的输出矩阵，与输入矩阵具有相同的大小和类型
		cv::Mat clasMat = cv::Mat::zeros(mat.size(), CV_8UC1);
		// 比较输入矩阵与所需值，将比较结果存储在输出矩阵中
		cv::compare(mat, clas, clasMat, cv::CMP_EQ);
		std::vector< std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(
			clasMat,
			contours,
			hierarchy,
			cv::RETR_TREE,
			cv::CHAIN_APPROX_TC89_KCOS
		);
		std::vector<cv::Point> contourPolys;
		double epsilon;
		for (size_t i = 0; i < hierarchy.size(); i++)
		{
			epsilon = 0.005 * cv::arcLength(cv::Mat(contours[i]), true);
			cv::approxPolyDP(cv::Mat(contours[i]), contourPolys, epsilon, true);
			if (hierarchy[i][3] < 0)  // 没有父轮廓
			{
				Json::Value polygons;
				polygons["labelIndex"] = clas;
				polygons["color"]["R"] = rgb.red();
				polygons["color"]["G"] = rgb.green();
				polygons["color"]["B"] = rgb.blue();
				polygons["polygon"]["pointNumber"] = contourPolys.size();
				Json::Value points;
				for (cv::Point p : contourPolys)
				{
					points.append(p.x);
					points.append(p.y);
				}
				polygons["polygon"]["points"] = points;
				polyList.append(polygons);
			}
		}
	}
	os << writer.write(polyList);
	os.close();  // 关闭json
}

bool ImagePress::openImage(
	QString imgPath,
	QPixmap& img,
	std::string& projs,
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
		GDALDataset* poDataset = (GDALDataset*)GDALOpen(
			imgPath.toStdString().c_str(), GA_ReadOnly);
		if (poDataset == NULL)
		{
			GDALClose(poDataset);
			GDALDestroyDriverManager();
			return false;
		}
		int bandCount = poDataset->GetRasterCount();
		QList<GDALRasterBand*> bandList;
		static std::vector<int> HSI_RGB_LOAD;
		static int NOW_BAND_COUNT;
		if (bandCount < 3)  // 小于3个波段只加载第一个波段
		{
			bandList.append(poDataset->GetRasterBand(1));
			bandList.append(poDataset->GetRasterBand(1));
			bandList.append(poDataset->GetRasterBand(1));
		}
		else if (bandCount == 3)  // RGB直接加载
		{
			bandList.append(poDataset->GetRasterBand(1));
			bandList.append(poDataset->GetRasterBand(2));
			bandList.append(poDataset->GetRasterBand(3));
		}
		else  // 多光谱计算oif再加载
		{
			// 只计算一次
			if (HSI_RGB_LOAD.empty() || bandCount != NOW_BAND_COUNT)
				HSI_RGB_LOAD = ImagePress::calcOIF(imgPath);
			bandList.append(poDataset->GetRasterBand(HSI_RGB_LOAD[0]));
			bandList.append(poDataset->GetRasterBand(HSI_RGB_LOAD[1]));
			bandList.append(poDataset->GetRasterBand(HSI_RGB_LOAD[2]));
		}
		NOW_BAND_COUNT = bandCount;
		projs = poDataset->GetProjectionRef();
		poDataset->GetGeoTransform(trans);
		img = QPixmap(GDALRastertoPixmap(&bandList));
		// 注销驱动
		GDALClose(poDataset);
		GDALDestroyDriverManager();
		return true;
	}
}

bool ImagePress::splitTiff(
		QString imgPath,
		QString saveDir,
		int blockHeight,
		int blockWidth
)
{
	// 获取文件名
	QString name = QFileInfo(imgPath).fileName().split(".")[0];
	// 读取数据解析属性
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	GDALDataset* poDataset = (GDALDataset*)GDALOpen(
		imgPath.toStdString().c_str(), GA_ReadOnly);
	if (poDataset == NULL)
	{
		GDALClose(poDataset);
		GDALDestroyDriverManager();
		return false;
	}
	int bandCount = poDataset->GetRasterCount();
	rsize_t nYSize = poDataset->GetRasterYSize();
	rsize_t nXSize = poDataset->GetRasterXSize();
	GDALDataType types = poDataset->GetRasterBand(1)->GetRasterDataType();
	double trans[6] = { 0 };
	double windowTrans[6] = { 0 };
	poDataset->GetGeoTransform(trans);
	const char* projs = poDataset->GetProjectionRef();
	// 定义读取输入图像波段顺序
	int* pBandMaps = new int[bandCount];
	for (int b = 0; b < bandCount; ++b)
		pBandMaps[b] = b + 1;
	// 循环分块并进行处理
	if (blockHeight > nXSize || blockWidth > nYSize)
		return false;
	void* pSrcData = nullptr;
	ImagePress::createArr(&pSrcData, types, blockWidth, blockHeight, bandCount);
	int row = 0;
	int tmpI, tmpJ;
	for (rsize_t i = 0; i < nYSize; i += blockWidth)
	{
		int col = 0;
		for (rsize_t j = 0; j < nXSize; j += blockHeight)
		{
			tmpI = i;
			tmpJ = j;
			if (i + blockHeight > nYSize)
				tmpI = nYSize - blockHeight;
			if (j + blockWidth > nXSize)
				tmpJ = nXSize - blockWidth;
			// 读取原始图像块
			poDataset->RasterIO(
				GF_Read, tmpJ, tmpI, blockWidth, blockHeight, pSrcData,
				blockWidth, blockHeight, types, bandCount, pBandMaps, 0, 0, 0, NULL
			);
			// 保存
			std::string windowSavePath = (saveDir + QDir::separator() + name + \
				"_" + QString::number(tmpJ) + \
				"_" + QString::number(tmpI) + ".tif").toStdString();
			std::copy(std::begin(trans), std::end(trans), std::begin(windowTrans));
			calcWindowTrans(windowTrans, col * blockWidth, row * blockHeight);
			if (!saveTiffFromGDAL(windowSavePath, pSrcData, blockWidth, blockHeight,
				bandCount, types, projs, windowTrans, pBandMaps))
			{
				delete[] pSrcData;
				GDALClose(poDataset);
				GDALDestroyDriverManager();
				return false;
			}
			col++;
		}
		row++;
	}
	delete[] pSrcData;
	GDALClose(poDataset);
	GDALDestroyDriverManager();
	return true;
}

bool ImagePress::mergeTiff(QString imgDir)
{
	// 读取
	if (imgDir.isEmpty()) return false;
	QDir dir(imgDir);
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Name);
	dir.setNameFilters(QString("*.tiff;*.tif").split(";"));
	QStringList subDirList = dir.entryList();
	std::sort(subDirList.begin(), subDirList.end(),
		[](const QString& s1, const QString& s2)
		{
			QStringList fi1 = QFileInfo(s1).baseName().split("_");
			QStringList fi2 = QFileInfo(s2).baseName().split("_");
			int j1 = fi1.at(fi1.size() - 2).toInt();
			int i1 = fi1.at(fi1.size() - 1).toInt();
			int j2 = fi2.at(fi1.size() - 2).toInt();
			int i2 = fi2.at(fi1.size() - 1).toInt();
			if (j1 < j2) return true;
			else if (j1 > j2) return false;
			else
			{
				if (i1 < i2) return true;
				else return false;
			}
		}
	);
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	// 读取第一个块，获取信息
	GDALDataset* tmpDataset = (GDALDataset*)GDALOpen(
		(imgDir + QDir::separator() + subDirList.at(0)).toStdString().c_str(),
		GA_ReadOnly
	);
	if (tmpDataset == NULL)
	{
		GDALClose(tmpDataset);
		GDALDestroyDriverManager();
		return false;
	}
	int bandCount = tmpDataset->GetRasterCount();
	int* pBandMaps = new int[bandCount];
	for (int b = 0; b < bandCount; ++b) pBandMaps[b] = b + 1;
	int blockX = tmpDataset->GetRasterXSize();
	int blockY = tmpDataset->GetRasterYSize();
	double trans[6] = { 0 };
	tmpDataset->GetGeoTransform(trans);
	GDALDataType types = tmpDataset->GetRasterBand(1)->GetRasterDataType();
	const char* projs = tmpDataset->GetProjectionRef();
	GDALClose(tmpDataset);
	// 读取最后一个块的坐标信息
	QString tmp = subDirList.at(subDirList.size() - 1);
	QFileInfo tmpInfo = QFileInfo(tmp);
	QStringList ids = tmpInfo.baseName().split("_");
	int jInd = ids.at(ids.size() - 2).toInt();
	int iInd = ids.at(ids.size() - 1).toInt();
	int nImgSizeX = jInd + blockX;
	int nImgSizeY = iInd + blockY;
	// 创建
	QString imgPath = imgDir + QDir::separator() + "merge.tif";
	GDALDriver* pDriverMEM = GetGDALDriverManager()->GetDriverByName("GTiff");
	if (!pDriverMEM)
	{
		GDALDestroyDriverManager();
		return false;
	}
	GDALDataset* poDataset = pDriverMEM->Create(
		imgPath.toStdString().c_str(), nImgSizeX, nImgSizeY, bandCount, types, NULL);
	// 循环填入
	QFileInfo blockInfo;
	QStringList blockIds;
	void* tmpSrcData = nullptr;
	for (QString blockPath : subDirList)
	{
		blockInfo = QFileInfo(blockPath);
		blockIds = blockInfo.baseName().split("_");
		GDALDataset* tmpDataset = (GDALDataset*)GDALOpen(
			(imgDir + QDir::separator() + blockPath).toStdString().c_str(),
			GA_ReadOnly
		);
		jInd = blockIds.at(ids.size() - 2).toInt();
		iInd = blockIds.at(ids.size() - 1).toInt();
		if (tmpDataset == NULL)
		{
			GDALClose(tmpDataset);
			GDALDestroyDriverManager();
			return false;
		}
		ImagePress::createArr(&tmpSrcData, types, blockX, blockY, bandCount);
		tmpDataset->RasterIO(
			GF_Read, 0, 0, blockX, blockY, tmpSrcData, blockX, blockY,
			types, bandCount, pBandMaps, 0, 0, 0, NULL
		);
		poDataset->RasterIO(
			GF_Write, jInd, iInd, blockX, blockY, tmpSrcData, blockX, blockY,
			types, bandCount, pBandMaps, 0, 0, 0, NULL
		);
		delete[] tmpSrcData;
		GDALClose(tmpDataset);
		poDataset->FlushCache();
	}
	poDataset->SetProjection(projs);
	poDataset->SetGeoTransform(trans);
	GDALClose(poDataset);
	GDALDestroyDriverManager();
	return true;
}

cv::Mat ImagePress::qpixmapToCVMat(QPixmap pimg)
{
	QImage image = pimg.toImage();
	cv::Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(
			image.height(), image.width(), CV_8UC4,
			(void*)image.constBits(), image.bytesPerLine()
		);
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(
			image.height(), image.width(), CV_8UC3,
			(void*)image.constBits(), image.bytesPerLine()
		);
		cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
		break;
	case QImage::Format_Grayscale8:
		mat = cv::Mat(
			image.height(), image.width(), CV_8UC1,
			(void*)image.constBits(), image.bytesPerLine()
		);
		break;
	}
	return mat;
}

std::vector<int> ImagePress::calcOIF(QString hsiPath)
{
	GDALDataset* ds = (GDALDataset*)GDALOpen(
		hsiPath.toStdString().c_str(), GA_ReadOnly);
	int bandCount = ds->GetRasterCount();
	int width = ds->GetRasterXSize();
	int height = ds->GetRasterYSize();
	// 计算标准差和相关系数矩阵
	Eigen::MatrixXd coMatrix(bandCount, bandCount);
	std::vector<double> stdDev;
	for (int i = 1; i <= bandCount; ++i)  // 波段1
	{
		GDALRasterBand* band1 = ds->GetRasterBand(i);
		double* data1 = new double[width * height];
		band1->RasterIO(
			GF_Read, 0, 0, width, height, data1, width, height, GDT_Float64, 0, 0);
		Eigen::Map<Eigen::MatrixXd> mat1(data1, height, width);
		// 计算标准差
		double std = mat1.array().sqrt().matrix().mean();
		stdDev.push_back(std);
		for (int j = i + 1; j <= bandCount; ++j)  // 波段2
		{
			GDALRasterBand* band2 = ds->GetRasterBand(j);
			double* data2 = new double[width * height];
			band2->RasterIO(
				GF_Read, 0, 0, width, height, data2,
				width, height, GDT_Float64, 0, 0
			);
			Eigen::Map<Eigen::MatrixXd> mat2(data2, height, width);
			// 计算相关系数矩阵
			double corr = (mat1.array() - \
				mat1.mean()).matrix().normalized().cwiseProduct((mat2.array() - \
					mat2.mean()).matrix().normalized()).sum() / ((double)width * height);
			coMatrix(i - 1, j - 1) = corr;
			coMatrix(j - 1, i - 1) = corr;
			// 释放内存
			delete[] data2;
		}
		delete[] data1;
	}
	// 计算OIF值
	double maxOIF(0), oif(0);
	std::vector<int> best_bands = { 0, 0, 0 };
	for (int i = 1; i <= bandCount; ++i)
	{
		for (int j = i + 1; j <= bandCount; ++j)
		{
			for (int k = j + 1; k <= bandCount; ++k)
			{
				oif = (stdDev[i - 1] + stdDev[j - 1] + stdDev[k - 1]) / (\
					coMatrix(i - 1, j - 1) + \
					coMatrix(i - 1, k - 1) + \
					coMatrix(j - 1, k - 1));
				if (oif > maxOIF) {
					maxOIF = oif;
					best_bands[0] = i;
					best_bands[1] = j;
					best_bands[2] = k;
				}
			}
		}
	}
	GDALClose(ds);
	return best_bands;
}

bool ImagePress::maskIsEmpty(QString maskPath)
{
	bool isEmpty = false;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("GDAL_PAM_ENABLED", "NO");  // 不创建aux.xml文件
	GDALDataset* maskData = (GDALDataset*)GDALOpen(
		maskPath.toStdString().c_str(), GA_ReadOnly);
	double mMin, mMax, mMean, mStddev;
	maskData->GetRasterBand(1)->ComputeStatistics(
		true, &mMin, &mMax, &mMean, &mStddev, NULL, NULL);
	if (mMin == mMax && mMin == 0) isEmpty = true;
	GDALClose(maskData);
	GDALDestroyDriverManager();
	return isEmpty;
}
