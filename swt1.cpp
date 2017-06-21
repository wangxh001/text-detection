#include "swt1.h"
#define PI 3.14159265

bool Point2dSort(SWTPoint2d const & lhs, SWTPoint2d const & rhs)
{
	return lhs.SWT < rhs.SWT;
}

bool judgeTextColor(Mat &grayImage)
{
	long edgePixs = 0, midPixs = 0;
	int rows = grayImage.rows;
	int cols = grayImage.cols;
	int mid = (0 + rows - 1) / 2;
	for (int row = 0; row < 2; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			edgePixs += (int)grayImage.at<uchar>(row, col);
		}
	}
	for (int row = rows - 1; row >rows - 3; row--)
	{
		for (int col = 0; col < cols; col++)
		{
			edgePixs += (int)grayImage.at<uchar>(row, col);
		}
	}
	edgePixs = edgePixs / (4 * cols);
	//cout << "edgePixs=" << edgePixs << endl;

	for (int row = mid - 1; row < mid + 3; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			midPixs += (int)grayImage.at<uchar>(row, col);
		}
	}
	midPixs = midPixs / (4 * cols);
	//cout << "midPixs=" << midPixs << endl;
	if (midPixs>edgePixs)
		return false;
	else
		return true;
}

std::vector<Candidate> swt::initialize(cv::Mat& Image, vector< cv::Rect >& cc)
{
	Image.copyTo(srcImage);
	for (int i = 0; i < cc.size(); i++)
	{
		char file_img[100] = { 0 };
		subImageStore.push_back(srcImage(cc[i]));
		//sprintf(file_img, "resource/mserRect_image/demo9/%d.jpg", i);
		//imwrite(file_img, subImageStore[i]);         
	}

	bool dark_on_light =false;
	for (int i = 0; i < subImageStore.size(); i++)
	{
		cv::cvtColor(subImageStore[i], subGrayImage, CV_BGR2GRAY);//�ҶȻ�
		dark_on_light = judgeTextColor(subGrayImage);//�ж�ͼ��ļ��ԣ����������ֻ������װ���
	    //Canny��Ե���õ���Եͼ��,��Ե���ֵΪ255������ֵΪ0
		edges(subGrayImage, edge_Image);
		char edgeImg[100] = { 0 };
		//sprintf(edgeImg, "resource/mserRect_image/demo9/%d-canny.jpg", i);
		//imwrite(edgeImg, edgeImageStore[i]);
		//����SWT�������ݶ��߶�ʸ��
		Mat SWTImage(subImageStore[i].size(), CV_32FC1);
		SWTImage = cvCreateMat(subImageStore[i].rows, subImageStore[i].cols, CV_32FC1);
		//SWTͼ����ÿ���㸳��ֵΪ-1
		for (int row = 1; row < subImageStore[i].rows; row++)
		{
			float* ptr = (float*)SWTImage.ptr(row);
			for (int col = 1; col < subImageStore[i].cols; col++)
			{
				*ptr++ = -1;
			}
		}

		//�õ��ʻ���ȱ仯���ͼ��SWTImage
		vector<Ray> rays;
		float prec = .05;//ǰ������Ϊ0.05
		for (int row = 0; row < edge_Image.rows; row++)
		{
			const uchar* ptr = (const uchar*)edge_Image.ptr(row);
			for (int col = 0; col < edge_Image.cols; col++)
			{
				if (*ptr > 0) //�õ�Ϊ��ɫ��Ե
				{
					Ray r;
					SWTPoint2d p;
					p.x = col;
					p.y = row;
					r.p = p;
					std::vector<SWTPoint2d> points;
					points.push_back(p);
					float curX = (float)col + 0.5;
					float curY = (float)row + 0.5;
						int curPixX = col;
						int curPixY = row;
						float G_x = gradientX.at<float>(row, col);
						float G_y = gradientY.at<float>(row, col);
						// normalize gradient
						// �õ���ʼ����ݶ�
						float mag = sqrt((G_x * G_x) + (G_y * G_y));
						//�����ֵķ���
						if (dark_on_light)
						{
							G_x = -G_x / mag;
							G_y = -G_y / mag;
						}
						else
						{
							G_x = G_x / mag;
							G_y = G_y / mag;

						}
						while (true)
						{
							//���ݶ�ǿ�ȵ�0.05����ǰ
							curX += G_x*prec;
							curY += G_y*prec;
							//��Ϊ��ǰ���ص㣬˵����ǰ������������
							if ((int)(floor(curX)) != curPixX || (int)(floor(curY)) != curPixY)
							{
								curPixX = (int)(floor(curX));
								curPixY = (int)(floor(curY));
								// ��ǰ���س���ͼƬ��Χ��ֹͣ
								if (curPixX < 0 || (curPixX >= SWTImage.cols) || curPixY < 0 || (curPixY >= SWTImage.rows))
								{
									break;
								}
								SWTPoint2d pnew;
								pnew.x = curPixX;
								pnew.y = curPixY;
								points.push_back(pnew);

								//������һ��Ե�������ݶ�
								if (edge_Image.at<uchar>(curPixY, curPixX) > 0) {
									r.q = pnew;
									// dot product
									float G_xt = gradientX.at<float>(curPixY, curPixX);
									float G_yt = gradientY.at<float>(curPixY, curPixX);
									mag = sqrt((G_xt * G_xt) + (G_yt * G_yt));
									if (dark_on_light) {
										G_xt = -G_xt / mag;
										G_yt = -G_yt / mag;
									}
									else 
									{
										G_xt = G_xt / mag;
										G_yt = G_yt / mag;

									}
									//�����յ���ݶȷ�Χ����һ����Χ������Ч�������·����Ч
									if (acos(G_x * -G_xt + G_y * -G_yt) < PI / 2.0) {
										//�����ݶ��߶γ��ȣ����ʻ���ȣ����¸��߶�·���ϵĵ�ıʻ����ֵ
										float length = sqrt(((float)r.q.x - (float)r.p.x)*((float)r.q.x - (float)r.p.x) +
											((float)r.q.y - (float)r.p.y)*((float)r.q.y - (float)r.p.y));
										for (std::vector<SWTPoint2d>::iterator pit = points.begin(); pit != points.end(); pit++) {
											if (SWTImage.at<float>(pit->y, pit->x) < 0) {
												SWTImage.at<float>(pit->y, pit->x) = length;
											}
											else {
												SWTImage.at<float>(pit->y, pit->x) = std::min(length, SWTImage.at<float>(pit->y, pit->x));
											}
										}
										r.points = points;
										rays.push_back(r);
									}
									break;
								}
							}
						}
					}
					ptr++;//�����Ǳ�Ե��������Ѱ�ң���ĳһ��Ե���Ѵ������
				}
			}

		std::vector<float> temp;
		std::vector<SWTPoint2d> component;
		Candidate cd;
		cd.mean = 0;
		cd.variance = 0;
		cd.median = 0;
		cd.max = 0;
		float tempColor = 0;
		float tempr = 0;
		float tempg = 0;
		float tempb = 0;
		cd.swMaxRatio = 1;
		cd.swMinRatio = 1;
		cd.Rb = 1;
		cd.minx = cc[i].x;
		cd.miny = cc[i].y;
		cd.maxx = cd.minx + cc[i].width;
		cd.maxy = cd.miny + cc[i].height;
		cd.middle_i = (cd.minx + cd.maxx) / 2;
		cd.middle_j = (cd.miny + cd.maxy) / 2;
		cd.height = cc[i].height;
		cd.width = cc[i].width;
		cd.index = i;
		//	cout << "x=" << cd.minx << "," << "y=" << cd.miny << ";" << "x1=" << cd.maxx << "," << "y0=" << cd.maxy << endl;
		//������δ�������߶Σ�������ʻ���ֵ������������ֵ�����ص㣬У���սǴ����ص�ıʻ����
		if (rays.size() != 0)
		{
			for (auto& rit : rays)
			{
				for (auto& pit : rit.points) //��SWTImage��ֵ
				{
					pit.SWT = SWTImage.at<float>(pit.y, pit.x);
				}
				//�߶�·���ϵĵ㰴�ʻ���ȴ�С��������ȡ�м��ıʻ���ȣ����´����м��Ŀ��
				//����������
				std::sort(rit.points.begin(), rit.points.end(), &Point2dSort);
				float median = (rit.points[rit.points.size() / 2]).SWT;
				//��·���ϵ����е���бȽϸ���
				for (auto& pit : rit.points)
				{
					SWTImage.at<float>(pit.y, pit.x) = std::min(pit.SWT, median);
				}
			}

			//��������ͼ���ʻ���ȵľ�ֵ�������ֵ

			for (std::vector<Ray>::const_iterator it = rays.begin(); it != rays.end(); it++)
			{
				for (std::vector<SWTPoint2d>::const_iterator it2 = it->points.begin(); it2 != it->points.end(); it2++)
				{
					component.push_back(*it2);
				}
			}

			for (std::vector<SWTPoint2d>::const_iterator it = component.begin(); it != component.end(); it++)
			{
				float t = SWTImage.at<float>(it->y, it->x);
				if (t > cd.max)
					cd.max = t;
				cd.mean += t;
				temp.push_back(t);
				tempColor += subGrayImage.at <unsigned char>(it->y, it->x);
				tempr += subImageStore[i].at<cv::Vec3b>(it->y, it->x)[0];
				tempg += subImageStore[i].at<cv::Vec3b>(it->y, it->x)[1];
				tempb += subImageStore[i].at<cv::Vec3b>(it->y, it->x)[2];
			}
			cd.mean = cd.mean / (float)component.size();
			for (std::vector<float>::const_iterator it = temp.begin(); it != temp.end(); it++)
			{
				cd.variance += (*it - cd.mean) * (*it - cd.mean);
			}
			std::sort(temp.begin(), temp.end());
			cd.median = double(temp[temp.size() / 2]);
			cd.variance = cd.variance / (float)component.size();
			//cout << "mean=" << cd.mean << ", " << "variance=" << cd.variance << ", " << "median=" << cd.median <<", "<<"max="<<cd.max <<","<<"middle_i="<<cd.middle_i<<","<<"middle_j="<<cd.middle_j<<endl;

			cd.averageColor = tempColor / (float)component.size();
			cd.averager = tempr / (float)component.size();
			cd.averageg = tempg / (float)component.size();
			cd.averageb = tempb / (float)component.size();
			float var = 0;
			for (std::vector<SWTPoint2d>::const_iterator it = component.begin(); it != component.end(); it++)
			{
				var += (subGrayImage.at <unsigned char>(it->y, it->x) - cd.averageColor)*(subGrayImage.at <unsigned char>(it->y, it->x) - cd.averageColor);
			}
			cd.colorVariance = var / (float)component.size();
			for (std::vector<float>::const_iterator it = temp.begin(); it != temp.end() - 1; it++)
			{
				if (*it / (*(it + 1))<cd.swMinRatio)
					cd.swMinRatio = *it / (*(it + 1));
			}
			for (std::vector<float>::const_iterator it = temp.begin(); it != temp.end() - 1; it++)
			{
				if (*it / (*(it + 1))>cd.swMaxRatio)
					cd.swMaxRatio = *it / (*(it + 1));
			}
		}
			//cout << "mean=" << cd.mean<<"," << "middle_i=" << cd.middle_i << "," << "middle_j=" << cd.middle_j << "," << "averager=" << cd.averager << "," << "averageg= " << cd.averageg << "," << "averageb=" << cd.averageb<<","<<"height="<< cd.height<< endl;
			//cout << "swMaxRatio=" << cd.swMaxRatio <<","<< "swMinRatio=" << cd.swMinRatio <<","<<"averageColor="<<cd.averageColor<<","<<"colorVariance="<<cd.colorVariance<< endl;
			//CandidateStore.push_back(cd);
			
			//�ʻ����ֵת��Ϊ��ֵͼ�����ʾ
			Mat output2(subImageStore[i].size(), CV_32FC1);
			binaryImage(SWTImage, output2);
			saveSWT = cvCreateMat(subImageStore[i].rows, subImageStore[i].cols, CV_8UC1);
			output2.convertTo(saveSWT, CV_8UC1, 255);
			SWTImageStore.push_back(saveSWT);
			char swtImg[100] = { 0 };
			//sprintf(swtImg, "resource/mserRect_image/demo9/%d-swt.jpg", i);
			//imwrite(swtImg, SWTImageStore[i]);

			//�������������ص�
			Mat_<uchar>::iterator it = SWTImageStore[i].begin<uchar>();
			Mat_<uchar>::iterator itend = SWTImageStore[i].end<uchar>();
			for (; it != itend; ++it)
			{
				if ((*it)==0) cd.Rb += 1;//��ֵ�������ص���0����255
			}
			cd.Rb =cd.Rb/ (SWTImageStore[i].rows*SWTImageStore[i].cols);
			//cout << "Rb=" << cd.Rb << endl;
			CandidateStore.push_back(cd);
		}
		return CandidateStore;
	}

