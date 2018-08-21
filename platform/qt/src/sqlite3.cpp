#include "sqlite3.hpp"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QThread>
#include <QVariant>
#include <QAtomicInt>

#include <cassert>
#include <cstring>
#include <cstdio>
#include <chrono>
#include <limits>

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/traits.hpp>

namespace mapbox {
namespace sqlite {

void checkQueryError(const QSqlQuery& query) {
    QSqlError lastError = query.lastError();
    if (lastError.type() != QSqlError::NoError) {
#if QT_VERSION >= 0x050300
        throw Exception { lastError.nativeErrorCode().toInt(), lastError.databaseText().toStdString() };
#else
        throw Exception { lastError.number(), lastError.databaseText().toStdString() };
#endif
    }
}

void checkDatabaseError(const QSqlDatabase &db) {
    QSqlError lastError = db.lastError();
    if (lastError.type() != QSqlError::NoError) {
#if QT_VERSION >= 0x050300
        throw Exception { lastError.nativeErrorCode().toInt(), lastError.databaseText().toStdString() };
#else
        throw Exception { lastError.number(), lastError.databaseText().toStdString() };
#endif
    }
}

namespace {
    QString incrementCounter() {
        static QAtomicInt count = 0;
        return QString::number(count.fetchAndAddAcquire(1));
    }
}

class DatabaseImpl {
public:
    DatabaseImpl(QString connectionName_)
        : connectionName(std::move(connectionName_))
    {
    }

    ~DatabaseImpl() {
        auto db = QSqlDatabase::database(connectionName);
        db.close();
        checkDatabaseError(db);
    }

    void setBusyTimeout(std::chrono::milliseconds timeout);
    void exec(const std::string& sql);

    QString connectionName;
};

class StatementImpl {
public:
    StatementImpl(const QString& sql, const QSqlDatabase& db) : query(db) {
        if (!query.prepare(sql)) {
            checkQueryError(query);
        }
    }

    ~StatementImpl() {
        query.clear();
    }

    QSqlQuery query;
    int64_t lastInsertRowId = 0;
    int64_t changes = 0;
};

template <typename T>
using optional = std::experimental::optional<T>;

mapbox::util::variant<Database, Exception> Database::tryOpen(const std::string &filename, int flags) {
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        return Exception { ResultCode::CantOpen, "SQLite driver not found." };
    }

    QString connectionName = QString::number(uint64_t(QThread::currentThread())) + incrementCounter();

    assert(!QSqlDatabase::contains(connectionName));
    auto db = QSqlDatabase::addDatabase("QSQLITE", connectionName);

    QString connectOptions = db.connectOptions();
    if (flags & OpenFlag::ReadOnly) {
        if (!connectOptions.isEmpty()) connectOptions.append(';');
        connectOptions.append("QSQLITE_OPEN_READONLY");
    }

    if (filename.compare(0, 5, "file:") == 0) {
        if (!connectOptions.isEmpty()) connectOptions.append(';');
        connectOptions.append("QSQLITE_OPEN_URI");
    }

    db.setConnectOptions(connectOptions);
    db.setDatabaseName(QString(filename.c_str()));

    if (!db.open()) {
        // Assume every error when opening the data as CANTOPEN. Qt
        // always returns -1 for `nativeErrorCode()` on database errors.
        return Exception { ResultCode::CantOpen, "Error opening the database." };
    }

