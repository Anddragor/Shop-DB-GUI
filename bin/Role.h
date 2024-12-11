#pragma once

#include <QString>

class Role {
protected:
    QString name;
	bool rights;
public:

	bool getRights() { return rights;}
};
