#include "Utils.h"

#include <sstream>
#include <cmath>

namespace cg
{
	std::vector<std::string> split(const std::string& s, char delim)
	{
		std::vector<std::string> result;
		std::stringstream ss(s);
		std::string item;

		while (std::getline(ss, item, delim))
		{
			result.push_back(item);
		}

		return result;
	}

	std::tuple<double, double, double> hsvToRgb(double h, double s, double v)
	{
		int hi = std::floor(h / 60);
		double f = h / 60.0 - hi;
		double p = v * (1 - s);
		double q = v * (1 - s * f);
		double t = v * (1 - s * (1 - f));
		switch (hi)
		{
		case 0:
		case 6:
			return { v, t, p };
		case 1:
			return { q, v, p };
		case 2:
			return { p, v, t };
		case 3:
			return { p, q, v };
		case 4:
			return { t, p, v };
		case 5:
			return { v, p, q };
		}
		return {};
	}
}
