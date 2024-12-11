#include "Controller.h"

#include <windows.h>
#include <QString>
#include <QSqlDatabase>
#include <stdexcept>
#include <QSettings>
#include <QString>
#include <QSqlDatabase>
#include <stdexcept>

Controller::Controller()
{
    QString iniFilePath = "config.ini";

    QSettings settings(iniFilePath, QSettings::IniFormat);

    QString host = settings.value("DBSettings/host", "localhost").toString();
    QString port = settings.value("DBSettings/port", "5432").toString();
    QString name = settings.value("DBSettings/name", "postgres").toString();
    QString user = settings.value("DBSettings/user", "postgres").toString();
    QString password = settings.value("DBSettings/password", "1111").toString();

    connection = QSqlDatabase::addDatabase("QPSQL");
    connection.setHostName(host);
    connection.setPort(port.toInt());
    connection.setDatabaseName(name);
    connection.setUserName(user);
    connection.setPassword(password);

    if (!connection.open())
    {
        throw std::runtime_error("Failed to connect to the database");
    }
}


Controller::~Controller() 
{
    connection.close();
}

QSqlQuery Controller::getGoods() 
{
    QString query = "SELECT * FROM goods";
    QSqlQuery result(connection);
    if (!result.exec(query)) throw std::runtime_error("Query execution failed: " + result.lastError().text().toStdString());
    return result;
}

QSqlQuery Controller::getSales() 
{
    QString query = "SELECT * FROM sales";
    QSqlQuery result(connection);
    if (!result.exec(query)) throw std::runtime_error("Query execution failed: " + result.lastError().text().toStdString());
    return result;
}

QSqlQuery Controller::getMostPopularGoods() 
{
    QString query = "SELECT * FROM most_popular_goods";
    QSqlQuery result(connection);
    if (!result.exec(query)) throw std::runtime_error("Query execution failed: " + result.lastError().text().toStdString());
    return result;
}

QSqlQuery Controller::getWarehouseStatistic(size_t warehouse_number) 
{
    QString query = "SELECT * FROM warehouse" + QString::number(warehouse_number);
    QSqlQuery result(connection);
    if (!result.exec(query)) throw std::runtime_error("Query execution failed: " + result.lastError().text().toStdString());
    return result;
}

QSqlQuery Controller::getGraphTable(const QString& good_name)
{
    QString query = "SELECT * FROM get_graph_table(:good_name)";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":good_name", good_name);
    if (!q.exec()) throw std::runtime_error("User lookup failed: " + q.lastError().text().toStdString());
    return q;
}

size_t Controller::getSellsChange(QString& good_name, QString& beginDate, QString& endDate) 
{
    QString query = QString("CALL get_sells_change('%1', '%2', '%3', 0)")
                        .arg(good_name)
                        .arg(beginDate)
                        .arg(endDate);

    QSqlQuery q(connection);

    if (!q.exec(query)) throw std::runtime_error("Sells change calculation failed: " + q.lastError().text().toStdString());

    if (q.next()) return q.value(0).toUInt();

    return 0;
}


void Controller::addUser(QString& username, QString& password, bool rights) 
{
    QString query = "INSERT INTO users (name, password, rights) VALUES (:username, :password, :rights)";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":username", username);
    q.bindValue(":password", password);
    q.bindValue(":rights", rights);
    if (!q.exec()) throw std::runtime_error("User insertion failed: " + q.lastError().text().toStdString());
}

bool Controller::findUser(QString& username, QString& password) 
{
    QString query = "SELECT * FROM users WHERE name = :username AND password = :password";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":username", username);
    q.bindValue(":password", password);
    if (!q.exec()) throw std::runtime_error("User lookup failed: " + q.lastError().text().toStdString());
    return q.next();
}

bool Controller::findUser(QString& username) 
{
    QString query = "SELECT * FROM users WHERE name = :username";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":username", username);
    if (!q.exec()) throw std::runtime_error("User lookup failed: " + q.lastError().text().toStdString());
    return q.next();
}

bool Controller::getUserRights(QString& username) 
{
    QString query = "SELECT rights FROM users WHERE name = :username";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":username", username);
    if (!q.exec()) throw std::runtime_error("Rights lookup failed: " + q.lastError().text().toStdString());
    if (q.next()) return q.value(0).toBool();
    return false;
}

void Controller::addSale(size_t good_id, size_t amount, QString& date) 
{
    QString query = "INSERT INTO sales (good_id, good_count, create_date) VALUES (:good_id, :amount, :date)";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":good_id", good_id);
    q.bindValue(":amount", amount);
    q.bindValue(":date", date);
    if (!q.exec()) throw std::runtime_error("Sale insertion failed: " + q.lastError().text().toStdString());
}

void Controller::addGood(QString& good_name, double priority) 
{
    QString query = "INSERT INTO goods (name, priority) VALUES (:good_name, :priority)";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":good_name", good_name);
    q.bindValue(":priority", priority);
    if (!q.exec()) throw std::runtime_error("Good insertion failed: " + q.lastError().text().toStdString());
}

void Controller::modifyGoods(QString& id, QString& newValue, QString& column)
{
    QString query = "UPDATE goods SET " + column + " = :newValue WHERE id = :id";

    QSqlQuery q(connection);
    q.prepare(query);

    q.bindValue(":newValue", newValue);
    q.bindValue(":id", id);

    if (!q.exec()) throw std::runtime_error("Good modification failed: " + q.lastError().text().toStdString());
}

void Controller::modifySells(QString& id, QString& newValue, QString& column)
{
    QString query = "UPDATE sales SET " + column + " = :newValue WHERE id = :id";

    QSqlQuery q(connection);
    q.prepare(query);

    q.bindValue(":newValue", newValue);
    q.bindValue(":id", id);

    if (!q.exec()) throw std::runtime_error("Sales modification failed: " + q.lastError().text().toStdString());
}

void Controller::removeGood(size_t good_id) 
{
    QString query = "DELETE FROM goods WHERE id = :good_id";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":good_id", good_id);
    if (!q.exec()) throw std::runtime_error("Good deletion failed: " + q.lastError().text().toStdString());
}

void Controller::closeSale(size_t sale_id) 
{
    QString query = "DELETE FROM sales WHERE id = :sale_id";
    QSqlQuery q(connection);
    q.prepare(query);
    q.bindValue(":sale_id", sale_id);
    if (!q.exec()) throw std::runtime_error("Sale deletion failed: " + q.lastError().text().toStdString());
}

