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

// https://www.sqlite.org/rescode.html#ok
static_assert(mbgl::underlying_type(Exception::OK) == 0, "error");
// https://www.sqlite.org/rescode.html#cantopen
static_assert(mbgl::underlying_type(Exception::CANTOPEN) == 14, "error");
// https://www.sqlite.org/rescode.html#notadb
static_assert(mbgl::underlying_type(Exception::NOTADB) == 26, "error");

void checkQueryError(const QSqlQuery& query) {
    QSqlError lastError = query.lastError();
    if (lastError.type() != QSqlError::NoError) {
#if QT_VERSION >= 0x050300
        throw Exception { lastError.nativeErrorCode().toInt(), lastError.text().toStdString() };
#else
        throw Exception { lastError.number(), lastError.text().toStdString() };
#endif
    }
}

void checkDatabaseError(const QSqlDatabase &db) {
    QSqlError lastError = db.lastError();
    if (lastError.type() != QSqlError::NoError) {
#if QT_VERSION >= 0x050300
        throw Exception { lastError.nativeErrorCode().toInt(), lastError.text().toStdString() };
#else
        throw Exception { lastError.number(), lastError.text().toStdString() };
#endif
    }
}

void checkDatabaseOpenError(const QSqlDatabase &db) {
    // Assume every error when opening the data as CANTOPEN. Qt
    // always returns -1 for `nativeErrorCode()` on database errors.
    QSqlError lastError = db.lastError();
    if (lastError.type() != QSqlError::NoError) {
        throw Exception { Exception::Code::CANTOPEN, "Error opening the database." };
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
    DatabaseImpl(const char* filename, int flags)
        : connectionName(QString::number(uint64_t(QThread::currentThread())) + incrementCounter())
    {
        if (!QSqlDatabase::drivers().contains("QSQLITE")) {
            throw Exception { Exception::Code::CANTOPEN, "SQLite driver not found." };
        }

        assert(!QSqlDatabase::contains(connectionName));
        auto db = QSqlDatabase::addDatabase("QSQLITE", connectionName);

        QString connectOptions = db.connectOptions();
        if (flags & OpenFlag::ReadOnly) {
            if (!connectOptions.isEmpty()) connectOptions.append(';');
            connectOptions.append("QSQLITE_OPEN_READONLY");
        }
        if (flags & OpenFlag::SharedCache) {
            if (!connectOptions.isEmpty()) connectOptions.append(';');
            connectOptions.append("QSQLITE_ENABLE_SHARED_CACHE");
        }

        db.setConnectOptions(connectOptions);
        db.setDatabaseName(QString(filename));

        if (!db.open()) {
            checkDatabaseOpenError(db);
        }
    }

    ~DatabaseImpl() {
        auto db = QSqlDatabase::database(connectionName);
        db.close();
        checkDatabaseError(db);
    }

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


Database::Database(const std::string& file, int flags)
        : impl(std::make_unique<DatabaseImpl>(file.c_str(), flags)) {
    assert(impl);
}

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

    // std::chrono::milliseconds.count() is a long and Qt will cast
    // internally to int, so we need to make sure the limits apply.
    std::string timeoutStr = mbgl::util::toString(timeout.count() & INT_MAX);

    auto db = QSqlDatabase::database(impl->connectionName);
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
        checkDatabaseOpenError(db);
    }
}

void Database::exec(const std::string &sql) {
    assert(impl);
    QStringList statements = QString::fromStdString(sql).split(';', QString::SkipEmptyParts);
    statements.removeAll("\n");
    for (QString statement : statements) {
        if (!statement.endsWith(';')) {
            statement.append(';');
        }
        QSqlQuery query(QSqlDatabase::database(impl->connectionName));
        query.prepare(statement);

        if (!query.exec()) {
            checkQueryError(query);
        }
    }
}

Statement Database::prepare(const char *query) {
    return Statement(this, query);
}

Statement::Statement(Database *db, const char *sql)
        : impl(std::make_unique<StatementImpl>(QString(sql), QSqlDatabase::database(db->impl->connectionName))) {
    assert(impl);
}

Statement::Statement(Statement &&other)
        : impl(std::move(other.impl)) {
    assert(impl);
}

Statement &Statement::operator=(Statement &&other) {
    assert(impl);
    std::swap(impl, other.impl);
    return *this;
}

Statement::~Statement() {
}

template void Statement::bind(int, int64_t);

template <typename T>
void Statement::bind(int offset, T value) {
    assert(impl);
    // Field numbering starts at 0.
    impl->query.bindValue(offset - 1, QVariant::fromValue<T>(value), QSql::In);
    checkQueryError(impl->query);
}

template <>
void Statement::bind(int offset, std::nullptr_t) {
    assert(impl);
    // Field numbering starts at 0.
    impl->query.bindValue(offset - 1, QVariant(QVariant::Invalid), QSql::In);
    checkQueryError(impl->query);
}

template <>
void Statement::bind(int offset, int32_t value) {
    bind(offset, static_cast<int64_t>(value));
}

template <>
void Statement::bind(int offset, bool value) {
    bind(offset, static_cast<int>(value));
}

template <>
void Statement::bind(int offset, int8_t value) {
    bind(offset, static_cast<int64_t>(value));
}

