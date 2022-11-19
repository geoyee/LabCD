#pragma once

#include <opencv2/opencv.hpp>
#include <QString>
#include "../widgets/labpolygon.h"

class ImagePress
{
public:
	static cv::Mat CVA(cv::Mat t1, cv::Mat t2);

	static bool saveMaskFromPolygon(
		QString savePath,
		int imageWidth,
		int imageHight,
		QList<LabPolygon*> polygons
	);
};