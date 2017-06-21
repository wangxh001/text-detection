#ifndef _SWT1_H_
#define _SWT1_H_
#include "headfile.h"
#include "Candidate.h"
#include <iostream>
#include <vector>
#include <set>
using namespace std;
using namespace cv;

struct SWTPoint2d
{
	int x;
	int y;
	float SWT;
};

struct Ray
{
	SWTPoint2d p;
	SWTPoint2d q;
	std::vector<SWTPoint2d> points;
};

class swt
{
public:
	std::vector<Candidate> initialize(cv::Mat& Image, vector< cv::Rect >& cc);
	void edges(Mat& subSrcImage, Mat &edgeImage);
	void swt::binaryImage(Mat& SWTImage, Mat& output2);
private:
	cv::Mat srcImage;
	cv::Mat subGrayImage;
	cv::Mat edge_Image;
	cv::Mat gradientX;
	cv::Mat gradientY;
	cv::Mat saveSWT;
	vector<cv::Mat> subImageStore;
	//vector<cv::Mat> subGrayStore;
	vector<cv::Mat> edgeImageStore;
	vector<cv::Mat> SWTImageStore;
	vector<cv::Mat> swtResImageStore;
	vector<Candidate> CandidateStore;
};

#endif