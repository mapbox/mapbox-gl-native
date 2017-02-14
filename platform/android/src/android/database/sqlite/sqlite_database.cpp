#include "sqlite_database.hpp"

namespace mbgl {
namespace android {
namespace android {
namespace database {
namespace sqlite {

jni::Class<SQLiteDatabase> SQLiteDatabase::Class(jni::JNIEnv& env) {
    static auto clazz = *jni::Class<SQLiteDatabase>::Find(env).NewGlobalRef(env).release();
    return clazz;
}

jni::Object<SQLiteDatabase>
SQLiteDatabase::OpenDatabase(jni::JNIEnv& env, jni::String path, jni::jint flags) {
    using Signature =
        jni::Object<SQLiteDatabase>(jni::String, jni::Object<CursorFactory>, jni::jint);
    static auto method = Class(env).GetStaticMethod<Signature>(env, "openDatabase");
    return Class(env).Call(env, method, path, jni::Object<CursorFactory>(nullptr), flags);
}

void SQLiteDatabase::ExecSQL(jni::JNIEnv& env, jni::Object<SQLiteDatabase> db, jni::String sql) {
    using Signature = void(jni::String);
    static auto method = Class(env).GetMethod<Signature>(env, "execSQL");
    return db.Call(env, method, sql);
}

void SQLiteDatabase::Close(jni::JNIEnv& env, jni::Object<SQLiteDatabase> db) {
    using Signature = void(void);
    static auto method = Class(env).GetMethod<Signature>(env, "close");
    return db.Call(env, method);
}

} // namespace sqlite
} // namespace database
} // namespace android
} // namespace android
} // namespace mbgl
