
#include "mySqlite.h"

int main(void)
{
    flight_query::my_sqlite db(std::string("../all_data.db"));
    int ret = 0;
    ret = db.open_db();
    if (ret == flight_query::FLIGHT_SUCCESS)
    {
        std::cout << "open success" << std::endl;
    }
    else
    {
        std::cout << "open failed" << std::endl;
        return -1;
    }

    ret = db.exec_query(std::string("select flightNo,departure,arrival from flight_data where carrier = 'CA';"));

    if (ret == flight_query::FLIGHT_SUCCESS)
    {
        std::cout << "query success" << std::endl;
    }
    else
    {
        std::cout << "query failed" << std::endl;
        return -2;
    }
    std::vector<std::vector<std::string>> result = db.get_result();
    return 0;
}