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

enum class ResultCode : int {
    OK = 0,
    Error = 1,
    Internal = 2,
    Perm = 3,
    Abort = 4,
    Busy = 5,
    Locked = 6,
    NoMem = 7,
    ReadOnly = 8,
    Interrupt = 9,
    IOErr = 10,
    Corrupt = 11,
    NotFound = 12,
    Full = 13,
    CantOpen = 14,
    Protocol = 15,
    Schema = 17,
    TooBig = 18,
    Constraint = 19,
    Mismatch = 20,
    Misuse = 21,
    NoLFS = 22,
    Auth = 23,
    Range = 25,
    NotADB = 26
};

class Exception : public std::runtime_error {
public:
    Exception(int err, const char* msg)
        : std::runtime_error(msg), code(static_cast<ResultCode>(err)) {
    }
    Exception(ResultCode err, const char* msg)
        : std::runtime_error(msg), code(err) {
    }
    Exception(int err, const std::string& msg)
        : std::runtime_error(msg), code(static_cast<ResultCode>(err)) {
    }
    Exception(ResultCode err, const std::string& msg)
        : std::runtime_error(msg), code(err) {
    }
    const ResultCode code = ResultCode::OK;
};

class DatabaseImpl;
class Statement;
class StatementImpl;
class Query;

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

private:
    std::unique_ptr<DatabaseImpl> impl;

    friend class Statement;
};

// A Statement object represents a prepared statement that can be run repeatedly run with a Query object.
class Statement {
public:
    Statement(Database& db, const char* sql);
    Statement(const Statement&) = delete;
    Statement(Statement&&) = delete;
    Statement& operator=(const Statement&) = delete;
    Statement& operator=(Statement&&) = delete;
    ~Statement();

    friend class Query;

private:
    std::unique_ptr<StatementImpl> impl;

#ifndef NDEBUG
    // This flag stores whether there exists a Query object that uses this prepared statement.
    // There may only be one Query object at a time. Statement objects must outlive Query objects.
    // While a Query object exists, a Statement object may not be moved or deleted.
    bool used = false;
#endif
};

// A Query object is used to run a database query with a prepared statement (stored in a Statement
// object). There may only exist one Query object per Statement object. Query objects are designed
// to be constructed and destroyed frequently.
class Query {
public:
    Query(Statement&);
    Query(const Query&) = delete;
    Query(Query&&) = delete;
    Query& operator=(const Query&) = delete;
    Query& operator=(Query&&) = delete;
    ~Query();

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

    int64_t lastInsertRowId() const;
    uint64_t changes() const;

private:
    Statement& stmt;
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
