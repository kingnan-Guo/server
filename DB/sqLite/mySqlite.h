#pragma once
#include <iostream>
#include <unistd.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <stdlib.h>
// #include <crow.h>
#include <ctime>
#include <cstdio>

namespace flight_query {
 
    const int32_t OPEN_DB_FAILED = -8001; //数据库打开失败
    const int32_t SQL_WORDS_ERROR = -8002; //SQL语句不合法
    const int32_t SQL_EXEC_FAILED = -8003; //SQL语句执行错误
    const int32_t FLIGHT_SUCCESS = 1;  //结果正确都返回这个


    class my_sqlite {
    
        public:
            my_sqlite(std::string path);
            ~my_sqlite();
        
        public:
            int open_db();      //打开数据库
            int exec_query(std::string sql); //执行查询语句
            void clean_buffer();    //清理结果数组
            std::vector<std::vector<std::string> > get_result(); //获取结果集
                
        private:
            sqlite3* m_db;
            std::string m_db_path;
            sqlite3_stmt* m_stmt;
            std::vector<std::vector<std::string> > m_data_array;
            std::vector<std::string> m_tmp;//存储到m_data_array中的中间变量
    };


}

