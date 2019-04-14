#include "Filters.h"

namespace cg
{
	Denoiser::Denoiser(int radius, double exponent) : radius(radius), exponent(exponent)
	{
	}

	Vector<unsigned char, 3> Denoiser::pixel(Bitmap<unsigned char> &bitmap, Bitmap<unsigned char> &normals, Bitmap<unsigned char> &distance, unsigned int x, unsigned int y)
	{
		double sumRed = 0;
		double sumGreen = 0;
		double sumBlue = 0;
		double sumWeights = 0;

		auto [w, h] = bitmap.getSize();

		int left = std::max((int)x - radius, 0);
		int right = std::min((int)x + radius, (int)w - 1);
		int top = std::max((int)y - radius, 0);
		int bottom = std::min((int)y + radius, (int)h - 1);

		//if (x % 100 == 0 && y % 100 == 0)
		//{
		//	std::cout << x << " " << y << std::endl;
		//	std::cout << left << " " << right << std::endl;
		//	std::cout << top << " " << bottom << std::endl << std::endl;
		//}

		for (int i = left; i <= right; i++)
		{
			for (int j = top; j <= bottom; j++)
			{
				if (std::abs(normals(i, j, 0) - normals(x, y, 0)) < 10)
				{
					if (std::abs(normals(i, j, 1) - normals(x, y, 1)) < 10)
					{
						if (std::abs(normals(i, j, 2) - normals(x, y, 2)) < 10)
						{
							if (std::abs(distance(i, j, 0) - distance(x, y, 0)) < 2)
							{
								//std::cout << i << " " << j << std::endl << std::endl;
								double weight = std::exp(-exponent * ((x - i) * (x - i) + (y - j) * (y - j)));
								sumWeights += weight;
								sumRed += weight * bitmap(i, j, 0);
								sumGreen += weight * bitmap(i, j, 1);
								sumBlue += weight * bitmap(i, j, 2);
							}
						}
					}
				}
			}
		}

		return { sumRed / sumWeights, sumGreen / sumWeights, sumBlue / sumWeights };
		//return { sumRed / sumWeights, 100 * sumWeights, 100 * sumWeights };
	}

	Bitmap<unsigned char> Denoiser::denoise(Bitmap<unsigned char> &bitmap, Bitmap<unsigned char> &normals, Bitmap<unsigned char> &distance)
	{
		auto [w, h] = bitmap.getSize();
		Bitmap<unsigned char> denoised(w, h, 3);

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				auto color = pixel(bitmap, normals, distance, i, j);
				denoised(i, j, 0) = color(0);
				denoised(i, j, 1) = color(1);
				denoised(i, j, 2) = color(2);
			}
		}

		return denoised;
	}
}