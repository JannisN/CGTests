#pragma once

#include <vector>

namespace cg
{
	template <class T>
	void insertBinaryData(std::vector<unsigned char> &data, T t, int pos, int size);
	template <class T>
	void insertBinaryDataInverse(std::vector<unsigned char> &data, T t, int pos, int size);
	template <class T>
	void insertBinaryData(std::vector<unsigned char> &data, T t, int pos);
	template <class T>
	void insertBinaryDataInverse(std::vector<unsigned char> &data, T t, int pos);

	std::vector<std::string> split(const std::string& s, char delim);

	// impl ---------------------------------

	template<class T>
	void insertBinaryData(std::vector<unsigned char>& data, T t, int pos, int size)
	{
		if (pos + size > data.size())
			data.resize(pos + size);
		for (int i = 0; i < size; i++)
		{
			data[pos + i] = *(reinterpret_cast<unsigned char*>(&t) + size - i - 1);
		}
	}

	template<class T>
	void insertBinaryDataInverse(std::vector<unsigned char>& data, T t, int pos, int size)
	{
		if (pos + size > data.size())
			data.resize(pos + size);
		for (int i = 0; i < size; i++)
		{
			data[pos + i] = *(reinterpret_cast<unsigned char*>(&t) + i);
		}
	}

	template<class T>
	void insertBinaryData(std::vector<unsigned char>& data, T t, int pos)
	{
		insertBinaryData(data, t, pos, sizeof(pos));
	}

	template<class T>
	void insertBinaryDataInverse(std::vector<unsigned char>& data, T t, int pos)
	{
		insertBinaryDataInverse(data, t, pos, sizeof(pos));
	}
}