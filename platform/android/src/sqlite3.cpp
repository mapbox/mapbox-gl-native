#include "../default/sqlite3.hpp"

#include "android/database/sqlite/sqlite_database.hpp"
#include "android/database/sqlite/sqlite_exception.hpp"
#include "java/lang/throwable.hpp"

#include "attach_env.hpp"

using namespace mbgl::android;

using android::database::sqlite::SQLiteDatabase;
using android::database::sqlite::SQLiteCantOpenDatabaseException;
using android::database::sqlite::SQLiteDatabaseCorruptException;
using java::lang::Throwable;

namespace mapbox {
namespace sqlite {

template <typename Lambda>
void checkException(jni::JNIEnv& env, Lambda&& lambda) {
    try {
        lambda();
    } catch (const jni::PendingJavaException&) {
        auto throwable = jni::Object<Throwable>(jni::ExceptionOccurred(env));
        jni::ExceptionClear(env);
        const std::string message =
            jni::Make<std::string>(env, Throwable::GetMessage(env, throwable));
        if (throwable.IsInstanceOf(env, SQLiteCantOpenDatabaseException::Class(env))) {
            throw Exception{ Exception::Code::CANTOPEN, message };
        } else if (throwable.IsInstanceOf(env, SQLiteDatabaseCorruptException::Class(env))) {
            throw Exception{ Exception::Code::NOTADB, message };
        } else {
            throw Exception{ 1 /* SQLITE_ERROR */, message };
        }
    }
}

class DatabaseImpl {
public:
    DatabaseImpl(const std::string& filename, OpenFlag openFlags) {
        int flags = 0;
        if (openFlags & OpenFlag::ReadOnly) {
            flags |= SQLiteDatabase::OPEN_READONLY;
        }
        if (openFlags & OpenFlag::ReadWrite) {
            flags |= SQLiteDatabase::OPEN_READWRITE;
        }
        if (openFlags & OpenFlag::Create) {
            flags |= SQLiteDatabase::CREATE_IF_NECESSARY;
        }

        checkException(*env, [&] {
            db = SQLiteDatabase::OpenDatabase(*env, jni::Make<jni::String>(*env, filename), flags).NewGlobalRef(*env);
        });
    }

    UniqueEnv env{ AttachEnv() };
    jni::UniqueObject<android::database::sqlite::SQLiteDatabase> db;
};

Database::Database(const std::string& filename, int flags)
    : impl(std::make_unique<DatabaseImpl>(filename, OpenFlag(flags))) {
}

Database::Database(Database&& other) : impl(std::move(other.impl)) {
}

Database& Database::operator=(Database&& other) {
    std::swap(impl, other.impl);
    return *this;
}

Database::~Database() {
}

Database::operator bool() const {
    return impl.operator bool();
}

void Database::setBusyTimeout(std::chrono::milliseconds timeout) {
    // Java bindings don't support setting a direct busy_timeout.
    exec(std::string{ "PRAGMA busy_timeout = " } +
         std::to_string(std::min<std::chrono::milliseconds::rep>(timeout.count(),
                                                                 std::numeric_limits<int>::max())));
}

void Database::exec(const std::string &sql) {
    assert(impl);
    checkException(*impl->env, [&] {
        SQLiteDatabase::ExecSQL(*impl->env, *impl->db, jni::Make<jni::String>(*impl->env, sql));
    });
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

Statement::~Statement() {
}

Statement::operator bool() const {
    return impl.operator bool();
}

} // namespace sqlite
} // namespace mapbox
