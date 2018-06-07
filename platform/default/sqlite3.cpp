#include "sqlite3.hpp"
#include <sqlite3.h>

#include <cassert>
#include <cstring>
#include <cstdio>
#include <chrono>
#include <experimental/optional>

#include <mbgl/util/logging.hpp>

namespace mapbox {
namespace sqlite {

class DatabaseImpl {
public:
    DatabaseImpl(sqlite3* db_)
        : db(db_)
    {
    }

    ~DatabaseImpl()
    {
        const int error = sqlite3_close(db);
        if (error != SQLITE_OK) {
            mbgl::Log::Error(mbgl::Event::Database, "%s (Code %i)", sqlite3_errmsg(db), error);
        }
    }

    void setBusyTimeout(std::chrono::milliseconds timeout);
    void exec(const std::string& sql);

    sqlite3* db;
};

class StatementImpl {
public:
    StatementImpl(sqlite3* db, const char* sql)
    {
        const int error = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (error != SQLITE_OK) {
            stmt = nullptr;
            throw Exception { error, sqlite3_errmsg(db) };
        }
    }

    ~StatementImpl()
    {
        if (!stmt) return;

        sqlite3_finalize(stmt);
    }

    void check(int err) {
        if (err != SQLITE_OK) {
            throw Exception { err, sqlite3_errmsg(sqlite3_db_handle(stmt)) };
        }
    }

    sqlite3_stmt* stmt = nullptr;
    int64_t lastInsertRowId = 0;
    int64_t changes = 0;
};

template <typename T>
using optional = std::experimental::optional<T>;

static void errorLogCallback(void *, const int err, const char *msg) {
    mbgl::Log::Record(mbgl::EventSeverity::Info, mbgl::Event::Database, err, "%s", msg);
}

const static bool sqliteVersionCheck __attribute__((unused)) = []() {
    if (sqlite3_libversion_number() / 1000000 != SQLITE_VERSION_NUMBER / 1000000) {
        char message[96];
        snprintf(message, 96,
                 "sqlite3 libversion mismatch: headers report %d, but library reports %d",
                 SQLITE_VERSION_NUMBER, sqlite3_libversion_number());
        throw std::runtime_error(message);
    }

    // Enable SQLite logging before initializing the database.
    sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

    return true;
}();

mapbox::util::variant<Database, Exception> Database::tryOpen(const std::string &filename, int flags) {
    sqlite3* db = nullptr;
    const int error = sqlite3_open_v2(filename.c_str(), &db, flags, nullptr);
    if (error != SQLITE_OK) {
        const auto message = sqlite3_errmsg(db);
        return Exception { error, message };
    }
    return Database(std::make_unique<DatabaseImpl>(db));
}

Database Database::open(const std::string &filename, int flags) {
    auto result = tryOpen(filename, flags);
    if (result.is<Exception>()) {
        throw result.get<Exception>();
    } else {
        return std::move(result.get<Database>());
    }
}

Database::Database(std::unique_ptr<DatabaseImpl> impl_)
    : impl(std::move(impl_))
{}

Database::Database(Database &&other)
    : impl(std::move(other.impl)) {}

Database &Database::operator=(Database &&other) {
    std::swap(impl, other.impl);
    return *this;
}

Database::~Database() = default;

void Database::setBusyTimeout(std::chrono::milliseconds timeout) {
    assert(impl);
    impl->setBusyTimeout(timeout);
}

void DatabaseImpl::setBusyTimeout(std::chrono::milliseconds timeout) {
    const int err = sqlite3_busy_timeout(db,
        int(std::min<std::chrono::milliseconds::rep>(timeout.count(), std::numeric_limits<int>::max())));
    if (err != SQLITE_OK) {
        throw Exception { err, sqlite3_errmsg(db) };
    }
}

void Database::exec(const std::string &sql) {
    assert(impl);
    impl->exec(sql);
}

void DatabaseImpl::exec(const std::string& sql) {
    char *msg = nullptr;
    const int err = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &msg);
    if (msg) {
        const std::string message = msg;
        sqlite3_free(msg);
        throw Exception { err, message };
    } else if (err != SQLITE_OK) {
        throw Exception { err, sqlite3_errmsg(db) };
    }
}

Statement::Statement(Database& db, const char* sql)
    : impl(std::make_unique<StatementImpl>(db.impl->db, sql)) {
}

Statement::~Statement() {
#ifndef NDEBUG
    // Crash if we're destructing this object while we know a Query object references this.
    assert(!used);
#endif
}

Query::Query(Statement& stmt_) : stmt(stmt_) {
    assert(stmt.impl);

#ifndef NDEBUG
    assert(!stmt.used);
    stmt.used = true;
#endif
}

Query::~Query() {
    reset();
    clearBindings();

#ifndef NDEBUG
    stmt.used = false;
#endif
}

template <> void Query::bind(int offset, std::nullptr_t) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_null(stmt.impl->stmt, offset));
}

