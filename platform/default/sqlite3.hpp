#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

namespace mapbox {
namespace sqlite {

enum OpenFlag : int {
    ReadOnly = 0x00000001,
    ReadWrite = 0x00000002,
    Create = 0x00000004,
    NoMutex = 0x00008000,
    FullMutex = 0x00010000,
    SharedCache = 0x00020000,
    PrivateCache = 0x00040000,
};

struct Exception : std::runtime_error {
    Exception(int err, const char* msg) : std::runtime_error(msg), code(err) {
    }
    Exception(int err, const std::string& msg) : std::runtime_error(msg), code(err) {
    }
    const int code = 0;
};

class Statement;

class Database {
private:
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

public:
    Database(const std::string& filename, int flags = 0);
    Database(Database&&);
    ~Database();
    Database& operator=(Database&&);

    explicit operator bool() const;

    static void errorLogCallback(void* arg, const int err, const char* msg);
    void setBusyTimeout(std::chrono::milliseconds);
    void exec(const std::string& sql);
    Statement prepare(const char* query);

    int64_t lastInsertRowid() const;
    uint64_t changes() const;

private:
    sqlite3* db = nullptr;
};

class Statement {
private:
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    void check(int err);

public:
    Statement(sqlite3* db, const char* sql);
    Statement(Statement&&);
    ~Statement();
    Statement& operator=(Statement&&);

    explicit operator bool() const;

    template <typename T>
    void bind(int offset, T value);

    // Text
    void bind(int offset, const char*, std::size_t length, bool retain = true);
    void bind(int offset, const std::string&, bool retain = true);

    // Blob
    void bindBlob(int offset, const void*, std::size_t length, bool retain = true);
    void bindBlob(int offset, const std::vector<uint8_t>&, bool retain = true);

    template <typename T>
    T get(int offset);

    bool run();
    void reset();
    void clearBindings();

private:
    sqlite3_stmt* stmt = nullptr;
};

class Transaction {
private:
    Transaction(const Transaction&) = delete;
    Transaction(Transaction&&) = delete;
    Transaction& operator=(const Transaction&) = delete;

public:
    enum Mode { Deferred, Immediate, Exclusive };

    Transaction(Database&, Mode = Deferred);
    ~Transaction();

    void commit();
    void rollback();

private:
    Database& db;
    bool needRollback = true;
};
}
}
