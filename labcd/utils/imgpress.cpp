#include <iostream>
#include <fstream>
#include <json/json.h>
#include "imgpress.h"

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
		QList<LabPolygon*> polygons
)
{
	// 处理保存路径
	QStringList pathAndName = savePath.split(".");
	std::string iPath = pathAndName[0].toStdString();
	std::string iExt = pathAndName[1].toStdString();
	cv::String pseudoSavaPath = iPath + "_pseudo." + iExt;  // 保存伪彩色
	cv::String baseSavaPath = iPath + ".bmp";  // 保存标签
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
					points.append(poly->mPoints.at(i)->x());
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
	cv::imwrite(baseSavaPath, result);
	os << writer.write(polyList);
	os.close();  // 关闭json
}
