#pragma once

#include <sstream>
#include <utility>
#include <sstream>
#include <msgpack.hpp>
#include <typeinfo>
#include <map>

#include "fct_holder.hpp"
#include "fct_info.hpp"

namespace rmte {
class fct_map {
public:
	fct_map() = default;

	template <typename T, typename ReturnType, typename... Args>
	void RegisterFunction(const rmte::FunctionInfo<T, ReturnType, Args...>& fct_info)
	{
		_fct_map.insert(std::pair<std::string, std::shared_ptr<rmte::abstract_function>>(
			fct_info.name, fct_info.holder.Copy()));
	}

protected:
	std::stringstream Call(void* t, const std::string& name, const msgpack::object& obj)
	{
		std::map<std::string, std::shared_ptr<rmte::abstract_function>>::iterator it = _fct_map.find(name);

		if (it != _fct_map.end()) {
			return it->second->Call(t, obj);
		}

		return std::stringstream();
	}

private:
	std::map<std::string, std::shared_ptr<rmte::abstract_function>> _fct_map;
};
} // rmte
