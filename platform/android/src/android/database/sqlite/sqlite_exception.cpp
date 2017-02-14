#include "sqlite_exception.hpp"

namespace mbgl {
namespace android {
namespace android {
namespace database {
namespace sqlite {

jni::Class<SQLiteException> SQLiteException::Class(jni::JNIEnv& env) {
    static auto clazz = *jni::Class<SQLiteException>::Find(env).NewGlobalRef(env).release();
    return clazz;
}

jni::Class<SQLiteCantOpenDatabaseException> SQLiteCantOpenDatabaseException::Class(jni::JNIEnv& env) {
    static auto clazz = *jni::Class<SQLiteCantOpenDatabaseException>::Find(env).NewGlobalRef(env).release();
    return clazz;
}

jni::Class<SQLiteDatabaseCorruptException> SQLiteDatabaseCorruptException::Class(jni::JNIEnv& env) {
    static auto clazz = *jni::Class<SQLiteDatabaseCorruptException>::Find(env).NewGlobalRef(env).release();
    return clazz;
}

} // namespace sqlite
} // namespace database
} // namespace android
} // namespace android
} // namespace mbgl
