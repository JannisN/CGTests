// Copyright Jannis Naske, All rights reserved, do not use without permission

#pragma once

#include <vector>
#include <tuple>
#include <type_traits>
#include <stdexcept>

// in header
namespace CompileInfo
{
#ifndef NDEBUG
	const bool debug = true;
#else
	const bool debug = false;
#endif
}

namespace ct
{
	template<typename Permutations, typename T, std::size_t... U>
	struct IndexTensor;

	template <std::size_t... T>
	struct TemplateType;

	template <typename T, std::size_t... U>
	class Tensor
	{
	public:
		static constexpr Tensor<T, U...> Id();

		constexpr Tensor();

		constexpr std::vector<std::size_t> getSize();
		constexpr std::size_t getTotalSize();
		template <typename... V>
		constexpr T& operator () (V... v);
		template <std::size_t... Indices>
		constexpr T& get();
		constexpr T& get(std::array<std::size_t, sizeof...(U)> indices);
		T& operator [] (std::size_t index);
		template <std::size_t... V>
		constexpr Tensor<T, U..., V...> operator * (Tensor<T, V...>& tensor);
		template <std::size_t... Indices>
		constexpr IndexTensor<TemplateType<Indices...>, T, U...> id();
	private:
		std::array<T, (U * ...)> content;
	};

	template<typename T, std::size_t... U>
	constexpr Tensor<T, U...> Tensor<T, U...>::Id()
	{
		Tensor<T, U...> tensor;
		std::size_t smallest = tensor.getSize()[0];
		for (const auto &a : tensor.getSize())
		{
			if (a < smallest)
				smallest = a;
		}
		for (std::size_t k = 0; k < smallest; k++)
		{
			std::size_t index = 0;
			for (std::size_t i = 0; i < sizeof...(U); i++)
			{
				std::size_t add = 1;
				for (std::size_t j = i + 1; j < sizeof...(U); j++)
				{
					add *= tensor.getSize()[j];
				}
				index += k * add;
			}
			tensor.content[index] = 1;
		}
		return tensor;
	}

	template<typename T, std::size_t... U>
	constexpr Tensor<T, U...>::Tensor() : content()
	{
	}

	template<typename T, std::size_t... U>
	constexpr std::vector<std::size_t> Tensor<T, U...>::getSize()
	{
		return { U... };
	}

	template<typename T, std::size_t... U>
	constexpr std::size_t Tensor<T, U...>::getTotalSize()
	{
		return (U * ...);
	}

	template<typename T, std::size_t... U>
	template<typename... V>
	constexpr T& Tensor<T, U...>::operator () (V... v)
	{
		static_assert(sizeof...(V) == sizeof...(U));
		std::size_t args[] = { v... };
		std::size_t index = 0;
		std::size_t add = 1;
		for (std::size_t i = sizeof...(U); i > 0; i--)
		{
			index += args[i - 1] * add;
			add *= getSize()[i - 1];
		}
		return content[index];
	}

	template<typename T, std::size_t... U>
	template<std::size_t... Indices>
	constexpr T& Tensor<T, U...>::get()
	{
		static_assert(sizeof...(Indices) == sizeof...(U));
		std::size_t args[] = { Indices... };
		std::size_t index = 0;
		std::size_t add = 1;
		for (std::size_t i = sizeof...(U); i > 0; i--)
		{
			index += args[i - 1] * add;
			add *= getSize()[i - 1];
		}
		return content[index];
	}

	template<typename T, std::size_t... U>
	constexpr T& Tensor<T, U...>::get(std::array<std::size_t, sizeof...(U)> indices)
	{
		std::size_t index = 0;
		for (std::size_t i = 0; i < sizeof...(U); i++)
		{
			std::size_t add = 1;
			for (std::size_t j = i + 1; j < sizeof...(U); j++)
			{
				add *= getSize()[j];
			}
			index += indices[i] * add;
		}
		return content[index];
	}

	template<typename T, std::size_t... U>
	inline T& Tensor<T, U...>::operator [] (std::size_t index)
	{
		if constexpr (CompileInfo::debug)
		{
			if (index >= getTotalSize())
			{
				throw std::invalid_argument("index bigger than total size");
			}
		}
		return content[index];
	}

