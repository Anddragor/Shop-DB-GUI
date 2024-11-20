#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <windows.h>

#include "Controller.h"

int main()
{
    system("chcp 65001>nul");
    Controller cont;
    try
    {
        std::string result;
        auto res = cont.getMostPopularGoods();

        //auto res = cont.addGood("Креветки", 8.6);
        //std::cout << res;
        for (auto const &row : res)
        {
            for (auto const& field : row) std::cout << field.c_str() << " ";
            std::cout << "\n";
        }
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}