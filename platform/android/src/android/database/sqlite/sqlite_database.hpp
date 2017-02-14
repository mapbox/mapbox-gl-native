#pragma once

#include <jni/jni.hpp>


namespace mbgl {
namespace android {
namespace android {
namespace database {
namespace sqlite {

class SQLiteDatabase : public jni::jobject {
public:
    static constexpr auto Name() {
        return "android/database/sqlite/SQLiteDatabase";
    }
    static jni::Class<SQLiteDatabase> Class(jni::JNIEnv&);

    class CursorFactory {
    public:
        static constexpr auto Name() {
            return "android/database/sqlite/SQLiteDatabase$CursorFactory";
        }
    };

    static constexpr const int OPEN_READWRITE = 0x00000000;
    static constexpr const int OPEN_READONLY = 0x00000001;
    static constexpr const int CREATE_IF_NECESSARY = 0x10000000;

    static jni::Object<SQLiteDatabase>
    OpenDatabase(jni::JNIEnv&, jni::String path, jni::jint flags);

    static void ExecSQL(jni::JNIEnv&, jni::Object<SQLiteDatabase>, jni::String sql);
    static void Close(jni::JNIEnv&, jni::Object<SQLiteDatabase>);
};

} // namespace sqlite
} // namespace database
} // namespace android
} // namespace android
} // namespace mbgl
