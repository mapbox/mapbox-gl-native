#pragma once

#include "../../../java/lang/throwable.hpp"

namespace mbgl {
namespace android {
namespace android {
namespace database {
namespace sqlite {

class SQLiteException : public java::lang::Throwable {
public:
    static constexpr auto Name() {
        return "android/database/sqlite/SQLiteException";
    }
    static jni::Class<SQLiteException> Class(jni::JNIEnv&);
};

class SQLiteCantOpenDatabaseException : public SQLiteException {
public:
    static constexpr auto Name() {
        return "android/database/sqlite/SQLiteCantOpenDatabaseException";
    }
    static jni::Class<SQLiteCantOpenDatabaseException> Class(jni::JNIEnv&);
};

class SQLiteDatabaseCorruptException : public SQLiteException {
public:
    static constexpr auto Name() {
        return "android/database/sqlite/SQLiteDatabaseCorruptException";
    }
    static jni::Class<SQLiteDatabaseCorruptException> Class(jni::JNIEnv&);
};

} // namespace sqlite
} // namespace database
} // namespace android
} // namespace android
} // namespace mbgl
