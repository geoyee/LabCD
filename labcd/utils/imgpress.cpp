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