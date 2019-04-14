#pragma once

#include "Bitmap.h"

namespace cg
{
	class Denoiser
	{
	public:
		Denoiser(int radius, double exponent);
		Bitmap<unsigned char> denoise(Bitmap<unsigned char> &bitmap, Bitmap<unsigned char> &normals, Bitmap<unsigned char> &distance);
	private:
		Vector<unsigned char, 3> pixel(Bitmap<unsigned char> &bitmap, Bitmap<unsigned char> &normals, Bitmap<unsigned char> &distance, unsigned int x, unsigned int y);

		int radius;
		double exponent;
	};
}