#pragma once

#include "Role.h"

class Admin : public virtual Role {
public:
    Admin(QString name)
	{
		this->name = name;
		this->rights = 1;
	}
};
