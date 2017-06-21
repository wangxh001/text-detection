#ifndef _FMEASURE_
#define _FMEASURE_

#include<string>
#include<vector>
#include"headfile.h"
#include"polyGroundTruth.h"

class Fmeasure
{
public:
	std::string run(std::vector<cv::Rect>& cgts, polyGroundTruthSet &pgts,int k);
	double intersection(const cv::Rect &r1, const cv::Rect &r2);
	double Union(const cv::Rect &r1, const cv::Rect &r2);
	double Precision(std::vector<cv::Rect>& cgts, polyGroundTruthSet &pgts);
	double Recall(polyGroundTruthSet &pgts, std::vector<cv::Rect>& cgts);

};


#endif