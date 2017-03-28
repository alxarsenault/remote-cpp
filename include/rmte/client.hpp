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

#include "call_struct.hpp"

namespace rmte {
class client {
public:
	inline client()
		: _socket(_io_service)
		, _resolver(_io_service)
	{
	}

	void close()
	{
		_socket.close();
	}

	inline void connect(const std::string& ip, const std::string& port)
	{
		try {
			boost::asio::connect(_socket, _resolver.resolve({ ip, port }));
		} catch (std::exception& e) {
			fst::errprint("Exception :", e.what());
		}
	}
    
    template <typename T, typename ReturnType, typename... Args>
    inline ReturnType call(rmte::FunctionName<T, ReturnType, Args...> fct_info, Args... args)
    {
        try {
            auto cs = create_call_struct(_socket, fct_info);
            return cs(_socket, fct_info, args...);
        } catch (std::exception& e) {
            fst::errprint("Exception :", e.what());
        }
        
        return ReturnType();
    }

	template <typename T, typename ReturnType, typename... Args>
	inline ReturnType call(rmte::FunctionName<T, ReturnType, Args...> fct_info, Args&&... args)
	{
		try {
			auto cs = create_call_struct(_socket, fct_info);
			return cs(_socket, fct_info, std::forward<Args>(args)...);
		} catch (std::exception& e) {
			fst::errprint("Exception :", e.what());
		}
        
		return ReturnType();
	}
    
    template <typename T, typename ReturnType>
    inline ReturnType call(rmte::FunctionName<T, ReturnType> fct_info)
    {
        try {
            auto cs = create_call_struct(_socket, fct_info);
            return cs(_socket, fct_info);
        } catch (std::exception& e) {
            fst::errprint("Exception :", e.what());
        }
        
        return ReturnType();
    }

private:
	boost::asio::io_service _io_service;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::ip::tcp::resolver _resolver;
};
} // rmte.
