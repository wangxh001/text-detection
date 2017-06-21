#ifndef _SWT_GET_CANDIDATE_
#define _SWT_GET_CANDIDATE_
#include "mserGetCandidate.h"
#include "Candidate.h"
#include "swt1.h"

using namespace std;
using namespace cv;
class swtGetCandidate
{
public:
	//swtGetCandidate(){};
	std::vector<Candidate> swtGetCandidate::run(cv::Mat& Image);
	void swtprocess();
	void Filter();
	
private:
	cv::Mat oriImage_8UC3;
	swt swt;
	std::vector<cv::Rect> filtStrVectorStore;
	std::vector<cv::Rect> filtStrVectorStore1;
	std::vector<Candidate> ccStore;
	std::vector<Candidate> swtCandidateStore;
};


#endif