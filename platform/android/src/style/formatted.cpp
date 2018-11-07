#include "formatted.hpp"
#include "formatted_section.hpp"

namespace mbgl {
namespace android {

void Formatted::registerNative(jni::JNIEnv& env) {
    jni::Class<Formatted>::Singleton(env);
}

jni::Local<jni::Object<Formatted>> Formatted::New(jni::JNIEnv& env, const style::expression::Formatted& value) {
    static auto& formatted = jni::Class<Formatted>::Singleton(env);
    static auto formattedConstructor = formatted.GetConstructor<jni::Array<jni::Object<FormattedSection>>>(env);
    static auto& formattedSection = jni::Class<FormattedSection>::Singleton(env);

    auto sections = jni::Array<jni::Object<FormattedSection>>::New(env, value.sections.size());
    for (std::size_t i = 0; i < value.sections.size(); i++) {
        auto section = value.sections.at(i);
        auto text = jni::Make<jni::String>(env, section.text);

        if (section.fontStack && section.fontScale) {
            double fontScale = section.fontScale.value();
            auto fontStack = jni::Array<jni::String>::New(env, section.fontStack.value().size());
            for (std::size_t j = 0; j < section.fontStack.value().size(); j++) {
                fontStack.Set(env, j, jni::Make<jni::String>(env, section.fontStack.value().at(j)));
            }
            static auto formattedSectionConstructor = formattedSection.GetConstructor<jni::String, jni::Number, jni::Array<jni::String>>(env);
            sections.Set(env, i, formattedSection.New(env, formattedSectionConstructor, text, jni::Box(env, fontScale), fontStack));
        } else if (section.fontScale) {
            double fontScale = section.fontScale.value();
            static auto formattedSectionConstructor = formattedSection.GetConstructor<jni::String, jni::Number>(env);
            sections.Set(env, i, formattedSection.New(env, formattedSectionConstructor, text, jni::Box(env, fontScale)));
        } else if (section.fontStack) {
            auto fontStack = jni::Array<jni::String>::New(env, section.fontStack.value().size());
            for (std::size_t j = 0; j < section.fontStack.value().size(); j++) {
                fontStack.Set(env, j, jni::Make<jni::String>(env, section.fontStack.value().at(j)));
            }
            static auto formattedSectionConstructor = formattedSection.GetConstructor<jni::String, jni::Array<jni::String>>(env);
            sections.Set(env, i, formattedSection.New(env, formattedSectionConstructor, text, fontStack));
        } else {
            static auto formattedSectionConstructor = formattedSection.GetConstructor<jni::String>(env);
            sections.Set(env, i, formattedSection.New(env, formattedSectionConstructor, text));
        }
    }

    return formatted.New(env, formattedConstructor, sections);
}

} // namespace android
} // namespace mbgl