	template<typename T, std::size_t... U>
	template<std::size_t... V>
	constexpr Tensor<T, U..., V...> Tensor<T, U...>::operator * (Tensor<T, V...>& tensor)
	{
		Tensor<T, U..., V...> result;
		for (std::size_t u = 0; u < (U * ...); u++)
		{
			for (std::size_t v = 0; v < (V * ...); v++)
			{
				result.get(u * (V * ...) + v) = content[u] * tensor.get(v);
			}
		}
		return result;
	}

	// -----------------------------------------------
	// TemplateType helpers
	// -----------------------------------------------

	template <std::size_t... T>
	struct TemplateType {};

	template <typename T, std::size_t... Args>
	auto toTensor(TemplateType<Args...> type)
	{
		return Tensor<T, Args...>(); 
	}

	template <typename T, std::size_t... Args>
	auto& getValueFromTensor(T& tensor, TemplateType<Args...> type)
	{
		return tensor(Args...);
	}

	template <std::size_t GetCount, std::size_t... Processed>
	TemplateType<Processed...> getFirstTypesExit(TemplateType<Processed...>)
	{
		return TemplateType<Processed...>();
	}

	template <std::size_t GetCount, std::size_t First, std::size_t... Rest, std::size_t... Processed, char a = 0, typename = typename std::enable_if<sizeof...(Processed) != GetCount && sizeof...(Rest) == 0>::type>
	auto getFirstTypes(TemplateType<Processed...>)
	{
		return getFirstTypesExit<GetCount>(TemplateType<Processed..., First>());
	}

	template <std::size_t GetCount, std::size_t First, std::size_t... Rest, std::size_t... Processed, int a = 0, typename = typename std::enable_if<sizeof...(Processed) == GetCount && sizeof...(Rest) == 0>::type>
	auto getFirstTypes(TemplateType<Processed...>)
	{
		return getFirstTypesExit<GetCount>(TemplateType<Processed...>());
	}

	// bool a = false ist hier nötig, weil sonst der MSVC compiler ein problem hat
	template <std::size_t GetCount, std::size_t First, std::size_t... Rest, std::size_t... Processed, bool a = false, typename = typename std::enable_if<sizeof...(Processed) == GetCount && sizeof...(Rest) != 0>::type>
	auto getFirstTypes(TemplateType<Processed...>)
	{
		return getFirstTypesExit<GetCount>(TemplateType<Processed...>());
	}

	template <std::size_t GetCount, std::size_t First, std::size_t... Rest, std::size_t... Processed, typename = typename std::enable_if<sizeof...(Processed) != GetCount && sizeof...(Rest) != 0>::type>
	auto getFirstTypes(TemplateType<Processed...>)
	{
		return getFirstTypes<GetCount, Rest...>(TemplateType<Processed..., First>());
	}

	// aufrufbare version
	template <std::size_t GetCount, std::size_t... Rest>
	auto getFirstTypesCallable()
	{
		return getFirstTypes<GetCount, Rest...>(TemplateType<>());
	}

	// -----------------------------------------------
	// IndexTensor
	// -----------------------------------------------

	template <typename Permutations, typename T, std::size_t... U>
	struct IndexTensor
	{
		constexpr IndexTensor();
		constexpr IndexTensor(Tensor<T, U...>& t, Permutations p);
		template <std::size_t... Sizes>
		constexpr std::vector<std::size_t> getPermutationsVariable(TemplateType<Sizes...>);
		template <std::size_t... Indices>
		constexpr T& indexToValue(TemplateType<Indices...>);
		constexpr T& indexToValue(std::vector<std::size_t> v);
		Tensor<T, U...>& tensor;
	};

	template <typename Permutations, typename T, std::size_t... U>
	template <std::size_t... Sizes>
	constexpr std::vector<std::size_t> IndexTensor<Permutations, T, U...>::getPermutationsVariable(TemplateType<Sizes...>)
	{
		return { Sizes... };
	}

	template<typename Permutations, typename T, std::size_t... U>
	constexpr IndexTensor<Permutations, T, U...>::IndexTensor()
	{
	}

	template <typename Permutations, typename T, std::size_t... U>
	constexpr IndexTensor<Permutations, T, U...>::IndexTensor(Tensor<T, U...>& t, Permutations p) : tensor(t)
	{
	}

