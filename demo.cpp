#include "linkCandidate.h"
#include<string>
#include<fstream>
#include"textDetection.h"
#include"tools.h"
#define N 5

using namespace std;
using namespace cv;

double t = (double)cvGetTickCount();
int main()
{
	char file_img[100];
	char file_data[100];
	double totalPrecision = 0.0;
	double totalRecall = 0.0;
	double totalFmeasure = 0.0;
	for (int i = 1; i <= N; i++)
	{
		vector<string> gtset;
		string line;
		sprintf(file_img, "resource/src/demo%d.jpg", i);
		//cv::Mat srcImage = cv::imread("resource/src/demo1.jpg");
		cv::Mat srcImage = cv::imread(file_img);
		if (srcImage.empty())
			return -1;
		imshow("srcImage", srcImage);
		sprintf(file_data, "resource/src/demo%d", i);
		ifstream fin(file_data);
		while (getline(fin, line))
		{
			gtset.push_back(line);
		}
		textDetection td;
		string str = td.detectText(srcImage, gtset, true, i);
		vector<std::string> vec = tools::split(str, ",");
		totalPrecision += atof(vec[0].c_str());
		totalRecall += atof(vec[1].c_str());
		totalFmeasure += atof(vec[2].c_str());
				
	}
	cout << "totalPrecision:" << totalPrecision / N << "\n" << "totalRecall:" << totalRecall / N << "\n" << "totalFmeasure:" << totalFmeasure / N<<endl;
	t = ((double)cvGetTickCount() - t) / (cvGetTickFrequency() * 1000);
	cout << "处理时间: " << t << "ms" << endl;
	cv::waitKey();
	//return 0;
}