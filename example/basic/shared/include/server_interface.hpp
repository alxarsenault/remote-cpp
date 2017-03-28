#pragma once

#include <string>
#include <vector>

class ServerInterface {
public:
	ServerInterface() = default;

	virtual void CreateFolder(std::string s0) = 0;

	virtual std::string GetId() = 0;

	virtual int MyFunction2(int a, int b, int c) = 0;

	virtual std::vector<std::string> GetNames(std::string client_id) = 0;

	virtual void AddName(std::string client_id, std::string name) = 0;

	virtual void SendFile(std::string file_name, std::vector<char> data) = 0;
};
