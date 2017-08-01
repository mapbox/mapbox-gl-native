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
    DatabaseImpl(const char* filename, int flags)
    {
        const int error = sqlite3_open_v2(filename, &db, flags, nullptr);
        if (error != SQLITE_OK) {
            const auto message = sqlite3_errmsg(db);
            db = nullptr;
            throw Exception { error, message };
        }
    }

    ~DatabaseImpl()
    {
        if (!db) return;

        const int error = sqlite3_close(db);
        if (error != SQLITE_OK) {
            mbgl::Log::Error(mbgl::Event::Database, "%s (Code %i)", sqlite3_errmsg(db), error);
        }
    }

    sqlite3* db = nullptr;
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
    if (err == SQLITE_ERROR) {
        mbgl::Log::Error(mbgl::Event::Database, "%s (Code %i)", msg, err);
    } else if (err == SQLITE_WARNING) {
        mbgl::Log::Warning(mbgl::Event::Database, "%s (Code %i)", msg, err);
    } else {
        mbgl::Log::Info(mbgl::Event::Database, "%s (Code %i)", msg, err);
    }
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

Database::Database(const std::string &filename, int flags)
    : impl(std::make_unique<DatabaseImpl>(filename.c_str(), flags))
{
}

Database::Database(Database &&other)
    : impl(std::move(other.impl)) {}

Database &Database::operator=(Database &&other) {
    std::swap(impl, other.impl);
    return *this;
}

Database::~Database() = default;

void Database::setBusyTimeout(std::chrono::milliseconds timeout) {
    assert(impl);
    const int err = sqlite3_busy_timeout(impl->db,
        int(std::min<std::chrono::milliseconds::rep>(timeout.count(), std::numeric_limits<int>::max())));
    if (err != SQLITE_OK) {
        throw Exception { err, sqlite3_errmsg(impl->db) };
    }
}

void Database::exec(const std::string &sql) {
    assert(impl);
    char *msg = nullptr;
    const int err = sqlite3_exec(impl->db, sql.c_str(), nullptr, nullptr, &msg);
    if (msg) {
        const std::string message = msg;
        sqlite3_free(msg);
        throw Exception { err, message };
    } else if (err != SQLITE_OK) {
        throw Exception { err, sqlite3_errmsg(impl->db) };
    }
}

Statement Database::prepare(const char *query) {
    assert(impl);
    return Statement(this, query);
}

Statement::Statement(Database *db, const char *sql)
    : impl(std::make_unique<StatementImpl>(db->impl->db, sql))
{
}

Statement::Statement(Statement &&other) {
    *this = std::move(other);
}

Statement &Statement::operator=(Statement &&other) {
    std::swap(impl, other.impl);
    return *this;
}

Statement::~Statement() = default;

template <> void Statement::bind(int offset, std::nullptr_t) {
    assert(impl);
    impl->check(sqlite3_bind_null(impl->stmt, offset));
}

