#include "textDetection.h"

std::string textDetection::detectText(const cv::Mat& image, const std::vector<std::string> gt, bool isshow,int i)
{
	gd.readImageGroundTruth(image, gt);
	gd.run();
	if (isshow == true)
	{
		gd.showPolyGroundTruthSet(i);
	}
	if (gt.size() == 0)
	{
		return "";
	}
	else
	{
		return gd.calculateResult(i);
	}
}