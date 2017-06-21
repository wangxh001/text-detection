#include "strngs.h"  
#include "baseapi.h" 
#include"textSegmentation.h"

#pragma  comment(lib,"libtesseract302d.lib")
void textSegmentation::textSegment(cv::Mat &image, int k, int m)
{
	locationImage = image;
	char fileName[100];
	cv::cvtColor(locationImage, binaryImage, CV_BGR2GRAY);//»Ò¶È»¯
	threshold(binaryImage, binaryImage, 0, 255, CV_THRESH_OTSU);
	sprintf(fileName, "./resource/textSegment_otsu/demo%d_%d.jpg", k, m);
	imwrite(fileName, binaryImage);
	tr.textRecognize(k, m);
}