template <> void Statement::bind(int offset, int8_t value) {
    assert(impl);
    impl->check(sqlite3_bind_int64(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, int16_t value) {
    assert(impl);
    impl->check(sqlite3_bind_int64(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, int32_t value) {
    assert(impl);
    impl->check(sqlite3_bind_int64(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, int64_t value) {
    assert(impl);
    impl->check(sqlite3_bind_int64(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, uint8_t value) {
    assert(impl);
    impl->check(sqlite3_bind_int64(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, uint16_t value) {
    assert(impl);
    impl->check(sqlite3_bind_int64(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, uint32_t value) {
    assert(impl);
    impl->check(sqlite3_bind_int64(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, float value) {
    assert(impl);
    impl->check(sqlite3_bind_double(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, double value) {
    assert(impl);
    impl->check(sqlite3_bind_double(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, bool value) {
    assert(impl);
    impl->check(sqlite3_bind_int(impl->stmt, offset, value));
}

template <> void Statement::bind(int offset, const char *value) {
    assert(impl);
    impl->check(sqlite3_bind_text(impl->stmt, offset, value, -1, SQLITE_STATIC));
}

// We currently cannot use sqlite3_bind_blob64 / sqlite3_bind_text64 because they
// was introduced in SQLite 3.8.7, and we need to support earlier versions:
//    iOS 8.0: 3.7.13
//    iOS 8.2: 3.8.5
// According to http://stackoverflow.com/questions/14288128/what-version-of-sqlite-does-ios-provide,
// the first iOS version with 3.8.7+ was 9.0, with 3.8.8.

void Statement::bind(int offset, const char * value, std::size_t length, bool retain) {
    assert(impl);
    if (length > std::numeric_limits<int>::max()) {
        throw std::range_error("value too long for sqlite3_bind_text");
    }
    impl->check(sqlite3_bind_text(impl->stmt, offset, value, int(length),
                            retain ? SQLITE_TRANSIENT : SQLITE_STATIC));
}

void Statement::bind(int offset, const std::string& value, bool retain) {
    bind(offset, value.data(), value.size(), retain);
}

void Statement::bindBlob(int offset, const void * value, std::size_t length, bool retain) {
    assert(impl);
    if (length > std::numeric_limits<int>::max()) {
        throw std::range_error("value too long for sqlite3_bind_text");
    }
    impl->check(sqlite3_bind_blob(impl->stmt, offset, value, int(length),
                            retain ? SQLITE_TRANSIENT : SQLITE_STATIC));
}

void Statement::bindBlob(int offset, const std::vector<uint8_t>& value, bool retain) {
    bindBlob(offset, value.data(), value.size(), retain);
}

template <>
void Statement::bind(
        int offset, std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> value) {
    assert(impl);
    impl->check(sqlite3_bind_int64(impl->stmt, offset, std::chrono::system_clock::to_time_t(value)));
}

template <> void Statement::bind(int offset, optional<std::string> value) {
    if (!value) {
        bind(offset, nullptr);
    } else {
        bind(offset, *value);
    }
}

template <>
void Statement::bind(
    int offset,
    optional<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>> value) {
    if (!value) {
        bind(offset, nullptr);
    } else {
        bind(offset, *value);
    }
}

bool Statement::run() {
    assert(impl);
    const int err = sqlite3_step(impl->stmt);
    impl->lastInsertRowId = sqlite3_last_insert_rowid(sqlite3_db_handle(impl->stmt));
    impl->changes = sqlite3_changes(sqlite3_db_handle(impl->stmt));
    if (err == SQLITE_DONE) {
        return false;
    } else if (err == SQLITE_ROW) {
        return true;
    } else if (err != SQLITE_OK) {
        throw Exception { err, sqlite3_errmsg(sqlite3_db_handle(impl->stmt)) };
    } else {
        return false;
    }
}

template <> bool Statement::get(int offset) {
    assert(impl);
    return sqlite3_column_int(impl->stmt, offset);
}

template <> int Statement::get(int offset) {
    assert(impl);
    return sqlite3_column_int(impl->stmt, offset);
}

template <> int64_t Statement::get(int offset) {
    assert(impl);
    return sqlite3_column_int64(impl->stmt, offset);
}

template <> double Statement::get(int offset) {
    assert(impl);
    return sqlite3_column_double(impl->stmt, offset);
}

template <> std::string Statement::get(int offset) {
    assert(impl);
    return {
        reinterpret_cast<const char *>(sqlite3_column_blob(impl->stmt, offset)),
        size_t(sqlite3_column_bytes(impl->stmt, offset))
    };
}

template <> std::vector<uint8_t> Statement::get(int offset) {
    assert(impl);
    const auto* begin = reinterpret_cast<const uint8_t*>(sqlite3_column_blob(impl->stmt, offset));
    const uint8_t* end   = begin + sqlite3_column_bytes(impl->stmt, offset);
    return { begin, end };
}

template <>
std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
Statement::get(int offset) {
    assert(impl);
    return std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::from_time_t(sqlite3_column_int64(impl->stmt, offset)));
}

template <> optional<int64_t> Statement::get(int offset) {
    assert(impl);
    if (sqlite3_column_type(impl->stmt, offset) == SQLITE_NULL) {
        return optional<int64_t>();
    } else {
        return get<int64_t>(offset);
    }
}

template <> optional<double> Statement::get(int offset) {
    assert(impl);
    if (sqlite3_column_type(impl->stmt, offset) == SQLITE_NULL) {
        return optional<double>();
    } else {
        return get<double>(offset);
    }
}

template <> optional<std::string> Statement::get(int offset) {
    assert(impl);
    if (sqlite3_column_type(impl->stmt, offset) == SQLITE_NULL) {
        return optional<std::string>();
    } else {
        return get<std::string>(offset);
    }
}

template <>
optional<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>>
Statement::get(int offset) {
    assert(impl);
    if (sqlite3_column_type(impl->stmt, offset) == SQLITE_NULL) {
        return {};
    } else {
        return get<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>>(
            offset);
    }
}

void Statement::reset() {
    assert(impl);
    sqlite3_reset(impl->stmt);
}

void Statement::clearBindings() {
    assert(impl);
    sqlite3_clear_bindings(impl->stmt);
}

int64_t Statement::lastInsertRowId() const {
    assert(impl);
    return impl->lastInsertRowId;
}

uint64_t Statement::changes() const {
    assert(impl);
    auto changes_ = impl->changes;
    return (changes_ < 0 ? 0 : changes_);
}

Transaction::Transaction(Database& db_, Mode mode)
    : db(db_) {
    switch (mode) {
    case Deferred:
        db.exec("BEGIN DEFERRED TRANSACTION");
        break;
    case Immediate:
        db.exec("BEGIN IMMEDIATE TRANSACTION");
        break;
    case Exclusive:
        db.exec("BEGIN EXCLUSIVE TRANSACTION");
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
    db.exec("COMMIT TRANSACTION");
}

void Transaction::rollback() {
    needRollback = false;
    db.exec("ROLLBACK TRANSACTION");
}

} // namespace sqlite
} // namespace mapbox