void swt::edges(Mat& subSrcImage,Mat &edgeImage)
{
	//cv::cvtColor(subSrcImage, subSrcImage, CV_BGR2GRAY);
	GaussianBlur(subSrcImage, subSrcImage, Size(5, 5), 0, 0);
	Canny(subSrcImage, edgeImage, 150, 100, 3);
	edgeImageStore.push_back(edgeImage);
	
	//����x��y�����ϵ��ݶ�
	Mat gaussianImage(subSrcImage.size(), CV_32FC1);
	gradientX = cvCreateMat(subSrcImage.rows, subSrcImage.cols, CV_32FC1);
	gradientY = cvCreateMat(subSrcImage.rows, subSrcImage.cols, CV_32FC1);
	subSrcImage.convertTo(gaussianImage, CV_32FC1, 1. / 255.);
	GaussianBlur(gaussianImage, gaussianImage, Size(5, 5), 0);

	//��ԭͼ�����һ�£�x�������ݶ�
	Scharr(gaussianImage, gradientX, -1, 1, 0);
	GaussianBlur(gradientX, gradientX, Size(3, 3), 0);
	//gradientStore_X.push_back(gradientX);
	Scharr(gaussianImage, gradientY, -1, 0, 1);
	GaussianBlur(gradientY, gradientY, Size(3, 3), 0);
}

void swt::binaryImage(Mat& SWTImage, Mat& output2)
	{
		assert(SWTImage.depth() == CV_32F);
		assert(SWTImage.channels() == 1);
		assert(output2.depth() == CV_32F);
		assert(output2.channels() == 1);
		for (int row = 0; row < SWTImage.rows; row++) {
			const float* ptrin = (const float*)SWTImage.ptr(row);
			float* ptrout = (float*)output2.ptr(row);
			for (int col = 0; col < SWTImage.cols; col++) {
				if (*ptrin < 0) {
					*ptrout = 1;
				}
				else {
					*ptrout =0;
				}
				ptrout++;
				ptrin++;
			}
		}
	}