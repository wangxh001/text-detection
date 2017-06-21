#ifndef _POLYGROUNDTRUTH_
#define _POLYGROUNDTRUTH_

#include "headfile.h"
#include"tools.h"
#include <string>
#include<vector>
#include<fstream>

class polyGroundTruth
{
public:
	polyGroundTruth(){};
	polyGroundTruth(std::string s, double xratio, double yratio);

private:
	cv::Rect rr;
	friend class polyGroundTruthSet;
	friend class Fmeasure;

};

class polyGroundTruthSet
{
public:
	//void display(cv::Mat& image);
	void push_back(polyGroundTruth pgt){ sets.push_back(pgt); };
	void clear(){ sets.clear(); }

private:
	std::vector<polyGroundTruth> sets;
	friend class Fmeasure;

};

#endif