#include "UserManager.h"

std::string UserManager::generatePassword(std::string password)
{

}

Role UserManager::signUp(std::string username, std::string password)
{
    //password = generatePassword(password);
	if (controller.findUser(username, password)) throw std::exception("User already exist");

	controller.addUser(username, password, 0);

	return User(username);
}

Role UserManager::signIn(std::string username, std::string password)
{
    //password = generatePassword(password);
	if (!controller.findUser(username, password)) throw std::exception("Wrong username or password");

	if (controller.getUserRights(username)) return Admin(username);
	else return User(username);
}
