#pragma once

#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <msgpack.hpp>

namespace rmte {
inline std::size_t get_sstream_size(std::stringstream& ss)
{
	std::size_t tmp = ss.tellg();
	ss.seekg(0, std::ios::end);
	std::size_t size = ss.tellg();
	ss.seekg(tmp, std::ios::beg);
	return size;
}

inline std::vector<std::uint8_t> sstream_to_vector(std::stringstream&& ss)
{
	ss.seekg(0, std::ios::end);
	std::size_t size = ss.tellg();
	ss.seekg(0, std::ios::beg);

	std::vector<std::uint8_t> data(size);
	ss.read((char*)&data[0], size);
	return data;
}

inline std::stringstream vector_to_sstream(std::vector<std::uint8_t>&& data)
{
	std::stringstream ss;
	ss.write((const char*)&data[0], data.size());
	return ss;
}

template <typename... Args>
inline std::vector<std::uint8_t> pack_fct_args(const std::string& fct_name, Args... args)
{
	std::stringstream ss;
	msgpack::pack(ss, fct_name);
	msgpack::pack(ss, std::tuple<Args...>(args...));
	return sstream_to_vector(std::move(ss));
}

template <typename T>
inline T get_obj_from_data(std::vector<std::uint8_t>&& data)
{
	std::stringstream ss = vector_to_sstream(std::move(data));
	msgpack::object_handle obj_handle = msgpack::unpack(ss.str().data(), ss.str().size());
	const msgpack::object& obj = obj_handle.get();
	return obj.as<T>();
}

template <typename T>
inline T get_obj_from_sstream(std::stringstream& ss, std::size_t& offset)
{
	msgpack::object_handle obj_handle = msgpack::unpack(ss.str().data(), ss.str().size(), offset);
	const msgpack::object& obj = obj_handle.get();
	return obj.as<T>();
}

template <typename T>
inline T get_obj_from_sstream(std::stringstream& ss)
{
	msgpack::object_handle obj_handle = msgpack::unpack(ss.str().data(), ss.str().size());
	const msgpack::object& obj = obj_handle.get();
	return obj.as<T>();
}

inline const msgpack::object& get_pack_obj_from_sstream(std::stringstream& ss, std::size_t& offset)
{
	msgpack::object_handle obj_handle = msgpack::unpack(ss.str().data(), ss.str().size(), offset);
	return obj_handle.get();
}

inline const msgpack::object& get_pack_obj_from_sstream(std::stringstream& ss)
{
	msgpack::object_handle obj_handle = msgpack::unpack(ss.str().data(), ss.str().size());
	return obj_handle.get();
}
} // rmte
