#include <mbgl/i18n/collator.hpp>
#include <mbgl/text/language_tag.hpp>

#include <jni/jni.hpp>

#include "../attach_env.hpp"
#include "collator_jni.hpp"

namespace mbgl {
namespace android {

void Collator::registerNative(jni::JNIEnv& env) {
    jni::Class<Collator>::Singleton(env);
}

jni::Local<jni::Object<Collator>> Collator::getInstance(jni::JNIEnv& env, const jni::Object<Locale>& locale) {
    static auto& javaClass = jni::Class<Collator>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<Collator> (jni::Object<Locale>)>(env, "getInstance");
    return javaClass.Call(env, method, locale);
}

void Collator::setStrength(jni::JNIEnv& env, const jni::Object<Collator>& collator, jni::jint strength) {
    static auto& javaClass = jni::Class<Collator>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::jint)>(env, "setStrength");
    collator.Call(env, method, strength);
}

jni::jint Collator::compare(jni::JNIEnv& env, const jni::Object<Collator>& collator, const jni::String& lhs, const jni::String& rhs) {
    static auto& javaClass = jni::Class<Collator>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jint (jni::String, jni::String)>(env, "compare");
    return collator.Call(env, method, lhs, rhs);
}

void StringUtils::registerNative(jni::JNIEnv& env) {
    jni::Class<StringUtils>::Singleton(env);
}

jni::Local<jni::String> StringUtils::unaccent(jni::JNIEnv& env, const jni::String& value) {
    static auto& javaClass = jni::Class<StringUtils>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::String (jni::String)>(env, "unaccent");
    return javaClass.Call(env, method, value);
}

void Locale::registerNative(jni::JNIEnv& env) {
    jni::Class<Locale>::Singleton(env);
}

/*
We would prefer to use for/toLanguageTag, but they're only available in API level 21+

jni::Object<Locale> Locale::forLanguageTag(jni::JNIEnv& env, jni::String languageTag) {
    using Signature = jni::Object<Locale>(jni::String);
    auto method = javaClass.GetStaticMethod<Signature>(env, "forLanguageTag");
    return javaClass.Call(env, method, languageTag);
}

jni::String Locale::toLanguageTag(jni::JNIEnv& env, jni::Object<Locale> locale) {
    using Signature = jni::String();
    auto static method = javaClass.GetMethod<Signature>(env, "toLanguageTag");
    return locale.Call(env, method);
}
*/

jni::Local<jni::String> Locale::getLanguage(jni::JNIEnv& env, const jni::Object<Locale>& locale) {
    static auto& javaClass = jni::Class<Locale>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::String ()>(env, "getLanguage");
    return locale.Call(env, method);
}

jni::Local<jni::String> Locale::getCountry(jni::JNIEnv& env, const jni::Object<Locale>& locale) {
    static auto& javaClass = jni::Class<Locale>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::String ()>(env, "getCountry");
    return locale.Call(env, method);
}

jni::Local<jni::Object<Locale>> Locale::getDefault(jni::JNIEnv& env) {
    static auto& javaClass = jni::Class<Locale>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<Locale> ()>(env, "getDefault");
    return javaClass.Call(env, method);
}

jni::Local<jni::Object<Locale>> Locale::New(jni::JNIEnv& env, const jni::String& language) {
    static auto& javaClass = jni::Class<Locale>::Singleton(env);
    static auto constructor = javaClass.GetConstructor<jni::String>(env);
    return javaClass.New(env, constructor, language);
}

jni::Local<jni::Object<Locale>> Locale::New(jni::JNIEnv& env, const jni::String& language, const jni::String& region) {
    static auto& javaClass = jni::Class<Locale>::Singleton(env);
    static auto constructor = javaClass.GetConstructor<jni::String, jni::String>(env);
    return javaClass.New(env, constructor, language, region);
}

} // namespace android

