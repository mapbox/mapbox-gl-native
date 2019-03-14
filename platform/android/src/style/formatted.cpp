#include "formatted.hpp"
#include "formatted_section.hpp"
#include "../conversion/conversion.hpp"
#include "../conversion/constant.hpp"

namespace mbgl {
namespace android {

void Formatted::registerNative(jni::JNIEnv& env) {
    jni::Class<Formatted>::Singleton(env);
}

jni::Local<jni::Object<Formatted>> Formatted::New(jni::JNIEnv& env, const style::expression::Formatted& value) {
    static auto& formatted = jni::Class<Formatted>::Singleton(env);
    static auto formattedConstructor = formatted.GetConstructor<jni::Array<jni::Object<FormattedSection>>>(env);
    static auto& formattedSectionClass = jni::Class<FormattedSection>::Singleton(env);

    auto sections = jni::Array<jni::Object<FormattedSection>>::New(env, value.sections.size());
    for (std::size_t i = 0; i < value.sections.size(); i++) {
        auto section = value.sections.at(i);
        auto text = jni::Make<jni::String>(env, section.text);
        static auto formattedSectionConstructor = formattedSectionClass.GetConstructor<jni::String>(env);
        auto formattedSection = formattedSectionClass.New(env, formattedSectionConstructor, text);

        if (section.fontScale) {
            double fontScale = section.fontScale.value();
            static auto method = formattedSectionClass.GetMethod<void (jni::Number)>(env, "setFontScale");
            formattedSection.Call(env, method, jni::Box(env, fontScale));
        }

        if (section.fontStack) {
            auto fontStack = jni::Array<jni::String>::New(env, section.fontStack.value().size());
            for (std::size_t j = 0; j < section.fontStack.value().size(); j++) {
                fontStack.Set(env, j, jni::Make<jni::String>(env, section.fontStack.value().at(j)));
            }
            static auto method = formattedSectionClass.GetMethod<void (jni::Array<jni::String>)>(env, "setFontStack");
            formattedSection.Call(env, method, fontStack);
        }

        if (section.textColor) {
            using namespace mbgl::android::conversion;
            auto textColor = std::move(*convert<jni::Local<jni::Object<>>>(env, *section.textColor));
            static auto method = formattedSectionClass.GetMethod<void (jni::Object<>)>(env, "setTextColor");
            formattedSection.Call(env, method, textColor);
        }

        sections.Set(env, i, formattedSection);
    }

    return formatted.New(env, formattedConstructor, sections);
}

} // namespace android
} // namespace mbgl