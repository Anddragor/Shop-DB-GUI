#pragma once

#include "Role.h"
#include "User.h"
#include "Admin.h"
#include "Controller.h"

class UserManager 
{
private:
	Controller controller;

    QString generatePassword(QString password);
public:
    Role signUp(QString username, QString password);
    Role signIn(QString username, QString password);

    void createAdmin(QString username, QString password);
};
