#pragma once

#include "Role.h"

class Admin : public virtual Role {
public:
	Admin(std::string name)
	{
		this->name = name;
		this->rights = 1;
	}
};