	template <typename Permutations, typename T, std::size_t... U>
	template <std::size_t... Indices>
	constexpr T& IndexTensor<Permutations, T, U...>::indexToValue(TemplateType<Indices...>)
	{
		std::vector<std::size_t> permutations = getPermutationsVariable(Permutations());
		std::size_t indices[] = { Indices... };
		std::array<std::size_t, sizeof...(U)> realIndices;
		for (std::size_t i = 0; i < sizeof...(U); i++)
		{
			realIndices[i] = indices[permutations[i]];
		}
		return tensor.get(realIndices);
	}

	template <typename Permutations, typename T, std::size_t... U>
	constexpr T& IndexTensor<Permutations, T, U...>::indexToValue(std::vector<std::size_t> v)
	{
		std::vector<std::size_t> permutations = getPermutationsVariable(Permutations());
		std::array<std::size_t, sizeof...(U)> realIndices;
		for (std::size_t i = 0; i < sizeof...(U); i++)
		{
			realIndices[i] = v[permutations[i]];
		}
		return tensor.get(realIndices);
	}

	// -----------------------------------------------
	// Tensor::id implementation
	// -----------------------------------------------

	template <typename T, std::size_t... U>
	template <std::size_t... Indices>
	constexpr IndexTensor<TemplateType<Indices...>, T, U...> Tensor<T, U...>::id()
	{
		return ct::IndexTensor(*this, ct::TemplateType<Indices...>());
	}

	// -----------------------------------------------
	// staticTensorProductExtended
	// -----------------------------------------------

	template <typename T, std::size_t ResultSize, typename... U, std::size_t... Processed>
	constexpr void staticIndexLoop(TemplateType<Processed...> templateType, T& result, U&... u)
	{
		getValueFromTensor(result, getFirstTypesCallable<ResultSize, Processed...>()) += (u.indexToValue(templateType) * ...);
	}
	
	template <typename T, std::size_t ResultSize, std::size_t FirstIndex, std::size_t... RestIndices, typename... U, std::size_t... Processed, bool a = false, typename = typename std::enable_if<FirstIndex == 1>::type>
	constexpr void staticIndexLoop(TemplateType<Processed...>, T& result, U& ... u)
	{
		staticIndexLoop<decltype(result), ResultSize, RestIndices...>(TemplateType<Processed..., FirstIndex - 1>(), result, u...);
	}

	template <typename T, std::size_t ResultSize, std::size_t FirstIndex, std::size_t... RestIndices, typename... U, std::size_t... Processed, typename = typename std::enable_if<FirstIndex != 1>::type>
	constexpr void staticIndexLoop(TemplateType<Processed...>, T& result, U&... u)
	{
		staticIndexLoop<decltype(result), ResultSize, RestIndices...>(TemplateType<Processed..., FirstIndex - 1>(), result, u...);
		staticIndexLoop<decltype(result), ResultSize, FirstIndex - 1, RestIndices...>(TemplateType<Processed...>(), result, u...);
	}
	
	template <typename T, std::size_t ResultSize, std::size_t... Indices, typename... U>
	constexpr auto staticTensorProductExtended(U&... u)
	{
		decltype(toTensor<T>(getFirstTypesCallable<ResultSize, Indices...>())) result;
		staticIndexLoop<decltype(result), ResultSize, Indices...>(TemplateType<>(), result, u...);
		return result;
	}

	// -----------------------------------------------
	// tensorProductExtended
	// -----------------------------------------------

	template <typename T, std::size_t ResultSize, typename... U>
	constexpr void indexLoop(std::vector<std::size_t> processed, T& result, U& ... u)
	{
		std::array<std::size_t, ResultSize> realIndices;
		for (std::size_t i = 0; i < ResultSize; i++)
		{
			realIndices[i] = processed[i];
		}
		result.get(realIndices) += (u.indexToValue(processed) * ...);
	}

	template <typename T, std::size_t ResultSize, std::size_t FirstIndex, std::size_t... RestIndices, typename... U>
	constexpr void indexLoop(std::vector<std::size_t> processed, T& result, U& ... u)
	{
		for (std::size_t i = 0; i < FirstIndex; i++)
		{
			auto newProcessed = processed;
			newProcessed.push_back(i);
			indexLoop<decltype(result), ResultSize, RestIndices...>(newProcessed, result, u...);
		}
	}

