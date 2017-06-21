#include "strngs.h"  
#include "baseapi.h" 
#include"textRecognition.h"
using namespace cv;
#pragma  comment(lib,"libtesseract302d.lib") 

void textRecognition::textRecognize(int k, int m)
{
	char fileName[100];
	sprintf(fileName, "resource/textSegment_otsu/demo%d_%d.jpg", k, m);
	//char * str = "resource/binary_otsu/demo1_0.jpg"; //Ҫʶ��ͼ��ľ���·��
	char * str = fileName;
	tesseract::TessBaseAPI  api;
	api.Init(NULL, "chi_sim", tesseract::OEM_DEFAULT);  //��ʼ�����������԰������ļ��壺chi_sim; Ӣ�ģ�eng��Ҳ�����Լ�ѵ�����԰�    
	STRING text_out;   //��������ı���   
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