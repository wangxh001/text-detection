#include"groundDetect.h"

//using namespace cv;

void groundDetect::readImageGroundTruth(const cv::Mat& image, const std::vector<std::string>gt)
{
	oriBgrImage_8UC3 = image;
	pgts.clear();
	cgts.clear();
	int xupdate = (oriBgrImage_8UC3.cols < 950) ? oriBgrImage_8UC3.cols : 950;
	int yupdate = (oriBgrImage_8UC3.rows < 840) ? oriBgrImage_8UC3.rows : 840;
	double xratio = double(xupdate) / oriBgrImage_8UC3.cols;
	double yratio = double(yupdate) / oriBgrImage_8UC3.rows;
	cv::resize(oriBgrImage_8UC3,oriBgrImage_8UC3,cv::Size(xupdate,yupdate));
	for (int i = 0; i < gt.size(); i++)
	{
		pgts.push_back(polyGroundTruth(gt[i],xratio,yratio));
	}
}
void groundDetect::run()
{
	pointsStore = lc.run(oriBgrImage_8UC3);
	for (int i = 0; i < pointsStore.size(); i++)
	{
		Rect r = boundingRect(pointsStore[i]);
		RotatedRect rect = minAreaRect(pointsStore[i]);
		cgts.push_back(r);
		rectStore.push_back(rect);
	}
}

void groundDetect::showPolyGroundTruthSet(int k)
{
	//display();
	char fileName[100];
	for (int i = 0; i < cgts.size(); i++)
	{
		textDetectImage = oriBgrImage_8UC3(cgts[i]);
		char file_name[100];
		sprintf(file_name, "./resource/final/demo%d_%d.jpg", k,i);
		imwrite(file_name, oriBgrImage_8UC3(cgts[i]));
		textRectify(k, i);//�ı��н���
		//ts.textSegment(textDetectImage, k, i);
		rectangle(oriBgrImage_8UC3, cgts[i], cvScalar(0, 0, 255), 2);
	}
	cv::imshow("Final Result", oriBgrImage_8UC3);
	sprintf(fileName,"./resource/finalDetectResult/demo%d.jpg",k);
	imwrite(fileName,oriBgrImage_8UC3);
	cv::waitKey(20);
}

void groundDetect::textRectify(int k, int m)
{
	float width = textDetectImage.cols;
	float height = textDetectImage.rows;
	//ȷ��������ͼ���ĸ������λ��
	Point2f srcPoint[4];
	vector<Point2f> srcPoints(srcPoint, srcPoint + 4);
	rectStore[m].points(srcPoint);
	//��ͼ����ĸ����㰴���ϡ����ϡ����¡�����˳������
	cv::Point2f center = rectStore[m].center;;
	std::vector<cv::Point2i> top, bot;
	for (int i = 0; i < 4; i++)
	{
		if (srcPoint[i].y < center.y)
			top.push_back(srcPoint[i]);
		else
			bot.push_back(srcPoint[i]);
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];
	srcPoints.clear();
	srcPoints.push_back(tl);
	srcPoints.push_back(tr);
	srcPoints.push_back(bl);
	srcPoints.push_back(br);
	//�Գ�������ϵ�ĵ���н���
	for (int i = 0; i < 4; i++)
	{
		srcPoints[i].x = srcPoints[i].x - cgts[m].x;
		srcPoints[i].y = srcPoints[i].y - cgts[m].y;
		if (srcPoints[i].x < 0) srcPoints[i].x = 0;
		if (srcPoints[i].x>width) srcPoints[i].x = width;
		if (srcPoints[i].y < 0) srcPoints[i].y = 0;
		if (srcPoints[i].y>height) srcPoints[i].y = height;
		cout << "������ͼ������������:" << srcPoints[i] << endl;

	}
	//ȷ��Ŀ��ͼ����ĸ�����	
	std::vector<cv::Point2f> dstPoints;
	dstPoints.push_back(cv::Point2f(0, 0));
	dstPoints.push_back(cv::Point2f(width, 0));
	dstPoints.push_back(cv::Point2f(0, height));
	dstPoints.push_back(cv::Point2f(width, height));
	for (int i = 0; i < dstPoints.size(); i++)
	{
		cout << "Ŀ��ͼ�������:" << dstPoints[i] << endl;
	}
	//����ӳ�����
	cv::Mat transmtx = cv::getPerspectiveTransform(srcPoints, dstPoints);
	//����͸�ӱ任
	cv::Mat textRectifyImage = cv::Mat::zeros(textDetectImage.size(), CV_8UC3);
	cv::warpPerspective(textDetectImage, textRectifyImage, transmtx, textRectifyImage.size());
	char fileName[100];
	sprintf(fileName, "./resource/rectifyImage/demo%d_%d.jpg", k, m);
	imwrite(fileName, textRectifyImage);
	ts.textSegment(textRectifyImage, k, m);//�ı��ָ�

}

std::string groundDetect::calculateResult(int k)
{
	return fmeasure.run(cgts, pgts,k);
}