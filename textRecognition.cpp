#include "strngs.h"  
#include "baseapi.h" 
#include"textRecognition.h"
using namespace cv;
#pragma  comment(lib,"libtesseract302d.lib") 

void textRecognition::textRecognize(int k, int m)
{
	char fileName[100];
	sprintf(fileName, "resource/textSegment_otsu/demo%d_%d.jpg", k, m);
	//char * str = "resource/binary_otsu/demo1_0.jpg"; //要识别图像的绝对路径
	char * str = fileName;
	tesseract::TessBaseAPI  api;
	api.Init(NULL, "chi_sim", tesseract::OEM_DEFAULT);  //初始化，设置语言包，中文简体：chi_sim; 英文：eng；也可以自己训练语言包    
	STRING text_out;   //定义输出的变量   
	if (!api.ProcessPages(str, NULL, 0, &text_out))
	{
		printf("Can not read %s this picture!!!", str);
		//return 0;
	}
	FileStorage file;
	char filename[100];
	sprintf(filename, "resource/textRecognition/demo%d_%d.txt", k, m);
	file.open(filename, FileStorage::WRITE);
	file << "result" << text_out.string();
	file.release();
}