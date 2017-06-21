#ifndef _LINK_CANDIDATE_
#define _LINK_CANDIDATE_
#include "Candidate.h"
#include "swtGetCandidate.h"
#include <vector>
#include <set>
#include <math.h>

using namespace std;
using namespace cv;

struct Chain
{
	Candidate p;
	Candidate q;
	std::vector<Candidate> components;
	float dist;
	Point2f direction;
	bool merged;
	bool flag;
};

class linkCandidate
{
public:
	//std::vector<Rect> linkCandidate::run(cv::Mat &image);
	std::vector<vector<cv::Point2i>> linkCandidate::run(cv::Mat &image);
	void makeChains();
	void renderChains();
	//void renderChainsWithBoxes();

private:
	std::vector<Candidate> candidateStore;
	Mat oriImage_8UC3;
	int candidateNum;
	//std::vector<cv::Rect>rectStore;
	std::vector<Chain>chains;
	vector<cv::Point2i> points;
	vector<vector<cv::Point2i>> pointStore;
};

#endif
