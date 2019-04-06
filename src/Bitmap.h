#pragma once

#include "Matrix.h"

#include <vector>
#include <tuple>
#include <fstream>

namespace cg
{
	template <class T>
	class Bitmap
	{
	public:
		Bitmap();
		Bitmap(unsigned int width, unsigned int height, unsigned int layerCount);

		Bitmap<T>& operator = (Bitmap<T> bitmap);

		std::tuple<unsigned int, unsigned int> getSize();
		void setSize(unsigned int width, unsigned int height);
		unsigned long getTotalSize();
		unsigned int getLayerCount();
		void setLayerCount(unsigned int count);
		T& operator () (unsigned int x, unsigned int y, unsigned int layer);
		T* getData();

		void renderTriangle(Vector<int, 2> a, Vector<int, 2> b, Vector<int, 2> c, std::vector<T> values);
		void saveToPPM(std::string filename);
	private:
		unsigned int width, height;
		unsigned int layerCount;
		std::vector<T> data;
		T* ptr;
	};

	template<class T>
	inline Bitmap<T>::Bitmap() : Bitmap(0, 0, 0)
	{
	}

	template<class T>
	inline Bitmap<T>::Bitmap(unsigned int width, unsigned int height, unsigned int layerCount)
	{
		this->width = width;
		this->height = height;
		this->layerCount = layerCount;
		data.resize(width * height * layerCount);
		ptr = data.data();
	}

	template<class T>
	inline Bitmap<T>& Bitmap<T>::operator = (Bitmap<T> bitmap)
	{
		auto[w, h] = bitmap.getSize();
		width = w;
		height = h;
		layerCount = bitmap.getLayerCount();
		data = bitmap.data;
		ptr = data.data();
		return *this;
	}

	template<class T>
	inline std::tuple<unsigned int, unsigned int> Bitmap<T>::getSize()
	{
		return { width, height };
	}

	template<class T>
	inline void Bitmap<T>::setSize(unsigned int width, unsigned int height)
	{
		this->width = width;
		this->height = height;
		data.resize(width * height * layerCount);
		ptr = data.data();
	}

	template<class T>
	inline unsigned long Bitmap<T>::getTotalSize()
	{
		return static_cast<unsigned long>(width) * height * layerCount;
	}

	template<class T>
	inline unsigned int Bitmap<T>::getLayerCount()
	{
		return layerCount;
	}

	template<class T>
	inline void Bitmap<T>::setLayerCount(unsigned int count)
	{
		layerCount = count;
		data.resize(width * height * layerCount);
		ptr = data.data();
	}

	template<class T>
	inline T& Bitmap<T>::operator () (unsigned int x, unsigned int y, unsigned int layer)
	{
		if (width <= x || height <= y || layerCount <= layer)
			throw std::exception("Coordinates out of bound");
		return ptr[(x + width * y) * layerCount + layer];
	}

	template<class T>
	inline T* Bitmap<T>::getData()
	{
		return ptr;
	}

