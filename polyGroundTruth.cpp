#include"polyGroundTruth.h"

using namespace std;

polyGroundTruth::polyGroundTruth(std::string s, double xupdate, double yupdate)
{
	vector<cv::Point> vec;
	vector<std::string> ret = tools::split(s, " ");
	cv::Point pt = cv::Point(atoi(ret[0].c_str())*xupdate, atoi(ret[1].c_str())*yupdate);
	vec.push_back(pt);
	cv::Point pt1 = cv::Point(atoi(ret[2].c_str())*xupdate, atoi(ret[3].c_str())*yupdate);
	vec.push_back(pt1);
	rr = cv::boundingRect(vec);

}