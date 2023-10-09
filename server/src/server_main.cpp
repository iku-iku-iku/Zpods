#include <mysql++/mysql++.h>
#include <iostream>

bool open_or_create_table(mysqlpp::Connection& conn, const char* table_name) {
    mysqlpp::Query query = conn.query();

    // 检查表是否已存在
    query << "SHOW TABLES LIKE " << mysqlpp::quote << table_name;
    mysqlpp::StoreQueryResult res = query.store();
    if (res.num_rows() == 1) {
        std::cout << "Table " << table_name << " already exists.\n";
        return true;
    }

    // 创建表
    query.reset();
    query << "CREATE TABLE " << table_name << " (id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(50))";
    if (query.execute()) {
        std::cout << "Table " << table_name << " created.\n";
        return true;
    } else {
        std::cerr << "Failed to create table " << table_name << "\n";
        return false;
    }
}

int main() {
    const char* db_name = "test";
    const char* server = "localhost";
    const char* user = "root";
    const char* password = "123456";  // Replace with your MySQL password

    mysqlpp::Connection conn(false);
    if (conn.connect(db_name, server, user, password)) {
        const char* table_name = "example_table";
        open_or_create_table(conn, table_name);
    } else {
        std::cerr << "Failed to connect to database.\n";
    }

    return 0;
}