	template<class T>
	inline void Bitmap<T>::renderTriangle(Vector<int, 2> a, Vector<int, 2> b, Vector<int, 2> c, std::vector<T> values)
	{
		if (getLayerCount() != values.size())
			throw std::exception("Values size doesn't match bitmap size");
		auto[width, height] = getSize();
		std::vector<std::tuple<int, int>> points;
		points.push_back(std::tuple(a[0], a[1]));
		points.push_back(std::tuple(b[0], b[1]));
		points.push_back(std::tuple(c[0], c[1]));
		std::sort(points.begin(), points.end(), [](const std::tuple<int, int>& a, const std::tuple<int, int>& b) { return std::get<1>(a) < std::get<1>(b); });
		double coef;
		if (std::get<1>(points[0]) - std::get<1>(points[2]) != 0)
			coef = (std::get<1>(points[1]) - std::get<1>(points[0])) / (double)(std::get<1>(points[0]) - std::get<1>(points[2]));
		std::tuple<int, int> middle = std::tuple(coef * (std::get<0>(points[2]) - std::get<0>(points[0])) + std::get<0>(points[2]), std::get<1>(points[1]));
		for (int i = (std::max)(std::get<1>(points[0]), 0); i <= (std::min)(std::get<1>(points[1]), (int)height); i++)
		{
			int widthCriterion;
			if ((std::get<1>(middle) - std::get<1>(points[0])) != 0)
				widthCriterion = std::get<0>(points[0]) + (std::get<0>(middle) - std::get<0>(points[0])) * (i - std::get<1>(points[0])) / (double)(std::get<1>(middle) - std::get<1>(points[0]));
			else
				widthCriterion = std::get<0>(points[0]) + (std::get<0>(middle) - std::get<0>(points[0]));
			int add = std::get<0>(points[1]) < std::get<0>(middle) ? 1 : -1;
			int j;
			if ((std::get<1>(points[1]) - std::get<1>(points[0])) != 0)
				j = std::get<0>(points[0]) + (std::get<0>(points[1]) - std::get<0>(points[0])) * (i - std::get<1>(points[0])) / (double)(std::get<1>(points[1]) - std::get<1>(points[0]));
			else
				j = std::get<0>(points[0]) + (std::get<0>(points[1]) - std::get<0>(points[0]));

			if (j < 0)
				j = 0;
			else if (j > width)
				j = width;
			if (widthCriterion < 0)
				widthCriterion = 0;
			else if (widthCriterion > width)
				widthCriterion = width;

			for (; j * add <= widthCriterion * add; j += add)
			{
				if (i < height && i >= 0 && j < width && j >= 0)
				{
					for (int k = 0; k < getLayerCount(); k++)
					{
						getData()[(i * std::get<0>(getSize()) + j) * getLayerCount() + k] = values[k];
					}
				}
			}
		}
		for (int i = (std::min)(std::get<1>(points[2]), (int)height); i >= (std::max)(std::get<1>(points[1]), 0); i--)
		{
			int widthCriterion;
			if ((std::get<1>(middle) - std::get<1>(points[2])) != 0)
				widthCriterion = std::get<0>(points[2]) + (std::get<0>(middle) - std::get<0>(points[2])) * (i - std::get<1>(points[2])) / (double)(std::get<1>(middle) - std::get<1>(points[2]));
			else widthCriterion = std::get<0>(points[2]) + (std::get<0>(middle) - std::get<0>(points[2]));
			int add = std::get<0>(points[1]) < std::get<0>(middle) ? 1 : -1;
			int j;
			if ((std::get<1>(points[1]) - std::get<1>(points[2])) != 0)
				j = std::get<0>(points[2]) + (std::get<0>(points[1]) - std::get<0>(points[2])) * (i - std::get<1>(points[2])) / (double)(std::get<1>(points[1]) - std::get<1>(points[2]));
			else
				j = std::get<0>(points[2]) + (std::get<0>(points[1]) - std::get<0>(points[2]));

			if (j < 0)
				j = 0;
			else if (j > width)
				j = width;
			if (widthCriterion < 0)
				widthCriterion = 0;
			else if (widthCriterion > width)
				widthCriterion = width;

			for (; j * add <= widthCriterion * add; j += add)
			{
				if (i < height && i >= 0 && j < width && j >= 0)
				{
					for (int k = 0; k < getLayerCount(); k++)
					{
						getData()[(i * std::get<0>(getSize()) + j) * getLayerCount() + k] = values[k];
					}
				}
			}
		}
	}

	template<class T>
	inline void Bitmap<T>::saveToPPM(std::string filename)
	{
		std::ofstream file;
		file.open(filename);
		file << "P3\n";
		file << width << " " << height << "\n";
		file << std::pow(2, sizeof(T) * 8) << "\n";
		for (auto e : data)
		{
			file << std::to_string(e) << " ";
		}
	}
}