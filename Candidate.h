#ifndef _CANDIDATE_
#define _CANDIDATE_

///Singal Character detected by GetCandidate 
class Candidate
{
public:
	int area;
	int maxx, maxy, minx, miny;
	double mean, variance, median,swMaxRatio,swMinRatio;
	double Rb; //��ֵͼ���ɫ���ر���
	//int counter;
	int max;
	double middle_i, middle_j;
	//double strokeWidh,strokeVariance;
	//double pointsNum;
	double averageColor, averager, averageg, averageb;
	double colorVariance;
	//double strokeWidthRatio;
	double height;
	double width;
	double heightWidthRatio;
	//double grayRatio;
	int index;
	//int chuan;
	int alive;
	//Rect rect;
};

#endif