    return Database(std::make_unique<DatabaseImpl>(connectionName));
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
        : impl(std::move(other.impl)) {
    assert(impl);
}

Database &Database::operator=(Database &&other) {
    std::swap(impl, other.impl);
    assert(impl);
    return *this;
}

Database::~Database() {
}

void Database::setBusyTimeout(std::chrono::milliseconds timeout) {
    assert(impl);
    impl->setBusyTimeout(timeout);
}

void DatabaseImpl::setBusyTimeout(std::chrono::milliseconds timeout) {
    // std::chrono::milliseconds.count() is a long and Qt will cast
    // internally to int, so we need to make sure the limits apply.
    std::string timeoutStr = mbgl::util::toString(timeout.count() & INT_MAX);

    auto db = QSqlDatabase::database(connectionName);
    QString connectOptions = db.connectOptions();
    if (connectOptions.isEmpty()) {
        if (!connectOptions.isEmpty()) connectOptions.append(';');
        connectOptions.append("QSQLITE_BUSY_TIMEOUT=").append(QString::fromStdString(timeoutStr));
    }
    if (db.isOpen()) {
        db.close();
    }
    db.setConnectOptions(connectOptions);
    if (!db.open()) {
        // Assume every error when opening the data as CANTOPEN. Qt
        // always returns -1 for `nativeErrorCode()` on database errors.
        throw Exception { ResultCode::CantOpen, "Error opening the database." };
    }
}

void Database::exec(const std::string &sql) {
    assert(impl);
    impl->exec(sql);
}

void DatabaseImpl::exec(const std::string& sql) {
    QStringList statements = QString::fromStdString(sql).split(';', QString::SkipEmptyParts);
    statements.removeAll("\n");
    for (QString statement : statements) {
        if (!statement.endsWith(';')) {
            statement.append(';');
        }
        QSqlQuery query(QSqlDatabase::database(connectionName));
        query.prepare(statement);

        if (!query.exec()) {
            checkQueryError(query);
        }
    }
}

Statement::Statement(Database& db, const char* sql)
    : impl(std::make_unique<StatementImpl>(QString(sql),
                                           QSqlDatabase::database(db.impl->connectionName))) {
    assert(impl);
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

template void Query::bind(int, int64_t);

template <typename T>
void Query::bind(int offset, T value) {
    assert(stmt.impl);
    // Field numbering starts at 0.
    stmt.impl->query.bindValue(offset - 1, QVariant::fromValue<T>(value), QSql::In);
    checkQueryError(stmt.impl->query);
}

template <>
void Query::bind(int offset, std::nullptr_t) {
    assert(stmt.impl);
    // Field numbering starts at 0.
    stmt.impl->query.bindValue(offset - 1, QVariant(QVariant::Invalid), QSql::In);
    checkQueryError(stmt.impl->query);
}

template <>
void Query::bind(int offset, int32_t value) {
    bind(offset, static_cast<int64_t>(value));
}

template <>
void Query::bind(int offset, bool value) {
    bind(offset, static_cast<int>(value));
}

template <>
void Query::bind(int offset, int8_t value) {
    bind(offset, static_cast<int64_t>(value));
}

template <>
void Query::bind(int offset, uint8_t value) {
    bind(offset, static_cast<int64_t>(value));
}

template <>
void Query::bind(int offset, mbgl::Timestamp value) {
    bind(offset, std::chrono::system_clock::to_time_t(value));
}

template <>
void Query::bind(int offset, optional<std::string> value) {
    if (value) {
        bind(offset, *value);
    } else {
        bind(offset, nullptr);
    }
}

template <>
void Query::bind(int offset, optional<mbgl::Timestamp> value) {
    if (value) {
        bind(offset, *value);
    } else {
        bind(offset, nullptr);
    }
}

void Query::bind(int offset, const char* value, std::size_t length, bool /* retain */) {
    assert(stmt.impl);
    if (length > std::numeric_limits<int>::max()) {
        // Kept for consistence with the default implementation.
        throw std::range_error("value too long");
    }

    // Field numbering starts at 0.
    stmt.impl->query.bindValue(offset - 1, QString(QByteArray(value, length)), QSql::In);

    checkQueryError(stmt.impl->query);
}

void Query::bind(int offset, const std::string& value, bool retain) {
    bind(offset, value.data(), value.size(), retain);
}

void Query::bindBlob(int offset, const void* value_, std::size_t length, bool retain) {
    assert(stmt.impl);
    const char* value = reinterpret_cast<const char*>(value_);
    if (length > std::numeric_limits<int>::max()) {
        // Kept for consistence with the default implementation.
        throw std::range_error("value too long");
    }

    // Field numbering starts at 0.
    stmt.impl->query.bindValue(offset - 1, retain ? QByteArray(value, length) :
            QByteArray::fromRawData(value, length), QSql::In | QSql::Binary);

    checkQueryError(stmt.impl->query);
}

void Query::bindBlob(int offset, const std::vector<uint8_t>& value, bool retain) {
    bindBlob(offset, value.data(), value.size(), retain);
}

bool Query::run() {
    assert(stmt.impl);

    if (!stmt.impl->query.isValid()) {
       if (stmt.impl->query.exec()) {
           stmt.impl->lastInsertRowId = stmt.impl->query.lastInsertId().value<int64_t>();
           stmt.impl->changes = stmt.impl->query.numRowsAffected();
       } else {
           checkQueryError(stmt.impl->query);
       }
    }

    const bool hasNext = stmt.impl->query.next();
    if (!hasNext) stmt.impl->query.finish();

    return hasNext;
}

template bool Query::get(int);
template int Query::get(int);
template int64_t Query::get(int);
template double Query::get(int);

template <typename T> T Query::get(int offset) {
    assert(stmt.impl && stmt.impl->query.isValid());
    QVariant value = stmt.impl->query.value(offset);
    checkQueryError(stmt.impl->query);
    return value.value<T>();
}

template <> std::vector<uint8_t> Query::get(int offset) {
    assert(stmt.impl && stmt.impl->query.isValid());
    QByteArray byteArray = stmt.impl->query.value(offset).toByteArray();
    checkQueryError(stmt.impl->query);
    std::vector<uint8_t> blob(byteArray.begin(), byteArray.end());
    return blob;
}

template <> mbgl::Timestamp Query::get(int offset) {
    assert(stmt.impl && stmt.impl->query.isValid());
    QVariant value = stmt.impl->query.value(offset);
    checkQueryError(stmt.impl->query);
    return std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::from_time_t(value.value<::time_t>()));
}

template <> optional<int64_t> Query::get(int offset) {
    assert(stmt.impl && stmt.impl->query.isValid());
    QVariant value = stmt.impl->query.value(offset);
    checkQueryError(stmt.impl->query);
    if (value.isNull())
        return {};
    return { value.value<int64_t>() };
}

template <> optional<double> Query::get(int offset) {
    assert(stmt.impl && stmt.impl->query.isValid());
    QVariant value = stmt.impl->query.value(offset);
    checkQueryError(stmt.impl->query);
    if (value.isNull())
        return {};
    return { value.value<double>() };
}

template <> std::string Query::get(int offset) {
    assert(stmt.impl && stmt.impl->query.isValid());
    QByteArray value = stmt.impl->query.value(offset).toByteArray();
    checkQueryError(stmt.impl->query);
    return std::string(value.constData(), value.size());
}

template <> optional<std::string> Query::get(int offset) {
    assert(stmt.impl && stmt.impl->query.isValid());
    QByteArray value = stmt.impl->query.value(offset).toByteArray();
    checkQueryError(stmt.impl->query);
    if (value.isNull())
        return {};
    return { std::string(value.constData(), value.size()) };
}

template <> optional<mbgl::Timestamp> Query::get(int offset) {
    assert(stmt.impl && stmt.impl->query.isValid());
    QVariant value = stmt.impl->query.value(offset);
    checkQueryError(stmt.impl->query);
    if (value.isNull())
        return {};
    return { std::chrono::time_point_cast<mbgl::Seconds>(
        std::chrono::system_clock::from_time_t(value.value<::time_t>())) };
}

void Query::reset() {
    assert(stmt.impl);
    stmt.impl->query.finish();
}

void Query::clearBindings() {
    // no-op
}

int64_t Query::lastInsertRowId() const {
    assert(stmt.impl);
    return stmt.impl->lastInsertRowId;
}

uint64_t Query::changes() const {
    assert(stmt.impl);
    return (stmt.impl->changes < 0 ? 0 : stmt.impl->changes);
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
