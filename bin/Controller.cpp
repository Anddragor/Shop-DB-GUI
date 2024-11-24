#include "Controller.h"

#include <codecvt>
#include <iconv.h>
#include <locale>

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

pqxx::result Controller::getWarehouseStatistic(size_t warehouse_number)
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


pqxx::result Controller::getMostPopularGoods()
{
    auto quary = "SELECT * FROM most_popular_goods;";

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();

    return res;
}

std::string Controller::addSale(std::string good_name, size_t amount, std::string date)
{
    good_name = to_utf8(good_name);
    date = to_utf8(date);

    std::string result = "All good";

    auto query = "CALL sell_goods($1, $2, $3)";

    pqxx::work tx(*connection);
    tx.exec_params(query, good_name, std::to_string(amount), date);
    tx.commit();

    return result;
}

std::string Controller::to_utf8(const std::string& str)
{
    iconv_t cd = iconv_open("UTF-8", "WINDOWS-1251");
    if (cd == (iconv_t)(-1)) {
        throw std::runtime_error("iconv_open failed");
    }

    size_t in_len = str.size();
    size_t out_len = in_len * 2;
    char* in_buf = const_cast<char*>(str.c_str());
    char* out_buf = new char[out_len];
    char* out_ptr = out_buf;

    if (iconv(cd, &in_buf, &in_len, &out_ptr, &out_len) == (size_t)(-1)) {
        iconv_close(cd);
        delete[] out_buf;
        throw std::runtime_error("iconv failed");
    }

    std::string result(out_buf, out_ptr - out_buf);
    delete[] out_buf;
    iconv_close(cd);

    return result;
}

std::string Controller::addGood(std::string good_name, double priority)
{
    good_name = to_utf8(good_name);

    std::string result = "All good";

    auto query = "INSERT INTO goods (name, priority) VALUES ($1, $2)";

    pqxx::work tx(*connection);
    tx.exec_params(query, good_name, priority);
    tx.commit();

    return result;
}

void Controller::addUser(std::string username, std::string password, bool rights)
{
    auto query = "INSERT INTO users (name, password, rights) VALUES ($1, $2, $3)";

    username = to_utf8(username);
    password = to_utf8(password);

    pqxx::work tx(*connection);
    tx.exec_params(query, username, password, rights);
    tx.commit();
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

pqxx::result Controller::getGraphTable(std::string good_name)
{
    auto query = "SELECT * FROM get_graph_table($1)";

    good_name = to_utf8(good_name);

    pqxx::work tx(*connection);
    auto res = tx.exec_params(query, good_name);
    tx.commit();

    return res;
}

bool Controller::findUser(std::string username, std::string password)
{
    auto query = "SELECT * FROM users WHERE name = $1 AND password = $2";

    username = to_utf8(username);
    password = to_utf8(password);

    pqxx::work tx(*connection);
    auto res = tx.exec_params(query, username, password);
    tx.commit();

    return !res.empty();
}

bool Controller::getUserRights(std::string username)
{
    auto query = "SELECT rights FROM users WHERE name = $1";

    username = to_utf8(username);

    pqxx::work tx(*connection);
    auto res = tx.exec_params(query, username);
    tx.commit();

    return res[0][0].as<bool>();
}

size_t Controller::getSellsChange(std::string good_name, std::string beginDate, std::string endDate)
{
    auto query = "CALL get_sells_change($1, $2, $3, $4)";

    good_name = to_utf8(good_name);
    beginDate = to_utf8(beginDate);
    endDate = to_utf8(endDate);

    pqxx::work tx(*connection);
    auto res = tx.exec_params(query, good_name, beginDate, endDate, 0);
    tx.commit();

    return res[0][0].as<size_t>();
}

void Controller::removeGood(size_t good_id)
{
    auto quary = "DELETE FROM goods WHERE id = " + std::to_string(good_id);

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();
}

void Controller::closeSale(size_t sale_id)
{
    auto quary = "DELETE FROM sales WHERE id = " + std::to_string(sale_id);

    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(quary);
    tx.commit();
}

Controller::~Controller()
{
    delete connection;
}