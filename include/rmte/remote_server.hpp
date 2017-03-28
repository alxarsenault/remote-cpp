#pragma once

#include <iostream>
#include <utility>
#include <boost/asio.hpp>

//#include "remote_call.hpp"
#include "fct_map.hpp"

namespace rmte {
template <typename T>
class remote_server;

template <typename T>
class RemoteSession : public std::enable_shared_from_this<RemoteSession<T>> {
public:
	RemoteSession(remote_server<T>* remote_server, boost::asio::ip::tcp::socket socket)
		: _remote_server(remote_server)
		, socket_(std::move(socket))
	{
	}

	void start()
	{
		do_read();
	}

private:
	void do_read()
	{
		auto self(this->shared_from_this());

		boost::asio::async_read(socket_, boost::asio::buffer(&_buffer_length, sizeof(unsigned int)),
			boost::asio::transfer_exactly(sizeof(unsigned int)),
			[this, self](boost::system::error_code ec, std::size_t) {
				if (!ec) {
					do_read_msg();
				}
			});
	}

	void do_read_msg()
	{
		auto self(this->shared_from_this());

		_data.resize(_buffer_length);

		boost::asio::async_read(socket_, boost::asio::buffer(_data),
			boost::asio::transfer_exactly(_buffer_length),
			[this, self](boost::system::error_code ec, std::size_t) {
				if (!ec) {
					std::stringstream ss;
					ss.write((const char*)&_data[0], _data.size());

					std::size_t offset = 0;
					msgpack::object_handle obj_handle_name
						= msgpack::unpack(ss.str().data(), ss.str().size(), offset);
					const msgpack::object& obj_name = obj_handle_name.get();

					const std::string fct_name = obj_name.as<std::string>();

					msgpack::object_handle obj_handle_param
						= msgpack::unpack(ss.str().data(), ss.str().size(), offset);
					const msgpack::object& obj_param = obj_handle_param.get();

					_returned_ss = _remote_server->Call(fct_name, obj_param);

					_buffer_length = 0;
					_data.clear();

					std::vector<std::uint8_t> dd(
						(std::istreambuf_iterator<char>(_returned_ss)), std::istreambuf_iterator<char>());

					if (!dd.empty()) {
						_returned_data = dd;
						do_write();
						return;
					}

					do_read();
				}
			});
	}

	void do_write()
	{
		auto self(this->shared_from_this());
		_returned_length = _returned_data.size();

		boost::asio::async_write(socket_, boost::asio::buffer(&_returned_length, sizeof(unsigned int)),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec) {
					boost::asio::write(
						socket_, boost::asio::buffer((const char*)&_returned_data[0], _returned_data.size()));

					_returned_ss.clear();
					_returned_length = 0;
					_returned_data.clear();

					do_read();
				}

			});
	}

	remote_server<T>* _remote_server = nullptr;
	boost::asio::ip::tcp::socket socket_;
	std::vector<std::uint8_t> _data;
	unsigned int _buffer_length = 0;
	std::stringstream _returned_ss;
	std::vector<std::uint8_t> _returned_data;
	unsigned int _returned_length = 0;
};

template <typename T>
class remote_server : public rmte::fct_map {
public:
	remote_server(T* handle, short port)
		: _acceptor(_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		, _socket(_io_service)
		, _handle(handle)
	{
		if (_handle == nullptr) {
			return;
		}

		do_accept();
	}

	void Run()
	{
		try {
			_io_service.run();
		} catch (std::exception& e) {
			fst::errprint("Exception :", e.what());
		}
	}

	std::stringstream Call(const std::string& name, const msgpack::object& obj)
	{
		return rmte::fct_map::Call(_handle, name, obj);
	}

private:
	void do_accept()
	{
		_acceptor.async_accept(_socket, [this](boost::system::error_code ec) {
			if (!ec) {
				std::make_shared<RemoteSession<T>>(this, std::move(_socket))->start();
			}

			do_accept();
		});
	}

	boost::asio::io_service _io_service;
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::socket _socket;
	T* _handle = nullptr;
};
} // rmte.
