#pragma once

#include "Matrix.h"
#include "Utils.h"

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
		void set(unsigned int x, unsigned int y, std::vector<T> color);
        std::vector<T> get(unsigned int x, unsigned int y);
        void fill(std::vector<T> color);

		void renderTriangle(Vector<int, 2> a, Vector<int, 2> b, Vector<int, 2> c, std::vector<T> values);
		void saveAsPPM(std::string filename);
		void saveAsBMP(std::string filename);
	private:
		unsigned int width, height;
		unsigned int layerCount;
		std::vector<T> data;
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
	}

	template<class T>
	inline Bitmap<T>& Bitmap<T>::operator = (Bitmap<T> bitmap)
	{
		auto[w, h] = bitmap.getSize();
		width = w;
		height = h;
		layerCount = bitmap.getLayerCount();
		data = bitmap.data;
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
	}

	template<class T>
	inline T& Bitmap<T>::operator () (unsigned int x, unsigned int y, unsigned int layer)
	{
		if (width <= x || height <= y || layerCount <= layer)
			throw std::runtime_error("Coordinates out of bound");
		return data[(x + width * y) * layerCount + layer];
	}

	template<class T>
	inline T* Bitmap<T>::getData()
	{
		return data.data();
	}

    template<class T>
    void Bitmap<T>::set(unsigned int x, unsigned int y, std::vector<T> color)
    {
        if (color.size() != layerCount)
            throw std::runtime_error("Color size doesn't match layer count");
        int index = (y * width + x) * layerCount;
        for (int i = 0; i < layerCount; i++)
        {
            data[i + index] = color[i];
        }
    }

    template<class T>
    std::vector<T> Bitmap<T>::get(unsigned int x, unsigned int y)
    {
        std::vector<T> color;
        int index = (y * width + x) * layerCount;
        for (int i = 0; i < layerCount; i++)
        {
            color[i] = data[i + index];
        }
    }

    template<class T>
    void Bitmap<T>::fill(std::vector<T> color)
    {
        if (color.size() != layerCount)
            throw std::runtime_error("Color size doesn't match layer count");
        for (long i = 0; i < width * height; i++)
        {
            for (int j = 0; j < layerCount; j++)
            {
                data[i * layerCount + j] = color[j];
            }
        }
    }

	template<class T>
	inline void Bitmap<T>::renderTriangle(Vector<int, 2> a, Vector<int, 2> b, Vector<int, 2> c, std::vector<T> values)
	{
		if (getLayerCount() != values.size())
			throw std::runtime_error("Values size doesn't match bitmap size");
		auto[width, height] = getSize();
		std::vector<std::tuple<int, int>> points;
		points.push_back({ a[0], a[1] });
		points.push_back({ b[0], b[1] });
		points.push_back({ c[0], c[1] });
		std::sort(points.begin(), points.end(), [](const std::tuple<int, int>& a, const std::tuple<int, int>& b) { return std::get<1>(a) < std::get<1>(b); });
		double coef;
		if (std::get<1>(points[0]) - std::get<1>(points[2]) != 0)
			coef = (std::get<1>(points[1]) - std::get<1>(points[0])) / (double)(std::get<1>(points[0]) - std::get<1>(points[2]));
		std::tuple<int, int> middle = { coef * (std::get<0>(points[2]) - std::get<0>(points[0])) + std::get<0>(points[2]), std::get<1>(points[1]) };
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
	inline void Bitmap<T>::saveAsPPM(std::string filename)
	{
		std::ofstream file;
		file.open(filename, std::ios::binary);
		file << "P3\n";
		file << width << " " << height << "\n";
		file << std::pow(2, sizeof(T) * 8) << "\n";
		for (auto e : data)
		{
			file << e << " ";
		}
	}

	template<class T>
	inline void Bitmap<T>::saveAsBMP(std::string filename)
	{
		std::vector<unsigned char> fileHeader(54);
		fileHeader[0] = 'B';
		fileHeader[1] = 'M';
		//insertBinaryData(fileHeader, totalSize, 2); // total file size, calculated later
		insertBinaryDataInverse(fileHeader, 0, 6); // application specific
		insertBinaryDataInverse(fileHeader, fileHeader.size(), 10, 4); // total header size

		insertBinaryDataInverse(fileHeader, 40, 14); // size of infoHeader
		insertBinaryDataInverse(fileHeader, width, 18); // width
		insertBinaryDataInverse(fileHeader, -height, 22); // height, negative because bmp...
		insertBinaryDataInverse(fileHeader, 1, 26, 2); // unused
		insertBinaryDataInverse(fileHeader, sizeof(T) * 8 * layerCount, 28, 2); // color depth, 1, 4, 8, 16, 24 or 32
		insertBinaryDataInverse(fileHeader, 0, 30); // compression
		insertBinaryDataInverse(fileHeader, 0, 34); // size of image or 0(if compression = 0)
		insertBinaryDataInverse(fileHeader, 0, 38); // pixel per meter, width
		insertBinaryDataInverse(fileHeader, 0, 42); // pixel per meter, height
		insertBinaryDataInverse(fileHeader, 0, 46); // no color table
		insertBinaryDataInverse(fileHeader, 0, 50); // no color table

		std::vector<unsigned char> imageSource;
		int zeroBytes = (4 - (width * layerCount * sizeof(T)) % 4) % 4;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int k = layerCount - 1; k >= 0; k--)
				{
					imageSource.push_back((*this)(j, i, k));
				}
			}
			for (int j = 0; j < zeroBytes; j++)
			{
				imageSource.push_back(0);
			}
		}

		insertBinaryDataInverse(fileHeader, fileHeader.size() + imageSource.size(), 2, 4); // total file size

		std::vector<unsigned char> totalImage = fileHeader;
		totalImage.insert(totalImage.end(), imageSource.begin(), imageSource.end());

		std::ofstream file;
		file.open(filename, std::ios::binary);
		file.write(reinterpret_cast<const char*>(&totalImage[0]), totalImage.size() * sizeof(T));
	}
}