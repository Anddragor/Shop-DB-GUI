#include "UserManager.h"

#include <sstream>
#include <iomanip>


QString UserManager::generatePassword(QString password)
{
    std::string stdPassword = password.toStdString();

    std::hash<std::string> mystdhash;
    size_t hash_value = mystdhash(stdPassword);

    std::stringstream ss;
    ss << std::setw(16) << std::setfill('0') << std::hex << hash_value;

    return QString::fromStdString(ss.str());
}

Role UserManager::signUp(QString username, QString password)
{
    password = generatePassword(password);
    if (controller.findUser(username)) throw std::exception("User already exist");

	controller.addUser(username, password, 0);

	return User(username);
}

Role UserManager::signIn(QString username, QString password)
{
    password = generatePassword(password);
	if (!controller.findUser(username, password)) throw std::exception("Wrong username or password");

	if (controller.getUserRights(username)) return Admin(username);
	else return User(username);
}

void UserManager::createAdmin(QString username, QString password)
{
	password = generatePassword(password);
	controller.addUser(username, password, 1);
}