template <> void Query::bind(int offset, int8_t value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int64(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, int16_t value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int64(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, int32_t value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int64(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, int64_t value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int64(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, uint8_t value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int64(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, uint16_t value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int64(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, uint32_t value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int64(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, float value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_double(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, double value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_double(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, bool value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int(stmt.impl->stmt, offset, value));
}

template <> void Query::bind(int offset, const char *value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_text(stmt.impl->stmt, offset, value, -1, SQLITE_STATIC));
}

// We currently cannot use sqlite3_bind_blob64 / sqlite3_bind_text64 because they
// were introduced in SQLite 3.8.7, and we need to support earlier versions:
//    Android 11: 3.7
//    Android 21: 3.8
//    Android 24: 3.9
// Per https://developer.android.com/reference/android/database/sqlite/package-summary.
// The first iOS version with 3.8.7+ was 9.0, with 3.8.8.

void Query::bind(int offset, const char * value, std::size_t length, bool retain) {
    assert(stmt.impl);
    if (length > std::numeric_limits<int>::max()) {
        throw std::range_error("value too long for sqlite3_bind_text");
    }
    stmt.impl->check(sqlite3_bind_text(stmt.impl->stmt, offset, value, int(length),
                            retain ? SQLITE_TRANSIENT : SQLITE_STATIC));
}

void Query::bind(int offset, const std::string& value, bool retain) {
    bind(offset, value.data(), value.size(), retain);
}

void Query::bindBlob(int offset, const void * value, std::size_t length, bool retain) {
    assert(stmt.impl);
    if (length > std::numeric_limits<int>::max()) {
        throw std::range_error("value too long for sqlite3_bind_text");
    }
    stmt.impl->check(sqlite3_bind_blob(stmt.impl->stmt, offset, value, int(length),
                            retain ? SQLITE_TRANSIENT : SQLITE_STATIC));
}

void Query::bindBlob(int offset, const std::vector<uint8_t>& value, bool retain) {
    bindBlob(offset, value.data(), value.size(), retain);
}

template <>
void Query::bind(
        int offset, std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> value) {
    assert(stmt.impl);
    stmt.impl->check(sqlite3_bind_int64(stmt.impl->stmt, offset, std::chrono::system_clock::to_time_t(value)));
}

template <> void Query::bind(int offset, optional<std::string> value) {
    if (!value) {
        bind(offset, nullptr);
    } else {
        bind(offset, *value);
    }
}

template <>
void Query::bind(
    int offset,
    optional<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>> value) {
    if (!value) {
        bind(offset, nullptr);
    } else {
        bind(offset, *value);
    }
}

bool Query::run() {
    assert(stmt.impl);
    const int err = sqlite3_step(stmt.impl->stmt);
    stmt.impl->lastInsertRowId = sqlite3_last_insert_rowid(sqlite3_db_handle(stmt.impl->stmt));
    stmt.impl->changes = sqlite3_changes(sqlite3_db_handle(stmt.impl->stmt));
    if (err == SQLITE_DONE) {
        return false;
    } else if (err == SQLITE_ROW) {
        return true;
    } else if (err != SQLITE_OK) {
        throw Exception { err, sqlite3_errmsg(sqlite3_db_handle(stmt.impl->stmt)) };
    } else {
        return false;
    }
}

template <> bool Query::get(int offset) {
    assert(stmt.impl);
    return sqlite3_column_int(stmt.impl->stmt, offset);
}

template <> int Query::get(int offset) {
    assert(stmt.impl);
    return sqlite3_column_int(stmt.impl->stmt, offset);
}

template <> int64_t Query::get(int offset) {
    assert(stmt.impl);
    return sqlite3_column_int64(stmt.impl->stmt, offset);
}

template <> double Query::get(int offset) {
    assert(stmt.impl);
    return sqlite3_column_double(stmt.impl->stmt, offset);
}

template <> std::string Query::get(int offset) {
    assert(stmt.impl);
    return {
        reinterpret_cast<const char *>(sqlite3_column_blob(stmt.impl->stmt, offset)),
        size_t(sqlite3_column_bytes(stmt.impl->stmt, offset))
    };
}

template <> std::vector<uint8_t> Query::get(int offset) {
    assert(stmt.impl);
    const auto* begin = reinterpret_cast<const uint8_t*>(sqlite3_column_blob(stmt.impl->stmt, offset));
    const uint8_t* end   = begin + sqlite3_column_bytes(stmt.impl->stmt, offset);
    return { begin, end };
}

template <>
std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
Query::get(int offset) {
    assert(stmt.impl);
    return std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::from_time_t(sqlite3_column_int64(stmt.impl->stmt, offset)));
}

template <> optional<int64_t> Query::get(int offset) {
    assert(stmt.impl);
    if (sqlite3_column_type(stmt.impl->stmt, offset) == SQLITE_NULL) {
        return optional<int64_t>();
    } else {
        return get<int64_t>(offset);
    }
}

template <> optional<double> Query::get(int offset) {
    assert(stmt.impl);
    if (sqlite3_column_type(stmt.impl->stmt, offset) == SQLITE_NULL) {
        return optional<double>();
    } else {
        return get<double>(offset);
    }
}

template <> optional<std::string> Query::get(int offset) {
    assert(stmt.impl);
    if (sqlite3_column_type(stmt.impl->stmt, offset) == SQLITE_NULL) {
        return optional<std::string>();
    } else {
        return get<std::string>(offset);
    }
}

template <>
optional<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>>
Query::get(int offset) {
    assert(stmt.impl);
    if (sqlite3_column_type(stmt.impl->stmt, offset) == SQLITE_NULL) {
        return {};
    } else {
        return get<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>>(
            offset);
    }
}

void Query::reset() {
    assert(stmt.impl);
    sqlite3_reset(stmt.impl->stmt);
}

void Query::clearBindings() {
    assert(stmt.impl);
    sqlite3_clear_bindings(stmt.impl->stmt);
}

int64_t Query::lastInsertRowId() const {
    assert(stmt.impl);
    return stmt.impl->lastInsertRowId;
}

uint64_t Query::changes() const {
    assert(stmt.impl);
    auto changes_ = stmt.impl->changes;
    return (changes_ < 0 ? 0 : changes_);
}

Transaction::Transaction(Database& db_, Mode mode)
    : dbImpl(*db_.impl) {
    switch (mode) {
    case Deferred:
        dbImpl.exec("BEGIN DEFERRED TRANSACTION");
        break;
    case Immediate:
        dbImpl.exec("BEGIN IMMEDIATE TRANSACTION");
        break;
    case Exclusive:
        dbImpl.exec("BEGIN EXCLUSIVE TRANSACTION");
        break;
    }
}

Transaction::~Transaction() {
    if (needRollback) {
        try {
            rollback();
        } catch (...) {
            // Ignore failed rollbacks in destructor.
        }
    }
}

void Transaction::commit() {
    needRollback = false;
    dbImpl.exec("COMMIT TRANSACTION");
}

void Transaction::rollback() {
    needRollback = false;
    dbImpl.exec("ROLLBACK TRANSACTION");
}

} // namespace sqlite
} // namespace mapbox