	template <typename T, std::size_t ResultSize, std::size_t... Indices, typename... U>
	constexpr auto tensorProductExtended(U& ... u)
	{
		decltype(toTensor<T>(getFirstTypesCallable<ResultSize, Indices...>())) result;
		indexLoop<decltype(result), ResultSize, Indices...>(std::vector<std::size_t>{}, result, u...);
		return result;
	}

	// -----------------------------------------------
	// More TemplateType and IndexTensor helpers
	// -----------------------------------------------

	template <typename T, std::size_t... Sizes>
	auto constexpr getTensorType(Tensor<T, Sizes...>& tensor)
	{
		T t;
		return t;
	}

	template <std::size_t T>
	constexpr auto getMax(TemplateType<T> t1, TemplateType<> t2)
	{
		return TemplateType<T>();
	}

	template <std::size_t T, std::size_t U, bool a = false, typename = typename std::enable_if<(T >= U)>::type>
	constexpr auto getMax(TemplateType<T> t1, TemplateType<U> t2)
	{
		return TemplateType<T>();
	}

	template <std::size_t T, std::size_t U, typename = typename std::enable_if<!(T >= U)>::type>
	constexpr auto getMax(TemplateType<T> t1, TemplateType<U> t2)
	{
		return TemplateType<U>();
	}

	template <std::size_t T>
	constexpr auto getMin(TemplateType<T> t1, TemplateType<> t2)
	{
		return TemplateType<T>();
	}

	template <std::size_t T, std::size_t U, bool a = false, typename = typename std::enable_if<(T >= U)>::type>
	constexpr auto getMin(TemplateType<T> t1, TemplateType<U> t2)
	{
		return TemplateType<U>();
	}

	template <std::size_t T, std::size_t U, typename = typename std::enable_if<!(T >= U)>::type>
	constexpr auto getMin(TemplateType<T> t1, TemplateType<U> t2)
	{
		return TemplateType<T>();
	}

	template <std::size_t Index, std::size_t First, std::size_t... Others, bool a = 0, typename = typename std::enable_if<Index == 0>::type>
	constexpr auto getIndexFromIndices()
	{
		return TemplateType<First>();
	}

	template <std::size_t Index, std::size_t First, std::size_t... Others, typename = typename std::enable_if<Index != 0>::type>
	constexpr auto getIndexFromIndices()
	{
		return getIndexFromIndices<Index - 1, Others...>();
	}

	template <std::size_t Index, std::size_t... Sizes>
	constexpr auto getIndexFromTemplateType(TemplateType<Sizes...>)
	{
		return getIndexFromIndices<Index, Sizes...>();
	}

	template <std::size_t Index, typename Permutations, typename T, std::size_t... Sizes>
	constexpr auto getSmallestSizeFromIndices(IndexTensor<Permutations, T, Sizes...>& tensorIndex)
	{
		return TemplateType<static_cast<std::size_t>(-1)>();
	}

	template <std::size_t Index, std::size_t First, std::size_t... Others, typename Permutations, typename T, std::size_t... Sizes, bool a = false, typename = typename std::enable_if<!std::is_same<TemplateType<Index>, decltype(getIndexFromTemplateType<sizeof...(Sizes) - sizeof...(Others) - 1>(Permutations()))>::value>::type>
	constexpr auto getSmallestSizeFromIndices(IndexTensor<Permutations, T, Sizes...>& tensorIndex)
	{
		return getMin(TemplateType<static_cast<std::size_t>(-1)>(), getSmallestSizeFromIndices<Index, Others...>(tensorIndex));
	}

	template <std::size_t Index, std::size_t First, std::size_t... Others, typename Permutations, typename T, std::size_t... Sizes, typename = typename std::enable_if<std::is_same<TemplateType<Index>, decltype(getIndexFromTemplateType<sizeof...(Sizes) - sizeof...(Others) - 1>(Permutations()))>::value>::type>
	constexpr auto getSmallestSizeFromIndices(IndexTensor<Permutations, T, Sizes...>& tensorIndex)
	{
		return getMin(getIndexFromIndices<sizeof...(Sizes) - sizeof...(Others) - 1, Sizes...>(), getSmallestSizeFromIndices<Index, Others...>(tensorIndex));
	}

