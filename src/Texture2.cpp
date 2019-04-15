#include "Matrix.h"
#include "Bitmap.h"

using namespace cg;

namespace texture2
{
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

	double func(Vector<double, 2> pos)
	{
		return std::sin(pos(0) * 2 + 123) + std::sin(pos(1) * 6 + 42)  + std::sin(pos(0) * 5.31 + 70) + std::sin(pos(1) * 8.31 + 89) + 5;
	}

	double tex1(double i, double j)
	{
		Vector<double, 2> pos = { (i - 500) / 500.0 - 2, (j - 500) / 500.0 };

		return (texture2::func(pos) * 2 - std::floor(texture2::func(pos) * 2));
	}

	Vector<double, 3> tex2(double i, double j, double lineWidth, double height, double width, double scale)
	{
		i *= scale;
		j *= scale;

		double offset = std::fmod(std::floor(j / height), 2) * width / 2;
		double tex1Y = std::floor(j / height) * 75.73456 + j * 2;
		double tex1X = std::floor((i + offset) / width) * 7429.73456 + i * 0.97;
		double strength = tex1(tex1X, tex1Y);
		Vector<double, 3> color = { 216 / 255.0, 190 / 255.0, 162 / 255.0 };
		color *= (strength * 0.7 + 0.3);

		double border;
		if (std::fmod(j, height) > height - lineWidth)
			border = 0;
		else if (std::fmod(i + offset, width) > width - lineWidth)
			border = 0;
		else
			border = 1;
		color *= border;

		return color;
	}
}

int main()
{
	Bitmap<unsigned char> bitmap(1000, 1000, 3);

	for (int i = 0; i < 1000; i++)
	{
		for (int j = 0; j < 1000; j++)
		{
			auto color = texture2::tex2(i, j, 2, 60, 300, 0.4);
			bitmap(i, j, 0) = 255 * color(0);
			bitmap(i, j, 1) = 255 * color(1);
			bitmap(i, j, 2) = 255 * color(2);
		}
	}

	bitmap.saveAsBMP("texture2.bmp");

	return 0;
}