
#include <iostream>
#include <pqxx/pqxx>


#include <iostream>
#include <pqxx/pqxx>

class ClientDB {
private:
    pqxx::connection conn;

public:
    ClientDB(const std::string& conn_str) : conn(conn_str) {}

    void create_tables() {
        pqxx::work txn(conn);
        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS clients (
                id SERIAL PRIMARY KEY,
                first_name TEXT NOT NULL,
                last_name TEXT NOT NULL,
                email TEXT
            );
            CREATE TABLE IF NOT EXISTS phones (
                id SERIAL PRIMARY KEY,
                client_id INTEGER REFERENCES clients(id) ON DELETE CASCADE,
                phone_number TEXT
            );
        )");
        txn.commit();
        std::cout << "Tables created.\n";
    }

    int add_client(const std::string& first_name, const std::string& last_name, const std::string& email) {
        pqxx::work txn(conn);
        pqxx::result r = txn.exec_params(
            "INSERT INTO clients (first_name, last_name, email) VALUES ($1, $2, $3) RETURNING id",
            first_name, last_name, email
        );
        txn.commit();
        std::cout << "Client added with ID: " << r[0][0].as<int>() << "\n";
        return r[0][0].as<int>();
    }

    void add_phone(int client_id, const std::string& phone) {
        pqxx::work txn(conn);
        txn.exec_params(
            "INSERT INTO phones (client_id, phone_number) VALUES ($1, $2)",
            client_id, phone
        );
        txn.commit();
        std::cout << "Phone added.\n";
    }

    void update_client(int client_id, const std::string& first_name, const std::string& last_name, const std::string& email) {
        pqxx::work txn(conn);
        txn.exec_params(
            "UPDATE clients SET first_name=$1, last_name=$2, email=$3 WHERE id=$4",
            first_name, last_name, email, client_id
        );
        txn.commit();
        std::cout << "Client updated.\n";
    }

    void delete_phone(int phone_id) {
        pqxx::work txn(conn);
        txn.exec_params("DELETE FROM phones WHERE id=$1", phone_id);
        txn.commit();
        std::cout << "Phone deleted.\n";
    }

    void delete_client(int client_id) {
        pqxx::work txn(conn);
        txn.exec_params("DELETE FROM clients WHERE id=$1", client_id);
        txn.commit();
        std::cout << "Client deleted.\n";
    }

    void find_client(const std::string& keyword) {
        pqxx::work txn(conn);
        pqxx::result r = txn.exec_params(R"(
            SELECT c.id, c.first_name, c.last_name, c.email, p.phone_number
            FROM clients c
            LEFT JOIN phones p ON c.id = p.client_id
            WHERE c.first_name ILIKE $1 OR c.last_name ILIKE $1 OR c.email ILIKE $1 OR p.phone_number ILIKE $1
        )", "%" + keyword + "%");

        for (auto row : r) {
            std::cout << "ID: " << row["id"].as<int>()
                << ", Name: " << row["first_name"].as<std::string>()
                << " " << row["last_name"].as<std::string>()
                << ", Email: " << row["email"].as<std::string>()
                << ", Phone: " << row["phone_number"].as<std::string>()
                << "\n";
        }
    }
};

int main() {
    try {
        ClientDB db("dbname=dz4 user=postgres password=1 host=localhost");

        db.create_tables();

        int id = db.add_client("Ivan", "Ivanov", "ivan@example.com");
        db.add_phone(id, "+123456789");
        db.add_phone(id, "+987654321");

        db.update_client(id, "Ivan", "Petrov", "ivan.petrov@example.com");

        std::cout << "\n-- Найти по 'Ivan' --\n";
        db.find_client("Ivan");

        db.delete_phone(1); // пример: удаляет телефон с id=1
        db.delete_client(id);

    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}