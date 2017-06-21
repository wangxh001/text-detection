#include "linkCandidate.h"
#define PI 3.14159265

bool chainSortDist(const Chain &lhs, const Chain &rhs) {
	return lhs.dist < rhs.dist;
}

bool chainSortLength(const Chain &lhs, const Chain &rhs) {
	return lhs.components.size() > rhs.components.size();
}

//两字符对有一端是相同的
bool sharesOneEnd(Chain c0, Chain c1) {
	if (c0.p.index == c1.p.index || c0.p.index == c1.q.index || c0.q.index == c1.q.index || c0.q.index == c1.p.index) {
		return true;
	}
	else {
		return false;
	}
}

//std::vector<Rect> linkCandidate::run(cv::Mat &image)
std::vector<vector<cv::Point2i>> linkCandidate::run(cv::Mat &image)
{
	oriImage_8UC3=image;
	swtGetCandidate sgc;
	candidateStore = sgc.run(oriImage_8UC3);
	//vector<Chain> chainStore;
	//chainStore=makeChains();
	makeChains();
	renderChains();
	//renderChainsWithBoxes();
	//return rectStore;
	return pointStore;
}

//std::vector<Chain> linkCandidate::makeChains()
void linkCandidate::makeChains()
{
	//std::vector<Chain> chains;
	candidateNum = candidateStore.size();
	for (unsigned int i = 0; i < candidateNum; i++)
	{
		for (unsigned int j = i+1; j < candidateNum; j++)
		{
			if ((candidateStore[i].mean / candidateStore[j].mean <=2.0 && candidateStore[j].mean / candidateStore[i].mean <=2.0)&&
				(candidateStore[i].height/candidateStore[j].height<=2.0 && candidateStore[j].height/candidateStore[i].height<=2.0)) 
			{
				float dist = sqrt((candidateStore[i].middle_i - candidateStore[j].middle_i)*(candidateStore[i].middle_i - candidateStore[j].middle_i)
					+ (candidateStore[i].middle_j - candidateStore[j].middle_j)*(candidateStore[i].middle_j - candidateStore[j].middle_j));
				float colorDist = (candidateStore[i].averager - candidateStore[j].averager)*(candidateStore[i].averager - candidateStore[j].averager)+
					(candidateStore[i].averageg - candidateStore[j].averageg)*(candidateStore[i].averageg - candidateStore[j].averageg)+
					(candidateStore[i].averageb - candidateStore[j].averageb)*(candidateStore[i].averageb - candidateStore[j].averageb);
				if (dist < 3* (float)std::max(std::min(candidateStore[i].height, candidateStore[i].width), std::min(candidateStore[j].height, candidateStore[j].width))
					&&colorDist<1600)
				//if (dist < 2 * (float)std::max(candidateStore[i].width, candidateStore[j].width)&&colorDist<1600)
				{
					Chain c;
					c.p = candidateStore[i];
					c.q = candidateStore[j];
					std::vector<Candidate> comps;
					comps.push_back(c.p);
					comps.push_back(c.q);
					c.components = comps;
					c.dist = dist;
					//计算方向和距离
					float d_x = candidateStore[j].middle_i - candidateStore[i].middle_i;
					float d_y = candidateStore[j].middle_j - candidateStore[i].middle_j;
					float mag = sqrt(d_x*d_x + d_y * d_y);
					d_x = d_x / mag;
					d_y = d_y / mag;
					Point2f dir;
					dir.x = d_x;
					dir.y = d_y;
					c.direction = dir;
					chains.push_back(c);
				}
			}
		}
	}
	std::cout << chains.size()<<" "<< "elgible pairs" << ","<<candidateNum<<endl;
	std::sort(chains.begin(), chains.end(), &chainSortDist);
	int k = 0;
	for (vector<Chain>::iterator it = chains.begin(); it != chains.end(); it++,k++)
	{
		//k++;
		std::cout << "第" << k << "组的结果" << it->p.index << "," << it->q.index << endl;
	}
	//return chains;
}

