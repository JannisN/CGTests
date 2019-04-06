#pragma once

#include <vector>
#include <array>
#include <complex>

namespace cg
{
	template <class T>
	class DynamicMatrix
	{
	public:
		static DynamicMatrix<T> Identity(unsigned int size);

		DynamicMatrix();
		DynamicMatrix(unsigned int height, unsigned int width);
		DynamicMatrix(std::tuple<unsigned int, unsigned int> size);
		DynamicMatrix(unsigned int height, unsigned int width, std::vector<T> data);
		DynamicMatrix(std::tuple<unsigned int, unsigned int> size, std::vector<T> data);
		DynamicMatrix(std::initializer_list<std::initializer_list<T>> data);
		DynamicMatrix(std::vector<std::vector<T>> data);
		DynamicMatrix(std::vector<T> data);
		template <class F>
		DynamicMatrix(DynamicMatrix<F> m);

		T& operator [] (unsigned int i);
		T& operator () (unsigned int i);
		T& operator () (unsigned int i, unsigned int j);
		DynamicMatrix<T>& operator = (std::initializer_list<std::initializer_list<T>> data);
		DynamicMatrix<T>& operator = (std::vector<std::vector<T>> data);
		DynamicMatrix<T>& operator = (std::vector<T> data);

		operator T() const;
		operator T*() const;
		operator std::vector<T>() const;

		void setSize(unsigned int height, unsigned int width);
		std::tuple<unsigned int, unsigned int> getSize();
		unsigned int getTotalSize();

		DynamicMatrix<T> operator - ();

		template <class F>
		DynamicMatrix<T> operator + (DynamicMatrix<F> m);
		template <class F>
		DynamicMatrix<T> operator - (DynamicMatrix<F> m);
		template <class F>
		DynamicMatrix<T> operator * (DynamicMatrix<F> m);
		template <class F>
		DynamicMatrix<T>& operator = (DynamicMatrix<F> m);
		template <class F>
		DynamicMatrix<T>& operator += (DynamicMatrix<F> m);
		template <class F>
		DynamicMatrix<T>& operator -= (DynamicMatrix<F> m);
		template <class F>
		DynamicMatrix<T>& operator *= (DynamicMatrix<F> m);

		DynamicMatrix<T> operator + (T f);
		DynamicMatrix<T> operator - (T f);
		DynamicMatrix<T> operator * (T f);
		DynamicMatrix<T> operator / (T f);
		DynamicMatrix<T>& operator += (T f);
		DynamicMatrix<T>& operator -= (T f);
		DynamicMatrix<T>& operator *= (T f);
		DynamicMatrix<T>& operator /= (T f);

		DynamicMatrix<T> cut(unsigned int h, unsigned int w);
		DynamicMatrix<T> transpose();
		DynamicMatrix<T> adjoint();
	protected:
		unsigned int height, width;
		std::vector<T> content;
	};

	template <class T>
	DynamicMatrix<T> operator + (T f, DynamicMatrix<T> m);
	template <class T>
	DynamicMatrix<T> operator - (T f, DynamicMatrix<T> m);
	template <class T>
	DynamicMatrix<T> operator * (T f, DynamicMatrix<T> m);

	template <class T, class ... F>
	DynamicMatrix<T> vector(F... f);

	template <class T>
	DynamicMatrix<T> transpose(DynamicMatrix<T> matrix);
	template <class T>
	DynamicMatrix<T> adjoint(DynamicMatrix<T> matrix);

	template <class T, unsigned int H, unsigned int W = H>
	class Matrix
	{
	public:
		static constexpr Matrix<T, H, H> Identity();

		Matrix();
		Matrix(std::initializer_list<std::initializer_list<T>> data);
		Matrix(std::vector<std::vector<T>> data);
		Matrix(std::vector<T> data);
		template <class ... F>
		Matrix(F... f);
		template <class F>
		Matrix(DynamicMatrix<F> m);

		T& operator [] (unsigned int i);
		T& operator () (unsigned int i);
		T& operator () (unsigned int i, unsigned int j);
		template <class F>
		Matrix<T, H, W>& operator = (DynamicMatrix<F> m);

