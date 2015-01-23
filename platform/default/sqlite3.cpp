#include "sqlite3.hpp"
#include <sqlite3.h>

#include <cassert>

// Check sqlite3 library version.
const static bool sqliteVersionCheck = []() {
    if (sqlite3_libversion_number() != SQLITE_VERSION_NUMBER) {
        char message[96];
        snprintf(message, 96,
                 "sqlite3 libversion mismatch: headers report %d, but library reports %d",
                 SQLITE_VERSION_NUMBER, sqlite3_libversion_number());
        throw std::runtime_error(message);
    }
    if (strcmp(sqlite3_sourceid(), SQLITE_SOURCE_ID) != 0) {
        char message[256];
        snprintf(message, 256,
                 "sqlite3 sourceid mismatch: headers report \"%s\", but library reports \"%s\"",
                 SQLITE_SOURCE_ID, sqlite3_sourceid());
        throw std::runtime_error(message);
    }

    return true;
}();

namespace mapbox {
namespace sqlite {

Database::Database(const std::string &filename, int flags) {
    const int err = sqlite3_open_v2(filename.c_str(), &db, flags, nullptr);
    if (err != SQLITE_OK) {
        Exception ex { err, sqlite3_errmsg(db) };
        db = nullptr;
        throw ex;
    }
}

Database::Database(Database &&other)
    : db(std::move(other.db)) {}

Database &Database::operator=(Database &&other) {
    std::swap(db, other.db);
    return *this;
}

Database::~Database() {
    if (db) {
        const int err = sqlite3_close(db);
        if (err != SQLITE_OK) {
            throw Exception { err, sqlite3_errmsg(db) };
        }
    }
}

Database::operator bool() const {
    return db != nullptr;
}

void Database::exec(const std::string &sql) {
    assert(db);
    char *msg = nullptr;
    const int err = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &msg);
    if (msg) {
        Exception ex { err, msg };
        sqlite3_free(msg);
        throw ex;
    } else if (err != SQLITE_OK) {
        throw Exception { err, sqlite3_errmsg(db) };
    }
}

Statement Database::prepare(const char *query) {
    assert(db);
    return std::move(Statement(db, query));
}

Statement::Statement(sqlite3 *db, const char *sql) {
    const int err = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (err != SQLITE_OK) {
        stmt = nullptr;
        throw Exception { err, sqlite3_errmsg(db) };
    }
}

#define CHECK_SQLITE_OK(err) \
    if (err != SQLITE_OK) { \
        throw Exception { err, sqlite3_errmsg(sqlite3_db_handle(stmt)) }; \
    }

Statement::Statement(Statement &&other) {
    *this = std::move(other);
}

Statement &Statement::operator=(Statement &&other) {
    std::swap(stmt, other.stmt);
    return *this;
}

Statement::~Statement() {
    if (stmt) {
        const int err = sqlite3_finalize(stmt);
        CHECK_SQLITE_OK(err)
    }
}

Statement::operator bool() const {
    return stmt != nullptr;
}

#define BIND_3(type, value) \
    assert(stmt); \
    const int err = sqlite3_bind_##type(stmt, offset, value); \
    CHECK_SQLITE_OK(err)

#define BIND_5(type, value, length, param) \
    assert(stmt); \
    const int err = sqlite3_bind_##type(stmt, offset, value, length, param); \
    CHECK_SQLITE_OK(err)

template <> void Statement::bind(int offset, int value) {
    BIND_3(int, value)
}

template <> void Statement::bind(int offset, int64_t value) {
    BIND_3(int64, value)
}

template <> void Statement::bind(int offset, double value) {
    BIND_3(double, value)
}

template <> void Statement::bind(int offset, bool value) {
    BIND_3(int, value)
}

template <> void Statement::bind(int offset, const char *value) {
    BIND_5(text, value, -1, nullptr)
}

void Statement::bind(int offset, const std::string &value, bool retain) {
    BIND_5(blob, value.data(), int(value.size()), retain ? SQLITE_TRANSIENT : SQLITE_STATIC)
}

bool Statement::run() {
    assert(stmt);
    const int err = sqlite3_step(stmt);
    if (err == SQLITE_DONE) {
        return false;
    } else if (err == SQLITE_ROW) {
        return true;
    } else {
        throw std::runtime_error("failed to run statement");
    }
}

template <> int Statement::get(int offset) {
    assert(stmt);
    return sqlite3_column_int(stmt, offset);
}

template <> int64_t Statement::get(int offset) {
    assert(stmt);
    return sqlite3_column_int64(stmt, offset);
}

template <> double Statement::get(int offset) {
    assert(stmt);
    return sqlite3_column_double(stmt, offset);
}

template <> std::string Statement::get(int offset) {
    assert(stmt);
    return {
        reinterpret_cast<const char *>(sqlite3_column_blob(stmt, offset)),
        size_t(sqlite3_column_bytes(stmt, offset))
    };
}

void Statement::reset() {
    assert(stmt);
    sqlite3_reset(stmt);
}

}
}
