#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <windows.h>

#include "Controller.h"
#include "UserManager.h"

int main()
{
    system("chcp 65001>nul");
    Controller cont;
    try
    {
        std::string result;
        
        UserManager test;
        
        try 
        {
            std::cout << test.signIn("TEST", "test_pass").getRights();

            test.signIn("TE4ST", "test_pass");
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << "\n";
        }

        //auto res = cont.addGood("Креветки", 8.6);
        //std::cout << res;
        //for (auto const &row : res)
        //{
        //    for (auto const& field : row) std::cout << field.c_str() << " ";
        //    std::cout << "\n";
        //}
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}