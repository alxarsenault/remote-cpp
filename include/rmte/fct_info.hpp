#pragma once

#include <sstream>
#include <utility>
#include <string>
#include <typeinfo>

#include "fct_holder.hpp"

namespace rmte {
template <typename T, typename ReturnType, typename... Args>
struct FunctionInfo {
	FunctionInfo() = default;

	FunctionInfo(ReturnType (T::*fct)(Args...), const std::string& name)
		: holder(fct)
		, name(name)
	{
	}

	rmte::function_holder<T, ReturnType, Args...> holder;
	std::string name;
};

template <typename T, typename ReturnType, typename... Args>
FunctionInfo<T, ReturnType, Args...> CreateFunctionInfo(const char* fct_name, ReturnType (T::*fct)(Args...))
{
	return FunctionInfo<T, ReturnType, Args...>(
		fct, std::string(fct_name) + "_" + std::string(typeid(fct).name()));
}

#define CREATE_FUNC_INFO(fct) rmte::CreateFunctionInfo(#fct, &fct)
#define RMTE_REGISTER_FUNCTION(fct) RegisterFunction(CREATE_FUNC_INFO(fct));

template <typename T, typename ReturnType, typename... Args>
std::string GetFctName(const char* fct_name, ReturnType (T::*fct)(Args...))
{
	return std::string(fct_name) + "_" + std::string(typeid(fct).name());
}

#define GET_FUNC_NAME(fct) rmte::GetFctName(#fct, &fct)

template <typename T, typename ReturnType, typename... Args>
struct FunctionName {
	FunctionName() = default;

	FunctionName(ReturnType (T::*)(Args...), const std::string& name)
		: name(name)
	{
	}

	typedef ReturnType (T::*fct)(Args...);
	std::string name;
};

template <typename T, typename ReturnType, typename... Args>
FunctionName<T, ReturnType, Args...> GetFunctionName(const char* fct_name, ReturnType (T::*fct)(Args...))
{
	return FunctionName<T, ReturnType, Args...>(
		fct, std::string(fct_name) + "_" + std::string(typeid(fct).name()));
}

#define GET_FUNC_NAME_INFO(fct) rmte::GetFunctionName(#fct, &fct)
#define RMTE_FUNC_INFO(fct) rmte::GetFunctionName(#fct, &fct)

} // rmte
