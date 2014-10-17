#pragma once

#include <string>
#include <stdexcept>

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
    inline Exception(int err, const char *msg) : std::runtime_error(msg), code(err) {}
    const int code = 0;
};

class Statement;

class Database {
private:
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

public:
    Database(const std::string &filename, int flags = 0);
    Database(Database &&);
    ~Database();
    Database &operator=(Database &&);

    operator bool() const;

    void exec(const std::string &sql);
    Statement prepare(const char *query);

private:
    sqlite3 *db = nullptr;
};

class Statement {
private:
    Statement(const Statement &) = delete;
    Statement &operator=(const Statement &) = delete;

public:
    Statement(sqlite3 *db, const char *sql);
    Statement(Statement &&);
    ~Statement();
    Statement &operator=(Statement &&);

    operator bool() const;

    template <typename T> void bind(int offset, T value);
    void bind(int offset, const std::string &value, bool retain = true);
    template <typename T> T get(int offset);

    bool run();
    void reset();

private:
    sqlite3_stmt *stmt = nullptr;
};

}
}
