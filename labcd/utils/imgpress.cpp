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

// TODO: 保存带调色板的单通道图像
void ImagePress::saveMaskFromPolygon(
		QString savePath,
		int labNum,
		int imgHeight,
		int imgWidth,
		QList<LabPolygon*> polygons
)
{
	cv::Mat result = cv::Mat::zeros(cv::Size(imgWidth, imgHeight), CV_8UC3);
	// 标号小的覆盖标号大的
	for (int labIndex = labNum - 1; labIndex >= 0; labIndex--)
	{
		for (LabPolygon* poly : polygons)
		{
			if (poly->labelIndex == labIndex)
			{
				const int numPoint = poly->mPoints.count();
				QColor c = poly->getColor();
				cv::Scalar color = cv::Scalar(c.blue(), c.green(), c.red());
				cv::Point** cvPoints = new cv::Point * [1];
				cvPoints[0] = new cv::Point[numPoint];
				for (int i = 0; i < numPoint; i++)
				{
					cvPoints[0][i] = cv::Point(
						poly->mPoints.at(i)->x(), poly->mPoints.at(i)->y()
					);
				}
				const cv::Point* ppt[1] = { cvPoints[0] };
				int npt[] = { numPoint };
				cv::fillPoly(result, ppt, npt, 1, color);
				// 清除数组
				delete[] cvPoints[0];
				delete[] cvPoints;
			}
		}
	}
	cv::String cvSavaPath = savePath.toStdString();
	cv::imwrite(cvSavaPath, result);
}
