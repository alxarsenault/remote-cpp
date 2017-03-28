#pragma once

#include <sstream>
#include <tuple>
#include <utility>
#include <memory>
#include <msgpack.hpp>

namespace rmte {
class abstract_function {
public:
	virtual ~abstract_function()
	{
	}

	virtual std::stringstream Call(void* t, const msgpack::object& obj) = 0;
};

template <typename T, typename ReturnType, typename... Args>
struct function_holder : public abstract_function {
	typedef ReturnType (T::*type)(Args...);
	typedef std::tuple<Args...> tuple_type;
	type func_ptr;

	function_holder(type func)
		: func_ptr(func)
	{
	}

	template <size_t... I>
	ReturnType callFunc(T* type, tuple_type msg, std::index_sequence<I...>)
	{
		return ((type)->*(func_ptr))(std::get<I>(msg)...);
	}

	virtual std::stringstream Call(void* t, const msgpack::object& obj)
	{
		tuple_type msg = obj.as<std::tuple<Args...>>();
		static constexpr auto size = std::tuple_size<tuple_type>::value;
		ReturnType rt = callFunc(static_cast<T*>(t), msg, std::make_index_sequence<size>{});
		std::stringstream ss;
		msgpack::pack(ss, rt);
		return ss;
	}

	std::shared_ptr<abstract_function> Copy() const
	{
		return std::shared_ptr<abstract_function>(new function_holder(func_ptr));
	}
};

// function_holder with void return type.
template <typename T, typename... Args>
struct function_holder<T, void, Args...> : public abstract_function {
	typedef void (T::*type)(Args...);
	typedef std::tuple<Args...> tuple_type;
	type func_ptr;

	function_holder(type func)
		: func_ptr(func)
	{
	}

	template <size_t... I>
	void callFunc(T* type, tuple_type msg, std::index_sequence<I...>)
	{
		((type)->*(func_ptr))(std::get<I>(msg)...);
	}

	virtual std::stringstream Call(void* t, const msgpack::object& obj)
	{
		static constexpr auto size = std::tuple_size<tuple_type>::value;
		callFunc(static_cast<T*>(t), obj.as<std::tuple<Args...>>(), std::make_index_sequence<size>{});
		return std::stringstream();
	}

	std::shared_ptr<abstract_function> Copy() const
	{
		return std::shared_ptr<abstract_function>(new function_holder(func_ptr));
	}
};

template <typename T, typename... Args>
function_holder<Args...> create_function_holder(void (T::*fct)(Args...))
{
	return function_holder<T, Args...>(fct);
}

} // rmte.