		operator T*() const;
		operator std::vector<T>() const;
		operator DynamicMatrix<T>() const;

		std::tuple<unsigned int, unsigned int> constexpr getSize();
		unsigned int constexpr getTotalSize();

		Matrix<T, H, W> operator - ();

		template <class F>
		Matrix<T, H, W> operator + (Matrix<F, H, W> m);
		template <class F>
		Matrix<T, H, W> operator - (Matrix<F, H, W> m);
		template <class F, unsigned int N>
		Matrix<T, H, N> operator * (Matrix<F, W, N> m);
		template <class F, typename = std::enable_if<W == 1>>
		T operator * (Matrix<F, H, 1> m)
		{
			T t = (*this)[0] * m[0];
			for (int i = 1; i < H; i++)
				t += (*this)[i] * m[i];
			return t;
		};
		template <class F>
		Matrix<T, H, W>& operator = (Matrix<F, H, W> m);
		template <class F>
		Matrix<T, H, W>& operator += (Matrix<F, H, W> m);
		template <class F>
		Matrix<T, H, W>& operator -= (Matrix<F, H, W> m);
		template <class F, typename = std::enable_if<W == H>>
		Matrix<T, H, W>& operator *= (Matrix<F, H, W> m)
		{
			*this = *this * m;
			return *this;
		};

		Matrix<T, H, W> operator + (T f);
		Matrix<T, H, W> operator - (T f);
		Matrix<T, H, W> operator * (T f);
		Matrix<T, H, W> operator / (T f);
		Matrix<T, H, W>& operator += (T f);
		Matrix<T, H, W>& operator -= (T f);
		Matrix<T, H, W>& operator *= (T f);
		Matrix<T, H, W>& operator /= (T f);

		template <unsigned int height, unsigned int width>
		Matrix<T, height, width> cut();
		Matrix<T, W, H> transpose();
		Matrix<T, W, H> adjoint();
	protected:
		std::array<T, H * W> content;
	};

	template <class T, unsigned int H, unsigned int W>
	Matrix<T, H, W> operator + (T f, Matrix<T, H, W> m);
	template <class T, unsigned int H, unsigned int W>
	Matrix<T, H, W> operator - (T f, Matrix<T, H, W> m);
	template <class T, unsigned int H, unsigned int W>
	Matrix<T, H, W> operator * (T f, Matrix<T, H, W> m);

	template <class T, unsigned int N>
	using Vector = Matrix<T, N, 1>;

	template <class T, unsigned int H, unsigned int W>
	Matrix<T, W, H> transpose(Matrix<T, H, W> m);
	template <class T, unsigned int H, unsigned int W>
	Matrix<T, W, H> adjoint(Matrix<T, H, W> m);

	// impl ----------------------------

	template<class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::Identity(unsigned int size)
	{
		DynamicMatrix<T> matrix(size, size);
		for (int i = 0; i < size; i++)
			matrix(i, i) = 1;
		return matrix;
	}

	template <class T>
	inline DynamicMatrix<T>::DynamicMatrix()
	{
		this->height = 1;
		this->width = 1;
		content.resize(1);
		std::fill(content.begin(), content.end(), T(0));
	}

	template <class T>
	inline DynamicMatrix<T>::DynamicMatrix(unsigned int height, unsigned int width)
	{
		this->height = height;
		this->width = width;
		content.resize(height * width);
		std::fill(content.begin(), content.end(), T(0));
	}

	template<class T>
	inline DynamicMatrix<T>::DynamicMatrix(std::tuple<unsigned int, unsigned int> size)
	{
		auto[y, x] = size;
		this->height = y;
		this->width = x;
		content.resize(y * x);
		std::fill(content.begin(), content.end(), T(0));
	}

	template<class T>
	inline DynamicMatrix<T>::DynamicMatrix(unsigned int height, unsigned int width, std::vector<T> data) : DynamicMatrix(height, width)
	{
		if (data.size() != height * width)
			throw std::runtime_error("Data size doesn't match matrix size");
		else
			content = data;
	}

