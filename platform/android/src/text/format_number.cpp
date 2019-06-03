#include <mbgl/style/expression/collator.hpp>
#include <mbgl/text/language_tag.hpp>
#include <mbgl/util/platform.hpp>

#include <jni/jni.hpp>

#include "../attach_env.hpp"
#include "format_number_jni.hpp"

namespace mbgl {
namespace android {

void NumberFormat::registerNative(jni::JNIEnv& env) {
    jni::Class<NumberFormat>::Singleton(env);
}

jni::Local<jni::Object<NumberFormat>> NumberFormat::getInstance(jni::JNIEnv& env, const jni::Object<Locale>& locale) {
    static auto& javaClass = jni::Class<NumberFormat>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<NumberFormat> (jni::Object<Locale>)>(env, "getInstance");
    return javaClass.Call(env, method, locale);
}

jni::Local<jni::Object<NumberFormat>> NumberFormat::getCurrencyInstance(jni::JNIEnv& env, const jni::Object<Locale>& locale) {
    static auto& javaClass = jni::Class<NumberFormat>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<NumberFormat> (jni::Object<Locale>)>(env, "getCurrencyInstance");
    return javaClass.Call(env, method, locale);
}

jni::Local<jni::String> NumberFormat::format(jni::JNIEnv& env, const jni::Object<NumberFormat>& nf, jni::jdouble number) {
    static auto& javaClass = jni::Class<NumberFormat>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::String (jni::jdouble)>(env, "format");
    return nf.Call(env, method, number);
}

void NumberFormat::setMinimumFractionDigits(jni::JNIEnv& env, const jni::Object<NumberFormat>& nf, jni::jint value) {
    static auto& javaClass = jni::Class<NumberFormat>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::jint)>(env, "setMinimumFractionDigits");
    return nf.Call(env, method, value);
}

void NumberFormat::setMaximumFractionDigits(jni::JNIEnv& env, const jni::Object<NumberFormat>& nf, jni::jint value) {
    static auto& javaClass = jni::Class<NumberFormat>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::jint)>(env, "setMaximumFractionDigits");
    return nf.Call(env, method, value);
}

} // namespace android

namespace platform {

std::string formatNumber(double number, const std::string& localeId, const std::string& currency,
                         uint8_t minFractionDigits, uint8_t maxFractionDigits) {

    auto env{ android::AttachEnv() };

    jni::Global<jni::Object<android::Locale>> locale;
    LanguageTag languageTag = !localeId.empty() ? LanguageTag::fromBCP47(localeId) : LanguageTag();
    if (!languageTag.language) {
        locale = jni::NewGlobal(*env, android::Locale::getDefault(*env));
    } else if (!languageTag.region) {
        locale = jni::NewGlobal(*env, android::Locale::New(*env, jni::Make<jni::String>(*env, *languageTag.language)));
    } else {
        locale = jni::NewGlobal(*env, android::Locale::New(*env, jni::Make<jni::String>(*env, *languageTag.language),
                                                                 jni::Make<jni::String>(*env, *languageTag.region)));
    }

    jni::Global<jni::Object<android::NumberFormat>> formatter;
    if (currency.empty()) {
        formatter = jni::NewGlobal(*env, android::NumberFormat::getInstance(*env, locale));
        android::NumberFormat::setMinimumFractionDigits(*env, formatter, static_cast<jni::jint>(minFractionDigits));
        android::NumberFormat::setMaximumFractionDigits(*env, formatter, static_cast<jni::jint>(maxFractionDigits));
    } else {
        formatter = jni::NewGlobal(*env, android::NumberFormat::getCurrencyInstance(*env, locale));
    }

    auto result = android::NumberFormat::format(*env, formatter, static_cast<jni::jdouble>(number));
    return jni::Make<std::string>(*env, result);
}

} // namespace platform
} // namespace mbgl
