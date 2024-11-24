#pragma once

#include <string>
#include "Role.h"
#include "User.h"
#include "Admin.h"
#include "Controller.h"

class UserManager 
{
private:
	Controller controller;

	std::string generatePassword(std::string password);
public:
	Role signUp(std::string username, std::string password);
	Role signIn(std::string username, std::string password);
};