	template<class T>
	inline DynamicMatrix<T>::DynamicMatrix(std::tuple<unsigned int, unsigned int> size, std::vector<T> data) : DynamicMatrix(size)
	{
		if (data.size() != getTotalSize())
			throw std::runtime_error("Data size doesn't match matrix size");
		else
			content = data;
	}

	template <class T>
	inline DynamicMatrix<T>::DynamicMatrix(std::initializer_list<std::initializer_list<T>> data)
	{
		height = data.size();
		width = data.begin()->size();
		content.resize(height * width);
		unsigned int i = 0, j = 0;
		for (auto row : data)
		{
			for (auto element : row)
			{
				content[i * width + j] = element;
				j++;
			}
			j = 0;
			i++;
		}
	}

	template <class T>
	inline DynamicMatrix<T>::DynamicMatrix(std::vector<std::vector<T>> data)
	{
		height = data.size();
		width = data[0].size();
		content.resize(height * width);
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				content[i * width + j] = data[i][j];
			}
		}
	}

	template <class T>
	inline DynamicMatrix<T>::DynamicMatrix(std::vector<T> data)
	{
		height = data.size();
		width = 1;
		content.resize(height);
		for (unsigned int i = 0; i < height; i++)
		{
			content[i] = data[i];
		}
	}

	template <class T>
	template <class F>
	inline DynamicMatrix<T>::DynamicMatrix(DynamicMatrix<F> m)
	{
		auto[height, width] = m.getSize();
		content.resize(height * width);
		this->height = height;
		this->width = width;
		for (unsigned int i = 0; i < getTotalSize(); i++)
		{
			content[i] = static_cast<T>(m[i]);
		}
	}

	template <class T>
	inline T& DynamicMatrix<T>::operator [] (unsigned int i)
	{
		return content[i];
	}

	template<class T>
	inline T& DynamicMatrix<T>::operator () (unsigned int i)
	{
		return content[i];
	}

	template <class T>
	inline T& DynamicMatrix<T>::operator () (unsigned int i, unsigned int j)
	{
		if (i >= height || j >= width)
			throw std::domain_error("Matrix index out of range");
		return content[i * width + j];
	}

	template <class T>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator = (std::initializer_list<std::initializer_list<T>> data)
	{
		*this = DynamicMatrix<T>(data);
		return *this;
	}

	template<class T>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator = (std::vector<std::vector<T>> data)
	{
		*this = DynamicMatrix<T>(data);
		return *this;
	}

	template<class T>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator = (std::vector<T> data)
	{
		*this = DynamicMatrix<T>(data);
		return *this;
	}

	template <class T>
	inline void DynamicMatrix<T>::setSize(unsigned int height, unsigned int width)
	{
		this->height = height;
		this->width = width;
		content.resize(height * width);
	}

	template <class T>
	inline std::tuple<unsigned int, unsigned int> DynamicMatrix<T>::getSize()
	{
		return { height, width };
	}

	template <class T>
	inline unsigned int DynamicMatrix<T>::getTotalSize()
	{
		return height * width;
	}

	template<class T>
	inline DynamicMatrix<T>::operator T() const
	{
		return (*this)(0, 0);
	}

	template <class T>
	inline DynamicMatrix<T>::operator T*() const
	{
		return &content[0];
	}

	template<class T>
	inline DynamicMatrix<T>::operator std::vector<T>() const
	{
		return content;
	}

	template <class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::operator - ()
	{
		DynamicMatrix<T> matrix(height, width);
		for (unsigned int i = 0; i < getTotalSize(); i++)
		{
			matrix[i] = -content[i];
		}
		return matrix;
	}

	template <class T>
	template <class F>
	inline DynamicMatrix<T> DynamicMatrix<T>::operator + (DynamicMatrix<F> m)
	{
		if (getSize() != m.getSize())
			throw std::runtime_error("Trying to add matrices of different sizes: " + std::to_string(height) + "x" + std::to_string(width) + " * " + std::to_string(m.height) + "x" + std::to_string(m.width));
		DynamicMatrix<T> matrix(height, width);
		for (unsigned int i = 0; i < getTotalSize(); i++)
		{
			matrix[i] = content[i] + m[i];
		}
		return matrix;
	}

	template <class T>
	template <class F>
	inline DynamicMatrix<T> DynamicMatrix<T>::operator - (DynamicMatrix<F> m)
	{
		if (getSize() != m.getSize())
			throw std::runtime_error("Trying to subtract matrices of different sizes: " + std::to_string(height) + "x" + std::to_string(width) + " * " + std::to_string(m.height) + "x" + std::to_string(m.width));
		DynamicMatrix<T> matrix(height, width);
		for (unsigned int i = 0; i < getTotalSize(); i++)
		{
			matrix[i] = content[i] - m[i];
		}
		return matrix;
	}

	template <class T>
	template <class F>
	inline DynamicMatrix<T> DynamicMatrix<T>::operator * (DynamicMatrix<F> m)
	{
		auto[y, x] = m.getSize();
		if (width == y)
		{
			DynamicMatrix<T> matrix(height, x);
			for (unsigned int i = 0; i < matrix.height; i++)
			{
				for (unsigned int j = 0; j < matrix.width; j++)
				{
					matrix(i, j) = (*this)(i, 0) * m(0, j);
				}
			}
			for (unsigned int i = 0; i < matrix.height; i++)
			{
				for (unsigned int j = 0; j < matrix.width; j++)
				{
					for (unsigned int k = 1; k < width; k++)
					{
						matrix(i, j) += (*this)(i, k) * m(k, j);
					}
				}
			}
			return matrix;
		}
		if (height == y && width == 1 && x == 1)
		{
			DynamicMatrix<T> matrix(1, 1);
			matrix(0, 0) = (*this)(0, 0) * m(0, 0);

			for (unsigned int i = 1; i < height; i++)
			{
				matrix(0, 0) += (*this)(i, 0) * m(i, 0);
			}
			return matrix;
		}
		if (height == 1 && width == 1)
		{
			DynamicMatrix<T> matrix(y, x);
			for (unsigned int i = 0; i < matrix.height; i++)
			{
				for (unsigned int j = 0; j < matrix.width; j++)
				{
					matrix(i, j) = (*this)(0, 0) * m(i, j);
				}
			}
			return matrix;
		}
		if (y == 1 && x == 1)
		{
			DynamicMatrix<T> matrix(height, width);
			for (unsigned int i = 0; i < matrix.height; i++)
			{
				for (unsigned int j = 0; j < matrix.width; j++)
				{
					matrix(i, j) = (*this)(i, j) * m(0, 0);
				}
			}
			return matrix;
		}
		throw std::runtime_error("Trying to multiply matrices of incompatible sizes: " + std::to_string(height) + "x" + std::to_string(width) + " * " + std::to_string(y) + "x" + std::to_string(x));
	}

	template <class T>
	template <class F>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator = (DynamicMatrix<F> m)
	{
		auto[y, x] = m.getSize();
		height = y;
		width = x;
		content.resize(height * width);
		for (int i = 0; i < content.size(); i++)
		{
			content[i] = m[i];
		}
		return *this;
	}

	template <class T>
	template <class F>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator += (DynamicMatrix<F> m)
	{
		*this = *this + m;
		return *this;
	}

	template <class T>
	template <class F>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator -= (DynamicMatrix<F> m)
	{
		*this = *this - m;
		return *this;
	}

	template <class T>
	template <class F>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator *= (DynamicMatrix<F> m)
	{
		*this = *this * m;
		return *this;
	}

	template <class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::operator + (T f)
	{
		DynamicMatrix<T> matrix = *this;
		unsigned int min = (std::min)(height, width);
		for (unsigned int i = 0; i < min; i++)
		{
			matrix(i, i) += f;
		}
		return matrix;
	}

	template <class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::operator - (T f)
	{
		DynamicMatrix<T> matrix = *this;
		unsigned int min = (std::min)(height, width);
		for (unsigned int i = 0; i < min; i++)
		{
			matrix(i, i) -= f;
		}
		return matrix;
	}

	template <class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::operator * (T f)
	{
		DynamicMatrix<T> matrix(height, width);
		for (unsigned int i = 0; i < getTotalSize(); i++)
		{
			matrix[i] = content[i] * f;
		}
		return matrix;
	}

	template <class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::operator / (T f)
	{
		DynamicMatrix<T> matrix(height, width);
		for (unsigned int i = 0; i < getTotalSize(); i++)
		{
			matrix[i] = content[i] / f;
		}
		return matrix;
	}

	template <class T>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator += (T f)
	{
		*this = *this + f;
		return *this;
	}

	template <class T>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator -= (T f)
	{
		*this = *this - f;
		return *this;
	}

	template <class T>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator *= (T f)
	{
		*this = *this * f;
		return *this;
	}

	template <class T>
	inline DynamicMatrix<T>& DynamicMatrix<T>::operator /= (T f)
	{
		*this = *this / f;
		return *this;
	}

	template<class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::cut(unsigned int h, unsigned int w)
	{
		DynamicMatrix<T> matrix(h, w);
		unsigned int minHeight = (std::min)(height, h);
		unsigned int minWidth = (std::min)(width, w);
		for (int i = 0; i < minHeight; i++)
		{
			for (int j = 0; j < minWidth; j++)
			{
				matrix(i, j) = (*this)(i, j);
			}
		}
		return matrix;
	}

	template<class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::transpose()
	{
		return transpose(*this);
	}

	template<class T>
	inline DynamicMatrix<T> DynamicMatrix<T>::adjoint()
	{
		return adjoint(*this);
	}

	template<class T>
	DynamicMatrix<T> operator + (T f, DynamicMatrix<T> m)
	{
		return m + f;
	}

	template<class T>
	DynamicMatrix<T> operator - (T f, DynamicMatrix<T> m)
	{
		return -m + f;
	}

	template<class T>
	DynamicMatrix<T> operator * (T f, DynamicMatrix<T> m)
	{
		return m * f;
	}

	template <class T, class ... F>
	inline DynamicMatrix<T> vector(F... f)
	{
		T args[] = { static_cast<T>(std::forward<F>(f), f) ... };
		std::vector<T> data;
		data.resize(sizeof...(F));
		for (unsigned int i = 0; i < sizeof...(F); i++)
			data[i] = args[i];
		return DynamicMatrix<T>(data);
	}

	template<class T>
	inline DynamicMatrix<T> transpose(DynamicMatrix<T> m)
	{
		auto[height, width] = m.getSize();
		DynamicMatrix<T> matrix(width, height);
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				matrix(i, j) = m(j, i);
			}
		}
		return matrix;
	}

	template<class T>
	inline DynamicMatrix<T> adjoint(DynamicMatrix<T> m)
	{
		return transpose(m);
	}

	template<class T>
	inline DynamicMatrix<std::complex<T>> adjoint(DynamicMatrix<std::complex<T>> m)
	{
		auto[height, width] = m.getSize();
		DynamicMatrix<std::complex<T>> matrix(width, height);
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				matrix(i, j) = std::conj(m(j, i));
			}
		}
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	inline constexpr Matrix<T, H, H> Matrix<T, H, W>::Identity()
	{
		Matrix<T, H, H> matrix;
		for (int i = 0; i < H; i++)
			matrix(i, i) = 1;
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>::Matrix()
	{
		content.fill(T(0));
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>::Matrix(std::initializer_list<std::initializer_list<T>> data)
	{
		unsigned int i = 0, j = 0;
		for (auto row : data)
		{
			for (auto element : row)
			{
				content[i * W + j] = element;
				j++;
			}
			j = 0;
			i++;
		}
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>::Matrix(std::vector<std::vector<T>> data)
	{
		if (data.size() != H)
			throw std::runtime_error("Data size doesn't match matrix size");
		for (unsigned int i = 0; i < H; i++)
		{
			if (data[i].size() != W)
				throw std::runtime_error("Data size doesn't match matrix size");
			for (unsigned int j = 0; j < W; j++)
			{
				content[i * W + j] = data[i][j];
			}
		}
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>::Matrix(std::vector<T> data)
	{
		if (data.size() != H * W)
			throw std::runtime_error("Data size doesn't match matrix size");
		for (unsigned int i = 0; i < H * W; i++)
		{
			content[i] = data[i];
		}
	}

	template<class T, unsigned int H, unsigned int W>
	template<class ...F>
	inline Matrix<T, H, W>::Matrix(F ...f)
	{
		T args[] = { static_cast<T>(std::forward<F>(f), f) ... };
		static_assert(H * W == sizeof...(F), "Argument count doesn't match vector size");
		for (unsigned int i = 0; i < sizeof...(F); i++)
			content[i] = args[i];
	}

	template<class T, unsigned int H, unsigned int W>
	template<class F>
	inline Matrix<T, H, W>::Matrix(DynamicMatrix<F> m)
	{
		m = m.cut(H, W);
		for (int i = 0; i < H * W; i++)
		{
			content[i] = m[i];
		}
	}

	template<class T, unsigned int H, unsigned int W>
	inline T& Matrix<T, H, W>::operator [] (unsigned int i)
	{
		return content[i];
	}

	template<class T, unsigned int H, unsigned int W>
	inline T& Matrix<T, H, W>::operator () (unsigned int i)
	{
		return content[i];
	}

	template<class T, unsigned int H, unsigned int W>
	inline T& Matrix<T, H, W>::operator () (unsigned int i, unsigned int j)
	{
		return content[i * W + j];
	}

	template<class T, unsigned int H, unsigned int W>
	template <class F>
	inline Matrix<T, H, W>& Matrix<T, H, W>::operator = (DynamicMatrix<F> m)
	{
		auto[y, x] = m.getSize();
		if (y != H || x != W)
			throw std::runtime_error("Matrix sizes not compatible");
		else
		{
			for (int i = 0; i < H * W; i++)
			{
				content[i] = m[i];
			}
		}
		return *this;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>::operator T*() const
	{
		return &content[0];
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>::operator DynamicMatrix<T>() const
	{
		return DynamicMatrix<T>(H, W, std::vector<T>(content.begin(), content.end()));
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>::operator std::vector<T>() const
	{
		return std::vector<T>(content.begin(), content.end());
	}

	template<class T, unsigned int H, unsigned int W>
	inline constexpr std::tuple<unsigned int, unsigned int> Matrix<T, H, W>::getSize()
	{
		return std::tuple<unsigned int, unsigned int>(H, W);
	}

	template<class T, unsigned int H, unsigned int W>
	inline constexpr unsigned int Matrix<T, H, W>::getTotalSize()
	{
		return H * W;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W> Matrix<T, H, W>::operator - ()
	{
		Matrix<T, H, W> matrix;
		for (int i = 0; i < H * W; i++)
			matrix[i] = -content[i];
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	template<class F>
	inline Matrix<T, H, W> Matrix<T, H, W>::operator + (Matrix<F, H, W> m)
	{
		Matrix<T, H, W> matrix;
		for (int i = 0; i < H * W; i++)
			matrix[i] = content[i] + m[i];
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	template<class F>
	inline Matrix<T, H, W> Matrix<T, H, W>::operator - (Matrix<F, H, W> m)
	{
		Matrix<T, H, W> matrix;
		for (int i = 0; i < H * W; i++)
			matrix[i] = content[i] - m[i];
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	template<class F, unsigned int N>
	inline Matrix<T, H, N> Matrix<T, H, W>::operator * (Matrix<F, W, N> m)
	{
		Matrix<T, H, N> matrix;
		for (unsigned int i = 0; i < H; i++)
		{
			for (unsigned int j = 0; j < N; j++)
			{
				matrix(i, j) = (*this)(i, 0) * m(0, j);
			}
		}
		for (unsigned int i = 0; i < H; i++)
		{
			for (unsigned int j = 0; j < N; j++)
			{
				for (unsigned int k = 1; k < W; k++)
				{
					matrix(i, j) += (*this)(i, k) * m(k, j);
				}
			}
		}
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	template<class F>
	inline Matrix<T, H, W>& Matrix<T, H, W>::operator = (Matrix<F, H, W> m)
	{
		for (int i = 0; i < H * W; i++)
			content[i] = m[i];
		return *this;
	}

	template<class T, unsigned int H, unsigned int W>
	template<class F>
	inline Matrix<T, H, W>& Matrix<T, H, W>::operator += (Matrix<F, H, W> m)
	{
		for (int i = 0; i < H * W; i++)
			content[i] += m[i];
		return *this;
	}

	template<class T, unsigned int H, unsigned int W>
	template<class F>
	inline Matrix<T, H, W>& Matrix<T, H, W>::operator -= (Matrix<F, H, W> m)
	{
		for (int i = 0; i < H * W; i++)
			content[i] -= m[i];
		return *this;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W> Matrix<T, H, W>::operator + (T f)
	{
		Matrix<T, H, W> matrix = *this;
		for (int i = 0; i < (std::min)(H, W); i++)
		{
			matrix(i, i) += f;
		}
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W> Matrix<T, H, W>::operator - (T f)
	{
		Matrix<T, H, W> matrix = *this;
		for (int i = 0; i < (std::min)(H, W); i++)
		{
			matrix(i, i) -= f;
		}
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W> Matrix<T, H, W>::operator * (T f)
	{
		Matrix<T, H, W> matrix;
		for (int i = 0; i < H * W; i++)
			matrix[i] = content[i] * f;
		return matrix;
	}
	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W> Matrix<T, H, W>::operator / (T f)
	{
		Matrix<T, H, W> matrix;
		for (int i = 0; i < H * W; i++)
			matrix[i] = content[i] / f;
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>& Matrix<T, H, W>::operator += (T f)
	{
		*this = *this + f;
		return *this;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>& Matrix<T, H, W>::operator -= (T f)
	{
		*this = *this - f;
		return *this;
	}
	
	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>& Matrix<T, H, W>::operator *= (T f)
	{
		for (int i = 0; i < H * W; i++)
			content[i] *= f;
		return *this;
	}
	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, H, W>& Matrix<T, H, W>::operator /= (T f)
	{
		for (int i = 0; i < H * W; i++)
			content[i] /= f;
		return *this;
	}

	template<class T, unsigned int H, unsigned int W>
	template<unsigned int height, unsigned int width>
	inline Matrix<T, height, width> Matrix<T, H, W>::cut()
	{
		Matrix<T, height, width> matrix;
		unsigned int minHeight = (std::min)(height, H);
		unsigned int minWidth = (std::min)(width, W);
		for (int i = 0; i < minHeight; i++)
		{
			for (int j = 0; j < minWidth; j++)
			{
				matrix(i, j) = (*this)(i, j);
			}
		}
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, W, H> Matrix<T, H, W>::transpose()
	{
		return transpose(*this);
	}

	template<class T, unsigned int H, unsigned int W>
	inline Matrix<T, W, H> Matrix<T, H, W>::adjoint()
	{
		return adjoint(*this);
	}

	template<class T, unsigned int H, unsigned int W>
	Matrix<T, H, W> operator + (T f, Matrix<T, H, W> m)
	{
		return m + f;
	}

	template<class T, unsigned int H, unsigned int W>
	Matrix<T, H, W> operator - (T f, Matrix<T, H, W> m)
	{
		return -m + f;
	}

	template<class T, unsigned int H, unsigned int W>
	Matrix<T, H, W> operator * (T f, Matrix<T, H, W> m)
	{
		return m * f;
	}

	template<class T, unsigned int H, unsigned int W>
	Matrix<T, W, H> transpose(Matrix<T, H, W> m)
	{
		Matrix<std::complex<T>, W, H> matrix;
		for (int i = 0; i < W; i++)
		{
			for (int j = 0; j < H; j++)
			{
				matrix(i, j) = m(j, i);
			}
		}
		return matrix;
	}

	template<class T, unsigned int H, unsigned int W>
	Matrix<T, W, H> adjoint(Matrix<T, H, W> m)
	{
		return transpose(m);
	}

	template<class T, unsigned int H, unsigned int W>
	Matrix<std::complex<T>, W, H> adjoint(Matrix<std::complex<T>, H, W> m)
	{
		Matrix<std::complex<T>, W, H> matrix;
		for (int i = 0; i < W; i++)
		{
			for (int j = 0; j < H; j++)
			{
				matrix(i, j) = std::conj(m(j, i));
			}
		}
		return matrix;
	}
}