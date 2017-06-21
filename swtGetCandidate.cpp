#include "swtGetCandidate.h"
using namespace std;
using namespace cv;

std::vector<Candidate> swtGetCandidate::run(cv::Mat& Image)
{
	oriImage_8UC3 = Image;
	mserGetCandidate mgc;
    filtStrVectorStore = mgc.run(oriImage_8UC3);
	swtprocess();
	Filter();
	return swtCandidateStore;
}

void swtGetCandidate::swtprocess()
{
	ccStore.clear();
	ccStore = swt.initialize(oriImage_8UC3, filtStrVectorStore);
	
}

void swtGetCandidate::Filter()
{
	swtCandidateStore.clear();
	for (int i = 0; i < filtStrVectorStore.size(); i++)
	{
		if (ccStore[i].variance < ccStore[i].mean*1.5 && (ccStore[i].max / ccStore[i].median>0.2&&ccStore[i].max / ccStore[i].median<5) &&
			(ccStore[i].swMaxRatio<3 && ccStore[i].swMinRatio>1 / 3)&&(ccStore[i].Rb>0.2&&ccStore[i].Rb<0.8))
		{
			swtCandidateStore.push_back(ccStore[i]);
			filtStrVectorStore1.push_back(filtStrVectorStore[i]);
		}
	}
	for (int i = 0; i < filtStrVectorStore1.size(); i++)
	{
		//rectangle(oriImage_8UC3, filtStrVectorStore1[i], cvScalar(0, 0, 255), 2);
	}
	//imshow("swtFilt_image", oriImage_8UC3);

}