#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include <fst/print.h>
#include <boost/asio.hpp>
#include <msgpack.hpp>

#include "pack.hpp"

namespace rmte {
template <typename T, typename ReturnType, typename... Args>
struct call_struct {
	call_struct() = default;

	inline ReturnType operator()(boost::asio::ip::tcp::socket& client_socket,
		rmte::FunctionName<T, ReturnType, Args...> fct_info, Args... args)
	{
		try {
			std::vector<std::uint8_t> buffer = pack_fct_args(fct_info.name, args...);

			const unsigned int length = (unsigned int)buffer.size();
			boost::asio::write(
				client_socket, boost::asio::buffer((const char*)&length, sizeof(unsigned int)));
			boost::asio::write(client_socket, boost::asio::buffer((const char*)&buffer[0], buffer.size()));

			unsigned int return_length = 0;
			boost::asio::read(client_socket, boost::asio::buffer(&return_length, sizeof(unsigned int)),
				boost::asio::transfer_exactly(sizeof(unsigned int)));

			std::vector<std::uint8_t> r_data(return_length);

			boost::asio::read(
				client_socket, boost::asio::buffer(r_data), boost::asio::transfer_exactly(return_length));

			return get_obj_from_data<ReturnType>(std::move(r_data));

		} catch (std::exception& e) {
			fst::errprint("Exception :", e.what());
		}

		return ReturnType();
	}
};

// void return type.
template <typename T, typename... Args>
struct call_struct<T, void, Args...> {
	call_struct()
	{
	}

	void operator()(
		boost::asio::ip::tcp::socket& socket, rmte::FunctionName<T, void, Args...> fct_info, Args... args)
	{
		try {
			std::vector<std::uint8_t> buffer = pack_fct_args(fct_info.name, args...);

			const unsigned int length = (unsigned int)buffer.size();
			boost::asio::write(socket, boost::asio::buffer((const char*)&length, sizeof(unsigned int)));
			boost::asio::write(socket, boost::asio::buffer((const char*)&buffer[0], length));
		} catch (std::exception& e) {
			fst::errprint("Exception :", e.what());
		}
	}
};

template <typename T, typename ReturnType, typename... Args>
call_struct<T, ReturnType, Args...> create_call_struct(
	boost::asio::ip::tcp::socket&, rmte::FunctionName<T, ReturnType, Args...>)
{
	return call_struct<T, ReturnType, Args...>();
}
} // rmte.