template <>
void Statement::bind(int offset, uint8_t value) {
    bind(offset, static_cast<int64_t>(value));
}

template <>
void Statement::bind(int offset, mbgl::Timestamp value) {
    bind(offset, std::chrono::system_clock::to_time_t(value));
}

template <>
void Statement::bind(int offset, optional<std::string> value) {
    if (value) {
        bind(offset, *value);
    } else {
        bind(offset, nullptr);
    }
}

template <>
void Statement::bind(int offset, optional<mbgl::Timestamp> value) {
    if (value) {
        bind(offset, *value);
    } else {
        bind(offset, nullptr);
    }
}

void Statement::bind(int offset, const char* value, std::size_t length, bool retain) {
    assert(impl);
    if (length > std::numeric_limits<int>::max()) {
        // Kept for consistence with the default implementation.
        throw std::range_error("value too long");
    }

    // Qt SQLite driver treats QByteArray as blob: we need to explicitly
    // declare the variant type as string.
    QVariant text(QVariant::Type::String);
    text.setValue(retain ? QByteArray(value, length) : QByteArray::fromRawData(value, length));

    // Field numbering starts at 0.
    impl->query.bindValue(offset - 1, std::move(text), QSql::In);

    checkQueryError(impl->query);
}

void Statement::bind(int offset, const std::string& value, bool retain) {
    bind(offset, value.data(), value.size(), retain);
}

void Statement::bindBlob(int offset, const void* value_, std::size_t length, bool retain) {
    assert(impl);
    const char* value = reinterpret_cast<const char*>(value_);
    if (length > std::numeric_limits<int>::max()) {
        // Kept for consistence with the default implementation.
        throw std::range_error("value too long");
    }

    // Field numbering starts at 0.
    impl->query.bindValue(offset - 1, retain ? QByteArray(value, length) :
            QByteArray::fromRawData(value, length), QSql::In | QSql::Binary);

    checkQueryError(impl->query);
}

void Statement::bindBlob(int offset, const std::vector<uint8_t>& value, bool retain) {
    bindBlob(offset, value.data(), value.size(), retain);
}

bool Statement::run() {
    assert(impl);

    if (!impl->query.isValid()) {
       if (impl->query.exec()) {
           impl->lastInsertRowId = impl->query.lastInsertId().value<int64_t>();
           impl->changes = impl->query.numRowsAffected();
       } else {
           checkQueryError(impl->query);
       }
    }

    const bool hasNext = impl->query.next();
    if (!hasNext) impl->query.finish();

    return hasNext;
}

template bool Statement::get(int);
template int Statement::get(int);
template int64_t Statement::get(int);
template double Statement::get(int);

template <typename T> T Statement::get(int offset) {
    assert(impl && impl->query.isValid());
    QVariant value = impl->query.value(offset);
    checkQueryError(impl->query);
    return value.value<T>();
}

template <> std::vector<uint8_t> Statement::get(int offset) {
    assert(impl && impl->query.isValid());
    QByteArray byteArray = impl->query.value(offset).toByteArray();
    checkQueryError(impl->query);
    std::vector<uint8_t> blob(byteArray.begin(), byteArray.end());
    return blob;
}

template <> mbgl::Timestamp Statement::get(int offset) {
    assert(impl && impl->query.isValid());
    QVariant value = impl->query.value(offset);
    checkQueryError(impl->query);
    return std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::from_time_t(value.value<::time_t>()));
}

template <> optional<int64_t> Statement::get(int offset) {
    assert(impl && impl->query.isValid());
    QVariant value = impl->query.value(offset);
    checkQueryError(impl->query);
    if (value.isNull())
        return {};
    return { value.value<int64_t>() };
}

template <> optional<double> Statement::get(int offset) {
    assert(impl && impl->query.isValid());
    QVariant value = impl->query.value(offset);
    checkQueryError(impl->query);
    if (value.isNull())
        return {};
    return { value.value<double>() };
}

template <> std::string Statement::get(int offset) {
    assert(impl && impl->query.isValid());
    QByteArray value = impl->query.value(offset).toByteArray();
    checkQueryError(impl->query);
    return std::string(value.constData(), value.size());
}

template <> optional<std::string> Statement::get(int offset) {
    assert(impl && impl->query.isValid());
    QByteArray value = impl->query.value(offset).toByteArray();
    checkQueryError(impl->query);
    if (value.isNull())
        return {};
    return { std::string(value.constData(), value.size()) };
}

template <> optional<mbgl::Timestamp> Statement::get(int offset) {
    assert(impl && impl->query.isValid());
    QVariant value = impl->query.value(offset);
    checkQueryError(impl->query);
    if (value.isNull())
        return {};
    return { std::chrono::time_point_cast<mbgl::Seconds>(
        std::chrono::system_clock::from_time_t(value.value<::time_t>())) };
}

void Statement::reset() {
    assert(impl);
    impl->query.finish();
}

void Statement::clearBindings() {
    // no-op
}

int64_t Statement::lastInsertRowId() const {
    assert(impl);
    return impl->lastInsertRowId;
}

uint64_t Statement::changes() const {
    assert(impl);
    return (impl->changes < 0 ? 0 : impl->changes);
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
