#ifndef _TEXT_SEGMENTATION_
#define _TEXT_SEGMENTATION_

#include"headfile.h"
#include"textRecognition.h"

class textSegmentation
{
public:
	void textSegment(cv::Mat &image, int k, int m);

private:
	cv::Mat locationImage;
	cv::Mat binaryImage;
	textRecognition tr;

};
#endif