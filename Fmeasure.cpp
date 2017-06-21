#include"Fmeasure.h"

using namespace std;
using namespace cv;

double Fmeasure::intersection(const cv::Rect &r1, const cv::Rect &r2)
{
	Rect recI = r1&r2;
	return recI.area();
}
double Fmeasure::Union(const cv::Rect &r1, const cv::Rect &r2)
{
	Rect recU = r1 | r2;
	return recU.area();
}

double Fmeasure::Precision(std::vector<cv::Rect>& cgts, polyGroundTruthSet &pgts)
{
	vector<cv::Rect> sets = cgts;
	polyGroundTruthSet T = pgts;
	if (sets.size() == 0 || T.sets.size() == 0)
	{
		return 0;
	}
	double total = 0;
	for (int i = 0; i < cgts.size(); i++)
	{
		double maxval = 0;
		for (int j = 0; j < T.sets.size(); j++)
		{
			double candval = intersection(cgts[i], T.sets[j].rr) / Union(cgts[i], T.sets[j].rr);
			if (candval>maxval)
			{
				maxval = candval;
			}
		}
		total += maxval;
	}
	return total / cgts.size();
}

double Fmeasure::Recall(polyGroundTruthSet &pgts, std::vector<cv::Rect>& cgts)
{
	vector<cv::Rect> sets = cgts;
	polyGroundTruthSet T = pgts;
	if (sets.size() == 0 || T.sets.size() == 0)
	{
		return 0;
	}
	double total = 0;
	for (int i = 0; i < T.sets.size(); i++)
	{
		double maxval = 0;
		for (int j = 0; j < sets.size(); j++)
		{
			double candval = intersection(T.sets[i].rr, sets[j]) / Union(T.sets[i].rr, sets[j]);
			if (candval>maxval)
			{
				maxval = candval;
			}

		}
		total += maxval;
	}
	return total / T.sets.size();
}

std::string Fmeasure::run(std::vector<cv::Rect>& cgts, polyGroundTruthSet &pgts,int k)
{
	double precision = Precision(cgts, pgts);
	double recall = Recall(pgts, cgts);
	double fmeasure;
	if (precision == 0 || recall == 0) fmeasure = 0;
	else
		fmeasure = 2 * precision*recall / (precision + recall);
	char ret[100];
	sprintf(ret, "%f,%f,%f ", precision, recall, fmeasure);
	FileStorage file;
	char filename[100];
	sprintf(filename, "resource/data/data%.xml", k);
	file.open(filename, FileStorage::WRITE);
	file << "precision" << precision;
	file << "recall" << recall;
	file << "fmeasure" << fmeasure;
	file.release();
	return string(ret);

}