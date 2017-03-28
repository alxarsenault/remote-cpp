#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <msgpack.hpp>
#include <fst/print.h>

#include "fct_map.hpp"
#include "pack.hpp"

namespace rmte {
template <typename T, typename ReturnType, typename... Args>
struct CallStruct {
	CallStruct()
	{
	}

	ReturnType operator()(
		boost::asio::ip::tcp::socket& sock, rmte::FunctionName<T, ReturnType, Args...> fct_info, Args... args)
	{
		try {
            std::stringstream ss;
			msgpack::pack(ss, fct_info.name);
			msgpack::pack(ss, std::make_tuple(args...));

//			std::vector<std::uint8_t> buffer(
//				(std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());
            
            std::vector<std::uint8_t> buffer = sstream_to_vector(std::move(ss));

			const unsigned int length = (unsigned int)buffer.size();
			boost::asio::write(sock, boost::asio::buffer((const char*)&length, sizeof(unsigned int)));
			boost::asio::write(sock, boost::asio::buffer((const char*)&buffer[0], buffer.size()));

			unsigned int return_length = 0;
			boost::asio::read(sock, boost::asio::buffer(&return_length, sizeof(unsigned int)),
				boost::asio::transfer_exactly(sizeof(unsigned int)));

			std::stringstream rss;
			std::vector<std::uint8_t> r_data(return_length);
			boost::asio::read(
				sock, boost::asio::buffer(r_data), boost::asio::transfer_exactly(return_length));
			rss.write((const char*)&r_data[0], r_data.size());
			msgpack::object_handle obj_handle_name = msgpack::unpack(rss.str().data(), rss.str().size(), 0);
			const msgpack::object& r_obj = obj_handle_name.get();
			return r_obj.as<ReturnType>();
		} catch (std::exception& e) {
			fst::errprint("Exception :", e.what());
		}

		return ReturnType();
	}
};

// void return type.
template <typename T, typename... Args>
struct CallStruct<T, void, Args...> {
	CallStruct()
	{
	}

	void operator()(
		boost::asio::ip::tcp::socket& sock, rmte::FunctionName<T, void, Args...> fct_info, Args... args)
	{
		std::stringstream ss;

		try {

			msgpack::pack(ss, fct_info.name);
			msgpack::pack(ss, std::make_tuple(args...));

			std::vector<std::uint8_t> buffer(
				(std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());

			const unsigned int length = (unsigned int)buffer.size();
			boost::asio::write(sock, boost::asio::buffer((const char*)&length, sizeof(unsigned int)));
			boost::asio::write(sock, boost::asio::buffer((const char*)&buffer[0], buffer.size()));
		} catch (std::exception& e) {
			fst::errprint("Exception :", e.what());
		}
	}
};

template <typename T, typename ReturnType, typename... Args>
CallStruct<T, ReturnType, Args...> CreateCallStruct(
	boost::asio::ip::tcp::socket&, rmte::FunctionName<T, ReturnType, Args...>)
{
	return CallStruct<T, ReturnType, Args...>();
}

class remote_client {
public:
	inline remote_client()
		: sock(io_service)
		, resolver(io_service)
	{
	}

	inline ~remote_client()
	{
	}

	void Close()
	{
		sock.close();
	}

	inline void Connect(const std::string& ip, const std::string& port)
	{
		try {
			boost::asio::connect(sock, resolver.resolve({ ip, port }));
		} catch (std::exception& e) {
			fst::errprint("Exception :", e.what());
		}
	}

	template <typename T, typename ReturnType, typename... Args>
	inline ReturnType Call(rmte::FunctionName<T, ReturnType, Args...> fct_info, Args... args)
	{
		auto cs = CreateCallStruct(sock, fct_info);
		return cs(sock, fct_info, args...);
	}

private:
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::socket sock;
	boost::asio::ip::tcp::resolver resolver;
};
} // rmte.
