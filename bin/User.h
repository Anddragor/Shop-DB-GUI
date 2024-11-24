#pragma once

#include "Role.h"

class User : public virtual Role {
public:
	User(std::string name)
	{
		this->name = name;
		this->rights = 0;
	}
};