void linkCandidate::renderChains()
{
	const float strictness = PI/12.0;
	//文本对融合
	int merges = 1;
	while (merges > 0)
	{   //所有对初始化为未融合
		for (unsigned int i = 0; i < chains.size(); i++)
		{
			chains[i].merged = false;
			chains[i].flag = false;
		}
		merges = 0;
		std::vector<Chain> newchains;
		for (unsigned int i = 0; i < chains.size(); i++)
		{
			for (unsigned int j = 0; j < chains.size(); j++)
			{
				if (i != j)
				{ //未融合过且有一头相同
					if (!chains[i].merged&&!chains[j].merged && sharesOneEnd(chains[i], chains[j]))
					{ //左边重合且方向在范围内
						if (chains[i].p.index == chains[j].p.index)
						{
							if ((acos(chains[i].direction.x*-chains[j].direction.x + chains[i].direction.y*-chains[j].direction.y) < strictness))
							{
								std::cout << "情况1：左重合：" << endl;
								std::cout << "chains[" << i << "].p.index=" << chains[i].p.index << "," << "chains[" << i << "].q.index=" << chains[i].q.index << ";" << "chains[" << j << "].p.index=" <<
									chains[j].p.index << "," << "chains[" << j << "].q.index=" << chains[j].q.index << endl;
								std::cout << "i=" << i << "," << "j=" << j << ";" << "chains[" << i << "].direction.x=" << chains[i].direction.x << "," <<
									"chains[" << j << "].direction.x=" << chains[j].direction.x << endl;
								std::cout << "值=" << chains[i].direction.x*-chains[j].direction.x + chains[i].direction.y*-chains[j].direction.y << ","
									<< "角度=" << acos(chains[i].direction.x*-chains[j].direction.x + chains[i].direction.y*-chains[j].direction.y) << endl;
								chains[i].p= chains[j].q;
								for (std::vector<Candidate>::iterator it = chains[j].components.begin(); it != chains[j].components.end(); it++)
								{
									chains[i].components.push_back(*it);
								}
								float d_x = chains[i].q.middle_i - chains[i].p.middle_i;
								float d_y = chains[i].q.middle_j - chains[i].p.middle_j;
								chains[i].dist = d_x*d_x + d_y*d_y;
								float mag = sqrt(d_x*d_x + d_y*d_y);
								d_x = d_x / mag;
								d_y = d_y / mag;
								Point2f dir;
								dir.x = d_x;
								dir.y = d_y;
								chains[i].direction = dir;
								chains[j].merged = true;
								chains[i].flag = true;
								merges++;
								int z = j;
								for (j = 0; j < chains.size(); j++)
								{
									if ((chains[j].p.index == chains[z].p.index || chains[j].q.index == chains[z].p.index) && !chains[j].merged)
									{
										chains[j].merged = true;
									}
								}
								j = 0;
							}
							else
							{
								if (chains[i].flag == true) chains[j].merged = true;
								//chains[j].merged = true;
							}
						}
						else if (chains[i].p.index == chains[j].q.index)
						{
							if ((acos(chains[i].direction.x*chains[j].direction.x + chains[i].direction.y*chains[j].direction.y) < strictness))
							{
								std::cout << "情况2：左与右重合：" << endl;
								std::cout << "chains[" << i << "].p.index=" << chains[i].p.index << "," << "chains[" << i << "].q.index=" << chains[i].q.index << ";" << "chains[" << j << "].p.index=" <<
									chains[j].p.index << "," << "chains[" << j << "].q.index=" << chains[j].q.index << endl;
								std::cout << "i=" << i << "," << "j=" << j << ";" << "chains[i].direction.x=" << chains[i].direction.x << "," << "chains[j].direction=" << 
									chains[j].direction.x<< endl;
								std::cout << "值=" << chains[i].direction.x*chains[j].direction.x + chains[i].direction.y*chains[j].direction.y << endl;
								std::cout << "角度=" << acos(chains[i].direction.x*chains[j].direction.x + chains[i].direction.y*chains[j].direction.y) << endl;
								chains[i].p = chains[j].p;
								for (std::vector<Candidate>::iterator it = chains[j].components.begin(); it != chains[j].components.end(); it++)
								{
									chains[i].components.push_back(*it);
								}
								float d_x = chains[i].q.middle_i - chains[i].p.middle_i;
								float d_y = chains[i].q.middle_j - chains[i].p.middle_j;
								float mag = sqrt(d_x*d_x + d_y*d_y);
								chains[i].dist = d_x*d_x + d_y*d_y;
								d_x = d_x / mag;
								d_y = d_y / mag;
								Point2f dir;
								dir.x = d_x;
								dir.y = d_y;
								chains[i].direction = dir;
								chains[j].merged = true;
								chains[i].flag = true;
								merges++;
								int z = j;
								for (j = 0; j < chains.size(); j++)
								{
									if ((chains[j].p.index == chains[z].q.index || chains[j].q.index == chains[z].q.index) && !chains[j].merged)
									{
										chains[j].merged = true;
									}
								}
								j = 0;
							}
							else
							{
								if (chains[i].flag == true) chains[j].merged = true;
								//chains[j].merged = true;
							}
						}
						else if (chains[i].q.index == chains[j].p.index)
						{
							if ((acos(chains[i].direction.x*chains[j].direction.x + chains[i].direction.y *chains[j].direction.y) < strictness))
							{
								cout << "情况3：右与左重合：" << endl;
								cout << "chains[" << i << "].p.index=" << chains[i].p.index << "," << "chains[" << i << "].q.index=" << chains[i].q.index << ";" << "chains[" << j << "].p.index=" <<
									chains[j].p.index << "," << "chains[" << j << "].q.index=" << chains[j].q.index << endl;
								cout << "i=" << i << "," << "j=" << j << ";" << "chains[i].direction.x=" << chains[i].direction.x << "," << "chains[j].direction.x=" <<
									chains[j].direction.x << endl;
								cout << "值=" << chains[i].direction.x*chains[j].direction.x + chains[i].direction.y * chains[j].direction.y << endl;
								cout << "角度=" << acos(chains[i].direction.x*chains[j].direction.x + chains[i].direction.y * chains[j].direction.y) << endl;
								chains[i].q = chains[j].q;
								for (std::vector<Candidate>::iterator it = chains[j].components.begin(); it != chains[j].components.end(); it++)
								{
									chains[i].components.push_back(*it);
								}
								float d_x = chains[i].q.middle_i - chains[i].p.middle_i;
								float d_y = chains[i].q.middle_j - chains[i].p.middle_j;
								float mag = sqrt(d_x*d_x + d_y*d_y);
								chains[i].dist = d_x*d_x + d_y*d_y;
								d_x = d_x / mag;
								d_y = d_y / mag;
								Point2f dir;
								dir.x = d_x;
								dir.y = d_y;
								chains[i].direction = dir;
								chains[j].merged = true;
								chains[i].flag = true;
								merges++;
								int z = j;
								for (j = 0; j < chains.size(); j++)
								{

									if ((chains[j].p.index == chains[z].p.index || chains[j].q.index == chains[z].p.index) && !chains[j].merged)
									{
										chains[j].merged = true;
									}
								}
								j = 0;
							}
							else
							{
								if (chains[i].flag == true) chains[j].merged = true;
								//chains[j].merged = true;
							}
						}
						else if (chains[i].q.index == chains[j].q.index)
						{
							if ((acos(chains[i].direction.x*-chains[j].direction.x + chains[i].direction.y*-chains[j].direction.y) < strictness))
							{
								std::cout << "情况4：右边与右边重合：" << endl;
								std::cout << "chains[" << i << "].p.index=" << chains[i].p.index << "," << "chains[" << i << "].q.index=" << chains[i].q.index << ";" << "chains[" << j << "].p.index=" << chains[j].p.index <<
									"," << "chains[" << j << "].q.index=" << chains[j].q.index << endl;
								std::cout << "i=" << i << "," << "j=" << j << ";" << "chains[i].direction.x=" << chains[i].direction.x << "," << "chains[j].direction.x=" <<
									chains[j].direction.x << endl;
								std::cout << "值=" << chains[i].direction.x*-chains[j].direction.x + chains[i].direction.y*-chains[j].direction.y << endl;
								std::cout << "角度=" << acos(chains[i].direction.x*-chains[j].direction.x + chains[i].direction.y*-chains[j].direction.y) << endl;
								chains[i].q = chains[j].p;
								for (std::vector<Candidate>::iterator it = chains[j].components.begin(); it != chains[j].components.end(); it++)
								{
									chains[i].components.push_back(*it);
								}
								float d_x = chains[i].q.middle_i - chains[i].p.middle_i;
								float d_y = chains[i].q.middle_j - chains[i].p.middle_j;
								float mag = sqrt(d_x*d_x + d_y*d_y);
								chains[i].dist = d_x*d_x + d_y*d_y;
								d_x = d_x / mag;
								d_y = d_y / mag;
								Point2f dir;
								dir.x = d_x;
								dir.y = d_y;
								chains[i].direction = dir;
								chains[j].merged = true;
								chains[i].flag = true;
								merges++;
								int z = j;
								for (j = 0; j < chains.size(); j++)
								{
									if ((chains[j].p.index == chains[z].q.index || chains[j].q.index == chains[z].q.index) && !chains[j].merged)
									{
										chains[j].merged = true;
									}
								}
								j = 0;
							}
							else
							{
								if (chains[i].flag == true) chains[j].merged = true;
								//chains[j].merged = true;
							}
						}
					}
				}
			}
		}
		for (unsigned int i = 0; i < chains.size(); i++)
		{
			if (!chains[i].merged)
			{
				newchains.push_back(chains[i]);
			}
		}
		chains = newchains;
		std::stable_sort(chains.begin(), chains.end(),&chainSortLength);
	}
	std::vector<Chain> newchains;
	newchains.reserve(chains.size());
	for (std::vector<Chain>::iterator cit = chains.begin(); cit != chains.end(); cit++)
	{
		if (cit->components.size() >2)
			newchains.push_back(*cit);
	}
	chains = newchains;
	std::cout << chains.size() << "chains after merging" << endl;
	for (unsigned int i = 0; i < chains.size(); i++)
	{
		points.clear();
		for (std::vector<Candidate>::iterator it = chains[i].components.begin(); it != chains[i].components.end(); it++)
		{
			//cout << it->index << ","<endl;
			points.push_back(cvPoint(it->minx, it->miny));
			points.push_back(cvPoint(it->maxx, it->maxy));
			points.push_back(cvPoint(it->minx, it->miny + it->height));
			points.push_back(cvPoint(it->maxx, it->maxy - it->height));
		}
		pointStore.push_back(points);
	}
}

