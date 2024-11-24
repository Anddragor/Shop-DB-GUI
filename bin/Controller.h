#pragma once

#include <string>
#include <pqxx/pqxx>

class Controller 
{
private:
    pqxx::connection *connection;

    std::string to_utf8(const std::string& str);

    pqxx::result getWarehouseTables();
    pqxx::result mergeTables(pqxx::result& tables, std::string merge_base, std::string merging_column);
public:
    Controller();
    ~Controller();
    pqxx::result getGoods();
    pqxx::result getSales();
    pqxx::result getMostPopularGoods();
    
    pqxx::result getWarehouseStatistic(size_t warehouse_number);
    pqxx::result getMergedWarehouses();
    pqxx::result getGraphTable(std::string good_name);

    bool findUser(std::string username, std::string password);
    bool getUserRights(std::string username);

    size_t getSellsChange(std::string good_name, std::string beginDate, std::string endDate);

    std::string addSale(std::string good_name, size_t amount, std::string date);
    std::string addGood(std::string good_name, double priority);

    void addUser(std::string username, std::string password, bool rights);

    void removeGood(size_t good_id);
    void closeSale(size_t sale_id);


};
