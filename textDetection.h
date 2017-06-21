#ifndef _TEXT_DETECTION_
#define _TEXT_DETECTION_

#include"groundDetect.h"
#include"headfile.h"
#include<string>
#include<vector>
#include<iostream>

class  textDetection
{
public:
	std::string detectText(const cv::Mat& image, const std::vector<std::string> gt, bool isshow, int i);
private:
	groundDetect gd;
};


#endif