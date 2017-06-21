#ifndef _TOOLS_
#define _TOOLS_

#include<string>
#include<vector>

class tools
{
public:
	static std::vector<std::string> split(std::string str, std::string pattern);
	static double min(double a1, double a2){ double ret = (a1 < a2 ? a1 : a2); return ret; }
	static double max(double a1, double a2){ double ret = { a1>a2 ? a1 : a2 }; return ret; }

};

#endif