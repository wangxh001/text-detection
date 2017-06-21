#include"mserGetCandidate.h"
using namespace std;
using namespace cv;

std::vector<cv::Rect> mserGetCandidate::run(cv::Mat& srcImage)
{
	oriBgrImage_8UC3 = srcImage;
	preprocess(); 
	featureExtract();                                                           
	Filter();
	//getSubImages();
	return candidateStore;
}

void mserGetCandidate::preprocess()
{
	cvtColor(oriBgrImage_8UC3, gray_source_image, CV_BGR2GRAY); //灰度化
	//imshow("gray_source_image",gray_source_image);
	GaussianBlur(gray_source_image, gray_source1_image, Size(3, 3), 0, 0);
	//imshow("gray_source1_image", gray_source1_image);
	equalizeHist(gray_source1_image, gray_dst_image); //直方图均衡化
	//imshow("gray_dst_image",gray_dst_image);
}

void mserGetCandidate::featureExtract()
{
	// 提取MSER
	strVectorStore.clear();
	mser(gray_dst_image, strVectorStore);
	//二值化
    mserBinary_image = Mat::zeros(gray_dst_image.size(), CV_8UC1);
	for (int i = (int)strVectorStore.size() - 1; i >= 0; i--)
	{
		// 根据检测区域点生成mser结果  
		const vector<Point>& r = strVectorStore[i];
		for (int j = 0; j < (int)r.size(); j++)
		{
			Point pt = r[j];
			mserBinary_image.at<unsigned char>(pt) = 255;
		}
	}
	// mser结果输出  
	//imwrite("resource/mserBinary_image/demo12.jpg", mserBinary_image);
	//imshow("mserBinary_image", mserBinary_image);
	//膨胀
	dilate(mserBinary_image, mserBinaryDilate_image, getStructuringElement(MORPH_RECT, Size(3, 3)));
	imshow("dilateImage", mserBinary_image);
	//闭运算连接各个笔画
	morphologyEx(mserBinaryDilate_image, mserClose_image, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(3, 3)));
	//morphologyEx(mserBinary_image, mserClose_image, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(7,8)));
	//imshow("mserBinaryClose_image", mserClose_image);
	//imwrite("resource/mserClose_image/demo12.jpg",mserClose_image);
}

void mserGetCandidate::ExtractCCfeatures()
{
	bj_image = mserClose_image.clone();
	findContours(mserClose_image, strVectorStore, RETR_LIST, CHAIN_APPROX_NONE, Point());
	//findContours(mserClose_image, strVectorStore, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());
	Mat lunkuo = Mat::zeros(oriBgrImage_8UC3.size(), CV_8UC1);
	for (int i = 0; i < strVectorStore.size(); i++)
	{
		drawContours(lunkuo, strVectorStore, i, Scalar(255));
	}
	imshow("lunkuo", lunkuo);
	//ccStore.clear();
	ccStrVectorStore.clear();
	int Num = strVectorStore.size();
	//Candidate cd;
	for (int i = 0; i < Num; i++)
	{
		// 二值化后的MSER用矩形框标记
		Rect rect = boundingRect(strVectorStore[i]);
		rectangle(bj_image, rect, cvScalar(255, 0, 0), 2);
		// 启发式规则过滤
		double area = 0;
		area = fabs(contourArea(Mat(strVectorStore[i], true), true));
		if (rect.height*rect.width>25 && (rect.height / (double)rect.width>0.1&&rect.height / (double)rect.width<10) && (rect.height>10 && rect.height<300) &&
			(rect.width>10 && rect.width<300) && (area / (rect.height*rect.width)>0.2 &&area / (rect.height*rect.width)<0.95) && 
			rect.width<0.4*oriBgrImage_8UC3.cols && rect.height<0.8*oriBgrImage_8UC3.rows)
			//filtStrVectorStore.push_back(strVectorStore[i]);
		ccStrVectorStore.push_back(strVectorStore[i]);
	}
	//imshow("bj_image",bj_image);
	//imwrite("resource/bj_image/demo12-close.jpg", bj_image);
}

void mserGetCandidate::JdDetect()
{
	vector<Point2f> corners;
	Mat jd_image = oriBgrImage_8UC3.clone();
	//Mat jd_image = Mat::zeros(oriBgrImage_8UC3(), CV_8UC1);
    goodFeaturesToTrack(gray_source_image, corners, 1000, 0.01, 10, Mat(), 3, false, 0.04);
	int r = 2;
	for (int i = 0; i < corners.size(); i++)
	{
		circle(jd_image, corners[i], r, Scalar(0, 0, 255), -1, 8, 0);
		//cout << "num:" << corners[i] << endl;
	}
	//imshow("角点检测后的效果图", jd_image);
	//imwrite("resource/jd_srcImage/demo12.jpg", jd_image);
	// 原图像角点标记
	jd_bj_image = jd_image.clone();
	filtStrVectorStore.clear();
	vector<Point2f> points;
	for (int i = 0; i<ccStrVectorStore.size(); i++)
	{
		Rect rect = boundingRect(ccStrVectorStore[i]);
		rectangle(jd_bj_image, rect, cvScalar(0,255,0), 2);
		// 根据角点特征过滤
		int num = 0;
		points.clear();
		for (int j = 0; j<corners.size(); j++)
		{
			if ((corners[j].x >= rect.x &&corners[j].x <= (rect.x + rect.width)) && (corners[j].y >= rect.y &&corners[j].y <= (rect.y + rect.height)))
			{
				num++;
				points.push_back(corners[j]);
			}
		}
		if (num>3)
		{
			Rect box = boundingRect(Mat(points));
			rectangle(jd_bj_image, box, cvScalar(255, 0, 255), 2);
			if (box.height>0.4*rect.height&& box.width>0.5*rect.width)
			  filtStrVectorStore.push_back(ccStrVectorStore[i]);
		}
	}
	//imshow("jd_bj_image", jd_bj_image);
	//imwrite("resource/jd_bj_image/demo6-close12.jpg",jd_bj_image);
}

void mserGetCandidate::Filter()
{
	ExtractCCfeatures();
	JdDetect();
	//jd_filt_image = Mat::zeros(oriBgrImage_8UC3.size(), CV_8UC1);
	jd_filt_image = oriBgrImage_8UC3.clone();
	candidateStore.clear();
	//subSrcImages.clear();
	for (int i = 0; i <filtStrVectorStore.size(); i++)
	{
		//drawContours(jd_filt_image, filtStrVectorStore, i, Scalar(255), CV_FILLED);
		Rect rect = boundingRect(filtStrVectorStore[i]);
		rectangle(jd_filt_image, rect, cvScalar(0, 0,255), 2);
		candidateStore.push_back(rect);
		//subSrcImages.push_back(rect);
	}
	//imshow("bj_filt_image", jd_filt_image);
	//imwrite("resource/jd_res_image/demo6-close12.jpg",jd_filt_image);
}


