#include "mySqlite.h"

flight_query::my_sqlite::my_sqlite(std::string path) : m_db_path(path)
{
}

flight_query::my_sqlite::~my_sqlite()
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = NULL;
    }
}

int flight_query::my_sqlite::open_db()
{
    if (!sqlite3_open(m_db_path.c_str(), &m_db))
    {
        return FLIGHT_SUCCESS;
    }
    else
    {
        return OPEN_DB_FAILED;
    }
}

int flight_query::my_sqlite::exec_query(std::string sql)
{
    // 语句检查——合法
    if (sqlite3_prepare_v2(m_db, sql.c_str(), sql.length(), &m_stmt, NULL) == SQLITE_OK)
    {

        int result = 0;
        int counts = 0;
        int count_col = sqlite3_column_count(m_stmt); // 获取列数

        // 如果返回SQLITE_ROW则，进行多次执行
        for (result = sqlite3_step(m_stmt); result == SQLITE_ROW;
             result = sqlite3_step(m_stmt))
        {
            // 获取数据

            std::string tmm;
            // 将每条数据插入vector
            for (int i = 0; i < count_col; i++)
            {

                m_tmp.push_back((char *)sqlite3_column_text(m_stmt, i));
            }
            m_data_array.push_back(m_tmp);
            m_tmp.clear();
            counts++;
        }

        if (result == SQLITE_DONE)
        {
            sqlite3_finalize(m_stmt); // 清理语句句柄，准备下一个语句
            return FLIGHT_SUCCESS;
        }
        else
        {
            sqlite3_finalize(m_stmt); // 清理语句句柄，准备下一个语句
            return SQL_EXEC_FAILED;
        }
    }
    else
    { // 不合法
        // sqlite3_errcode(m_db),
        // sqlite3_errmsg(m_db);

        sqlite3_finalize(m_stmt); // 清理语句句柄，准备下一个语句
        return SQL_WORDS_ERROR;
    }
}

void flight_query::my_sqlite::clean_buffer()
{
    m_data_array.clear();
    m_tmp.clear();
}

std::vector<std::vector<std::string>> flight_query::my_sqlite::get_result()
{

    return m_data_array;
}