	template <std::size_t Index>
	constexpr auto getSmallestSizeFromTensors()
	{
		return TemplateType<static_cast<std::size_t>(-1)>();
	}

	template <std::size_t Index, typename Permutations, typename T, std::size_t... Sizes, typename... Others>
	constexpr auto getSmallestSizeFromTensors(IndexTensor<Permutations, T, Sizes...>& first, Others&... others)
	{
		return getMin(getSmallestSizeFromIndices<Index, Sizes...>(first), getSmallestSizeFromTensors<Index>(others...));
	}

	template <std::size_t... First, std::size_t... Second>
	constexpr auto combineTemplateTypes(TemplateType<First...>, TemplateType<Second...>)
	{
		return TemplateType<First..., Second...>();
	}

	template <std::size_t Largest, typename... Others, bool a = false, typename = typename std::enable_if<Largest == 0>::type>
	constexpr auto getAllSmallestSizesFromTensors(Others&... others)
	{
		return getSmallestSizeFromTensors<Largest>(others...);
	}

	template <std::size_t Largest, typename... Others, typename = typename std::enable_if<Largest != 0>::type>
	constexpr auto getAllSmallestSizesFromTensors(Others&... others)
	{
		return combineTemplateTypes(getAllSmallestSizesFromTensors<Largest - 1>(others...), getSmallestSizeFromTensors<Largest>(others...));
	}

	template <bool a = false>
	constexpr auto getHighestPermutationFromIndices(TemplateType<>)
	{
		return TemplateType<0>();
	}

	template <bool a = false, std::size_t First, std::size_t... Others>
	constexpr auto getHighestPermutationFromIndices(TemplateType<First, Others...>)
	{
		return getMax(TemplateType<First>(), getHighestPermutationFromIndices<true>(TemplateType<Others...>()));
	}

	template <bool a = false>
	constexpr auto getHighestPermutationFromTensors()
	{
		return TemplateType<0>();
	}

	template <bool a = false, typename Permutations, typename T, std::size_t... Sizes, typename... Others>
	constexpr auto getHighestPermutationFromTensors(IndexTensor<Permutations, T, Sizes...>& first, Others& ... others)
	{
		return getMax(getHighestPermutationFromIndices<true>(Permutations()), getHighestPermutationFromTensors<true>(others...));
	}

	template <std::size_t Largest, typename... Others, typename = typename std::enable_if<Largest != 0>::type>
	constexpr auto getAllSmallestSizesFromTensors(TemplateType<Largest>, Others& ... others)
	{
		return getAllSmallestSizesFromTensors<Largest>(others...);
	}

	// -----------------------------------------------
	// automatic tensor products
	// -----------------------------------------------

	template <std::size_t ResultSize, std::size_t... Size, typename FirstTensor, typename... OtherTensors>
	constexpr auto staticTensorProductInternal(TemplateType<Size...>, FirstTensor& first, OtherTensors& ... u)
	{
		return staticTensorProductExtended<decltype(getTensorType(first.tensor)), ResultSize, Size...>(first, u...);
	}

	template <std::size_t ResultSize, typename FirstTensor, typename... OtherTensors>
	constexpr auto staticTensorProduct(FirstTensor& first, OtherTensors& ... u)
	{
		return staticTensorProductInternal<ResultSize>(getAllSmallestSizesFromTensors(getHighestPermutationFromTensors(first, u...), first, u...), first, u...);
	}

	template <std::size_t ResultSize, std::size_t... Size, typename FirstTensor, typename... OtherTensors>
	constexpr auto tensorProductInternal(TemplateType<Size...>, FirstTensor& first, OtherTensors& ... u)
	{
		return tensorProductExtended<decltype(getTensorType(first.tensor)), ResultSize, Size...>(first, u...);
	}

	template <std::size_t ResultSize, typename FirstTensor, typename... OtherTensors>
	constexpr auto tensorProduct(FirstTensor& first, OtherTensors& ... u)
	{
		return tensorProductInternal<ResultSize>(getAllSmallestSizesFromTensors(getHighestPermutationFromTensors(first, u...), first, u...), first, u...);
	}
 }

namespace rt
{
	template <typename T>
	class Tensor
	{
	};
}