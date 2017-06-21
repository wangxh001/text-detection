#ifndef _MSER_GET_CANDIDATE_
#define _MSER_GET_CANDIDATE_
#include "headfile.h"
#include <vector>
using namespace std;

class mserGetCandidate{
public:
	std::vector<cv::Rect> run(cv::Mat& srcImage);
	void preprocess();
	void featureExtract();
	void ExtractCCfeatures();
	void JdDetect();
	void Filter();
	//void getSubImages();

private:
	cv::MSER mser;
	//cv::MSER mser2;
	cv::Mat oriBgrImage_8UC3;
	cv::Mat gray_source_image;
	cv::Mat gray_source1_image;
	cv::Mat gray_dst_image;
	cv::Mat mserBinary_image;
	cv::Mat mserBinaryDilate_image;
	cv::Mat mserClose_image;
	cv::Mat bj_image;
	cv::Mat filt_image;
	cv::Mat jd_srcImage;
	cv::Mat jd_bj_image;
	cv::Mat jd_filt_image;
	vector<vector<cv::Point2i>> strVectorStore;  //MSER区域
	vector<vector<cv::Point2i>> ccStrVectorStore; //启发式规则过滤
	vector<vector<cv::Point2i>> filtStrVectorStore; //角点检测过滤
	vector<cv::Rect> candidateStore;
	//vector<vector<cv::Point2i>> candidateStore1;
	//vector<cv::Rect> subSrcImages;
};
#endif
