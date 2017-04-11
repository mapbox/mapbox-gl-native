#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <memory>

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
    enum Code : int {
        OK = 0,
        CANTOPEN = 14,
        NOTADB = 26
    };

    Exception(int err, const char *msg) : std::runtime_error(msg), code(err) {}
    Exception(int err, const std::string& msg) : std::runtime_error(msg), code(err) {}
    const int code = OK;
};

class DatabaseImpl;
class Statement;
class StatementImpl;

class Database {
private:
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

public:
    Database(const std::string &filename, int flags = 0);
    Database(Database &&);
    ~Database();
    Database &operator=(Database &&);

    void setBusyTimeout(std::chrono::milliseconds);
    void exec(const std::string &sql);
    Statement prepare(const char *query);

private:
    std::unique_ptr<DatabaseImpl> impl;

    friend class Statement;
};

class Statement {
private:
    Statement(const Statement &) = delete;
    Statement &operator=(const Statement &) = delete;

public:
    Statement(Database *db, const char *sql);
    Statement(Statement &&);
    ~Statement();
    Statement &operator=(Statement &&);

    template <typename T> void bind(int offset, T value);

    // Text
    void bind(int offset, const char *, std::size_t length, bool retain = true);
    void bind(int offset, const std::string&, bool retain = true);

    // Blob
    void bindBlob(int offset, const void *, std::size_t length, bool retain = true);
    void bindBlob(int offset, const std::vector<uint8_t>&, bool retain = true);

    template <typename T> T get(int offset);

    bool run();
    void reset();
    void clearBindings();

    int64_t lastInsertRowId() const;
    uint64_t changes() const;

private:
    std::unique_ptr<StatementImpl> impl;
};

class Transaction {
private:
    Transaction(const Transaction&) = delete;
    Transaction(Transaction&&) = delete;
    Transaction& operator=(const Transaction&) = delete;

public:
    enum Mode {
        Deferred,
        Immediate,
        Exclusive
    };

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
