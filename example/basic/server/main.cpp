
#include <fst/print.h>
#include "server_interface.hpp"
#include "rmte/remote_server.hpp"

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class Server : public ServerInterface, public rmte::remote_server<Server> {
public:
	Server(short port)
		: rmte::remote_server<Server>(this, port)
	{
		RMTE_REGISTER_FUNCTION(ServerInterface::CreateFolder);
		RMTE_REGISTER_FUNCTION(ServerInterface::GetId);
		RMTE_REGISTER_FUNCTION(ServerInterface::MyFunction2);
		RMTE_REGISTER_FUNCTION(ServerInterface::GetNames);
		RMTE_REGISTER_FUNCTION(ServerInterface::AddName);
		RMTE_REGISTER_FUNCTION(ServerInterface::SendFile);
	}

	virtual void CreateFolder(std::string s0)
	{
		mkdir(s0.c_str(), 0777);
	}

	virtual std::string GetId()
	{
		auto randchar = []() -> char {
			const char charset[] = "0123456789"
								   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								   "abcdefghijklmnopqrstuvwxyz";
			const size_t max_index = (sizeof(charset) - 1);
			return charset[rand() % max_index];
		};

		std::string str(25, 0);
		std::generate_n(str.begin(), 25, randchar);
		_client_id_list.push_back(str);
		return str;
	}

	virtual int MyFunction2(int a, int b, int c)
	{
		fst::print("MyFunction2 :", a, b, c);
		return a + b + c;
	}

	virtual std::vector<std::string> GetNames(std::string client_id)
	{
		for (auto& n : _client_id_list) {
			if (n == client_id) {
				return _name_list;
			}
		}

		return {};
	}

	virtual void AddName(std::string client_id, std::string name)
	{
		for (auto& n : _client_id_list) {
			if (n == client_id) {
				_name_list.push_back(name);
			}
		}
	}

	virtual void SendFile(std::string file_name, std::vector<char> data)
	{
		fst::print("Got file content :", (int)data.size());
		std::ofstream fout(file_name, std::ios::out | std::ios::binary);
		fout.write((char*)&data[0], data.size());
		fout.close();
	}

private:
	std::vector<std::string> _name_list;
	std::vector<std::string> _client_id_list;
};

int main()
{
	Server server(5000);
	server.Run();

	return 0;
}
