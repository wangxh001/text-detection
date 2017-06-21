#ifndef _GROUND_DETECT_
#define _GROUND_DETECT_

#include<string>
#include"headfile.h"
#include"Fmeasure.h"
#include"polyGroundTruth.h"
#include"linkCandidate.h"
#include"textSegmentation.h"

class groundDetect
{
public:
	void readImageGroundTruth(const cv::Mat& image, const std::vector<std::string>gt);
	void run();
	void showPolyGroundTruthSet(int k);
	//void textSegmentation(int k, int m);
	void textRectify(int k, int i);//文本行矫正
	void textSegment(cv::Mat &image, int k, int m);
	std::string calculateResult(int k);
	int num = 1;

private:
	linkCandidate lc;
	Fmeasure fmeasure;
	cv::Mat oriBgrImage_8UC3;
	polyGroundTruthSet pgts;
	std::vector<cv::Rect> cgts;
	std::string PRF;
	cv::Mat textDetectImage;
	textSegmentation ts;
	cv::Mat textRectifyImage;
	vector<vector<cv::Point2i>> pointsStore;
	vector<RotatedRect> rectStore;
};

#endif