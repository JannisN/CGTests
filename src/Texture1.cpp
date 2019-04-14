#include "Matrix.h"
#include "Bitmap.h"

#include <random>

using namespace cg;

double metric(Vector<double, 2> v1, Vector<double, 2> v2)
{
	Vector<double, 2> v3 = v1 - v2;
	return std::sqrt(v3 * v3);
}

double smooth(double start, double end, double value)
{
	if (value < start)
		return 0;
	if (value > end)
		return 1;
	return (value - start) / (end - start);
}

double posSin(double value)
{
	return 0.5 * (std::sin(value) + 1);
}

double sineSum1(double value)
{
	return std::sin(value * 4) * 0.5 + 0.5;
}

double sineSum2(double value)
{
	return std::cos(value * 3 + 14) * 0.5 + 0.5;
}

int main()
{
	Bitmap<unsigned char> bitmap(1000, 1000, 3);

	std::uniform_real_distribution<double> random(0, 1.0);
	std::default_random_engine generator;

	std::vector<Vector<double, 2>> points;
	for (int i = 0; i < 20; i++)
	{
		points.push_back({ random(generator), random(generator) });
	}

	for (int i = 0; i < 1000; i++)
	{
		for (int j = 0; j < 1000; j++)
		{
			Vector<double, 2> pos = { i / 1000.0, j / 1000.0 };
			double minDistance1 = 1;
			for (int k = 0; k < 20; k++)
			{
				double dist = metric(pos, points[k]);
				if (dist < minDistance1)
					minDistance1 = dist;
			}
			double minDistance2 = 1;
			double minDistance3 = 1;
			for (int k = 0; k < 20; k++)
			{
				double dist = metric(pos, points[k]);
				if (dist < minDistance2)
				{
					minDistance3 = minDistance2;
					minDistance2 = dist;
				}
				else if (dist < minDistance3)
				{
					minDistance3 = dist;
				}
			}

			double strength = 0.0 + smooth(0, 0.3, minDistance3) - smooth(0, 0.3, minDistance2);

			double sin = sineSum1(pos(0)) + sineSum2(pos(1));
			sin /= 2.0;

			Vector<double, 3> color;

			//sin = sin * 0.3;
			//if (sin > strength)
			//{
			//	color = { 180, 120, 80 };
			//}
			//else
			{
				color = { 180 * strength, 120 * strength, 80 * strength };
			}

			bitmap(i, j, 0) = color(0);
			bitmap(i, j, 1) = color(1);
			bitmap(i, j, 2) = color(2);
		}
	}

	bitmap.saveAsBMP("texture1.bmp");

	return 0;
}