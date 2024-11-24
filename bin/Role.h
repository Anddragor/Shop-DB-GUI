#pragma once

#include <string>

class Role {
protected:
	std::string name;
	bool rights;
public:

	bool getRights() { return rights;}
};