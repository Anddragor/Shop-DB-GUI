#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>

class Controller {
private:
    QSqlDatabase connection;
public:
    Controller();
    ~Controller();

    QSqlQuery getGoods();
    QSqlQuery getSales();
    QSqlQuery getMostPopularGoods();

    QSqlQuery getWarehouseStatistic(size_t warehouse_number);
    QSqlQuery getGraphTable(const QString& good_name);
    size_t getSellsChange(QString& good_name, QString& beginDate, QString& endDate);

    void addUser(QString& username, QString& password, bool rights);
    bool findUser(QString& username, QString& password);
    bool findUser(QString& username);
    bool getUserRights(QString& username);

    void addSale(size_t good_name, size_t amount, QString& date);
    void addGood(QString& good_name, double priority);

    void modifyGoods(QString& id, QString& newValue, QString& column);
    void modifySells(QString& id, QString& newValue, QString& column);

    void removeGood(size_t good_id);
    void closeSale(size_t sale_id);
};

#endif // CONTROLLER_H
