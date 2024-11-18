#include "Controller.h"

Controller::Controller() {
    std::string HOST = "localhost",
        PORT = "5432",
        DB_NAME = "postgres",
        USER = "postgres",
        PASSWORD = "1111";

    std::string url = "host=" + HOST + " port=" + PORT + " dbname=" + DB_NAME +
        " user=" + USER + " password=" + PASSWORD;

    
    this->connection = new pqxx::connection(url);
};

pqxx::result Controller::getGoods() {
    auto quary = "SELECT * FROM goods";

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();

    return res;
};

pqxx::result Controller::getSales() 
{
    auto quary = "SELECT * FROM sales";

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();

    return res;
};

pqxx::result Controller::getWarehouseStatistic(int warehouse_number)
{
    auto quary = "SELECT * FROM warehouse" + std::to_string(warehouse_number);

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();

    return res;
}

pqxx::result Controller::getWarehouseTables()
{
    auto quary = "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public' AND table_type = 'BASE TABLE' AND table_name LIKE '%warehouse%'";

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();

    return res;
}

std::string Controller::addSale(std::string good_name, int amount, std::string date, std::string result)
{
    auto quary = "CALL sell_goods('" + good_name + "' , " + std::to_string(amount) + " , '" + date + "' , ' " + result + " ')";

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();

    return result;
}

pqxx::result Controller::mergeTables(pqxx::result& tables, std::string merge_base, std::string merging_column)
{
    if (tables.size() < 2) throw std::exception("Not enough tables to merge...\n");

    std::string quary = "SELECT " + tables[0][0].as<std::string>() + "." + merge_base + ", ";

    for (auto it = tables.begin(); it != tables.end(); it++)
    {
        if (it != tables.begin()) quary += " + ";
        quary += " COALESCE(SUM(" + (*it)[0].as<std::string>() + "." + merging_column + "), 0)";
    }

    quary += " AS total FROM " + tables[0][0].as<std::string>() + " FULL OUTER JOIN ";

    for (auto it = std::next(tables.begin()); it != tables.end(); ++it) {
        quary += (*it)[0].as<std::string>() + " ON ";
        quary += tables[0][0].as<std::string>() + "." + merge_base + " = " + (*it)[0].as<std::string>() + "." + merge_base;
    }

    quary += " WHERE warehouse1.good_id IS NOT NULL GROUP BY warehouse1.good_id ";
    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();

    return res;
}

pqxx::result Controller::getMergedWarehouses()
{
    auto tables = this->getWarehouseTables();
    auto res = mergeTables(tables, "good_id", "good_count");

    return res;
}

void Controller::removeGood(int good_id)
{
    auto quary = "DELETE FROM goods WHERE id = " + std::to_string(good_id);

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();
}

void Controller::closeSale(int sale_id)
{
    auto quary = "DELETE FROM sales WHERE id = " + std::to_string(sale_id);

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();
}