namespace platform {

class Collator::Impl {
public:
    Impl(bool caseSensitive_, bool diacriticSensitive_, optional<std::string> locale_)
        : caseSensitive(caseSensitive_)
        , diacriticSensitive(diacriticSensitive_)
        , env(android::AttachEnv())
    {
        LanguageTag languageTag = locale_ ? LanguageTag::fromBCP47(*locale_) : LanguageTag();
        if (!languageTag.language) {
            locale = jni::NewGlobal(*env,
                android::Locale::getDefault(*env));
        } else if (!languageTag.region) {
            locale = jni::NewGlobal(*env,
                android::Locale::New(*env, jni::Make<jni::String>(*env, *languageTag.language)));
        } else {
            locale = jni::NewGlobal(*env,
                android::Locale::New(*env, jni::Make<jni::String>(*env, *languageTag.language),
                                           jni::Make<jni::String>(*env, *languageTag.region)));
        }
        collator = jni::NewGlobal(*env, android::Collator::getInstance(*env, locale));
        if (!diacriticSensitive && !caseSensitive) {
            android::Collator::setStrength(*env, collator, 0 /*PRIMARY*/);
        } else if (diacriticSensitive && !caseSensitive) {
            android::Collator::setStrength(*env, collator, 1 /*SECONDARY*/);
        } else if (caseSensitive) {
            // If we're case-sensitive and diacritic-sensitive, we use a case-sensitive collator
            // and a fallback implementation of diacritic-insensitivity.
            android::Collator::setStrength(*env, collator, 2 /*TERTIARY*/);
        }
    }

    bool operator==(const Impl& other) const {
        return caseSensitive == other.caseSensitive &&
                diacriticSensitive == other.diacriticSensitive &&
                resolvedLocale() == other.resolvedLocale();
    }

    int compare(const std::string& lhs, const std::string& rhs) const {
        bool useUnaccent = !diacriticSensitive && caseSensitive;
        // java.text.Collator doesn't support a diacritic-insensitive/case-sensitive collation
        // order, so we have to compromise here. We use Android's case-sensitive Collator
        // against strings that have been "unaccented" using non-locale-aware nunicode logic.
        // Because of the difference in locale-awareness, this means turning on case-sensitivity
        // can _potentially_ change compare results for strings that don't actually have any case
        // differences.
        jni::Local<jni::String> jlhs = useUnaccent
                                           ? android::StringUtils::unaccent(*env, jni::Make<jni::String>(*env, lhs))
                                           : jni::Make<jni::String>(*env, lhs);
        jni::Local<jni::String> jrhs = useUnaccent
                                           ? android::StringUtils::unaccent(*env, jni::Make<jni::String>(*env, rhs))
                                           : jni::Make<jni::String>(*env, rhs);

        jni::jint result = android::Collator::compare(*env, collator, jlhs, jrhs);

        return result;
    }

    std::string resolvedLocale() const {
        std::string language = jni::Make<std::string>(*env, android::Locale::getLanguage(*env, locale));
        std::string region = jni::Make<std::string>(*env, android::Locale::getCountry(*env, locale));

        optional<std::string> resultLanguage;
        if (!language.empty()) resultLanguage = language;
        optional<std::string> resultRegion;
        if (!region.empty()) resultRegion = region;

        return LanguageTag(resultLanguage, {}, resultRegion).toBCP47();
    }

private:
    bool caseSensitive;
    bool diacriticSensitive;

    android::UniqueEnv env;
    jni::Global<jni::Object<android::Collator>> collator;
    jni::Global<jni::Object<android::Locale>> locale;
};


Collator::Collator(bool caseSensitive, bool diacriticSensitive, optional<std::string> locale_)
    : impl(std::make_shared<Impl>(caseSensitive, diacriticSensitive, std::move(locale_)))
{}

bool Collator::operator==(const Collator& other) const {
    return *impl == *(other.impl);
}

int Collator::compare(const std::string& lhs, const std::string& rhs) const {
    return impl->compare(lhs, rhs);
}

std::string Collator::resolvedLocale() const {
    return impl->resolvedLocale();
}

} // namespace platform
} // namespace mbgl
