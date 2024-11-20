#pragma once

#include <string>
#include <pqxx/pqxx>

class Controller {
private:
    pqxx::connection *connection;

    std::string to_utf8(const std::string& str);

    pqxx::result getWarehouseTables();
    pqxx::result mergeTables(pqxx::result& tables, std::string merge_base, std::string merging_column);
public:
    Controller();
    pqxx::result getGoods();
    pqxx::result getSales();
    pqxx::result getMostPopularGoods();
    
    pqxx::result getWarehouseStatistic(int warehouse_number);
    pqxx::result getMergedWarehouses();

    std::string addSale(std::string good_name, int amount, std::string date);
    std::string addGood(std::string good_name, double priority);

    void removeGood(int good_id);
    void closeSale(int sale_id);
};
