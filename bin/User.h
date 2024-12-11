#pragma once

#include "Role.h"

class User : public virtual Role {
public:
    User(QString name)
	{
		this->name = name;
		this->rights = 0;
	}
};
