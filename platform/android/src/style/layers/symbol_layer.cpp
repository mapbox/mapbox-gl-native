// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "symbol_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

namespace mbgl {
namespace android {

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    SymbolLayer::SymbolLayer(jni::JNIEnv& env, jni::String& layerId, jni::String& sourceId)
        : Layer(env, std::make_unique<mbgl::style::SymbolLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    /**
     * Creates a non-owning peer object (for layers currently attached to the map)
     */
    SymbolLayer::SymbolLayer(mbgl::Map& map, mbgl::style::SymbolLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    /**
     * Creates an owning peer object (for layers not attached to the map)
     */
    SymbolLayer::SymbolLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::SymbolLayer> coreLayer)
        : Layer(map, std::move(coreLayer)) {
    }

    SymbolLayer::~SymbolLayer() = default;

    // Property getters

    jni::Local<jni::Object<>> SymbolLayer::getSymbolPlacement(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getSymbolPlacement()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getSymbolSpacing(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getSymbolSpacing()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getSymbolAvoidEdges(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getSymbolAvoidEdges()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getSymbolZOrder(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getSymbolZOrder()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconAllowOverlap(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconAllowOverlap()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconIgnorePlacement(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconIgnorePlacement()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconOptional(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconOptional()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconRotationAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconRotationAlignment()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconSize(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconSize()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconTextFit(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconTextFit()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconTextFitPadding(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconTextFitPadding()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconImage(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconImage()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconRotate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconRotate()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconPadding(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconPadding()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconKeepUpright(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconKeepUpright()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconOffset(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconOffset()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconAnchor()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconPitchAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconPitchAlignment()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextPitchAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextPitchAlignment()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextRotationAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextRotationAlignment()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextField(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextField()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextFont(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextFont()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextSize(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextSize()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextMaxWidth(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextMaxWidth()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextLineHeight(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextLineHeight()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextLetterSpacing(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextLetterSpacing()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextJustify(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextJustify()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextAnchor()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextMaxAngle(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextMaxAngle()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextRotate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextRotate()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextPadding(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextPadding()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextKeepUpright(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextKeepUpright()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextTransform(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextTransform()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextOffset(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextOffset()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextAllowOverlap(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextAllowOverlap()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextIgnorePlacement(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextIgnorePlacement()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextOptional(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextOptional()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setIconOpacityTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setIconColorTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconHaloColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconHaloColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconHaloColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconHaloColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconHaloColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setIconHaloColorTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconHaloWidth(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconHaloWidth()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconHaloWidthTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconHaloWidthTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconHaloWidthTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setIconHaloWidthTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconHaloBlur(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconHaloBlur()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconHaloBlurTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconHaloBlurTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconHaloBlurTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setIconHaloBlurTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconTranslate()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconTranslateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconTranslateTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setIconTranslateTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getIconTranslateAnchor()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setTextOpacityTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setTextColorTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextHaloColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextHaloColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextHaloColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextHaloColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextHaloColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setTextHaloColorTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextHaloWidth(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextHaloWidth()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextHaloWidthTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextHaloWidthTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextHaloWidthTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setTextHaloWidthTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextHaloBlur(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextHaloBlur()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextHaloBlurTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextHaloBlurTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextHaloBlurTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setTextHaloBlurTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextTranslate()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextTranslateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextTranslateTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::setTextTranslateTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::SymbolLayer>()->SymbolLayer::getTextTranslateAnchor()));
    }


    jni::Local<jni::Object<Layer>> SymbolLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto& javaClass = jni::Class<SymbolLayer>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
        return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void SymbolLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<SymbolLayer>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<SymbolLayer>(
            env, javaClass, "nativePtr",
            jni::MakePeer<SymbolLayer, jni::String&, jni::String&>,
            "initialize",
            "finalize",
            METHOD(&SymbolLayer::getSymbolPlacement, "nativeGetSymbolPlacement"),
            METHOD(&SymbolLayer::getSymbolSpacing, "nativeGetSymbolSpacing"),
            METHOD(&SymbolLayer::getSymbolAvoidEdges, "nativeGetSymbolAvoidEdges"),
            METHOD(&SymbolLayer::getSymbolZOrder, "nativeGetSymbolZOrder"),
            METHOD(&SymbolLayer::getIconAllowOverlap, "nativeGetIconAllowOverlap"),
            METHOD(&SymbolLayer::getIconIgnorePlacement, "nativeGetIconIgnorePlacement"),
            METHOD(&SymbolLayer::getIconOptional, "nativeGetIconOptional"),
            METHOD(&SymbolLayer::getIconRotationAlignment, "nativeGetIconRotationAlignment"),
            METHOD(&SymbolLayer::getIconSize, "nativeGetIconSize"),
            METHOD(&SymbolLayer::getIconTextFit, "nativeGetIconTextFit"),
            METHOD(&SymbolLayer::getIconTextFitPadding, "nativeGetIconTextFitPadding"),
            METHOD(&SymbolLayer::getIconImage, "nativeGetIconImage"),
            METHOD(&SymbolLayer::getIconRotate, "nativeGetIconRotate"),
            METHOD(&SymbolLayer::getIconPadding, "nativeGetIconPadding"),
            METHOD(&SymbolLayer::getIconKeepUpright, "nativeGetIconKeepUpright"),
            METHOD(&SymbolLayer::getIconOffset, "nativeGetIconOffset"),
            METHOD(&SymbolLayer::getIconAnchor, "nativeGetIconAnchor"),
            METHOD(&SymbolLayer::getIconPitchAlignment, "nativeGetIconPitchAlignment"),
            METHOD(&SymbolLayer::getTextPitchAlignment, "nativeGetTextPitchAlignment"),
            METHOD(&SymbolLayer::getTextRotationAlignment, "nativeGetTextRotationAlignment"),
            METHOD(&SymbolLayer::getTextField, "nativeGetTextField"),
            METHOD(&SymbolLayer::getTextFont, "nativeGetTextFont"),
            METHOD(&SymbolLayer::getTextSize, "nativeGetTextSize"),
            METHOD(&SymbolLayer::getTextMaxWidth, "nativeGetTextMaxWidth"),
            METHOD(&SymbolLayer::getTextLineHeight, "nativeGetTextLineHeight"),
            METHOD(&SymbolLayer::getTextLetterSpacing, "nativeGetTextLetterSpacing"),
            METHOD(&SymbolLayer::getTextJustify, "nativeGetTextJustify"),
            METHOD(&SymbolLayer::getTextAnchor, "nativeGetTextAnchor"),
            METHOD(&SymbolLayer::getTextMaxAngle, "nativeGetTextMaxAngle"),
            METHOD(&SymbolLayer::getTextRotate, "nativeGetTextRotate"),
            METHOD(&SymbolLayer::getTextPadding, "nativeGetTextPadding"),
            METHOD(&SymbolLayer::getTextKeepUpright, "nativeGetTextKeepUpright"),
            METHOD(&SymbolLayer::getTextTransform, "nativeGetTextTransform"),
            METHOD(&SymbolLayer::getTextOffset, "nativeGetTextOffset"),
            METHOD(&SymbolLayer::getTextAllowOverlap, "nativeGetTextAllowOverlap"),
            METHOD(&SymbolLayer::getTextIgnorePlacement, "nativeGetTextIgnorePlacement"),
            METHOD(&SymbolLayer::getTextOptional, "nativeGetTextOptional"),
            METHOD(&SymbolLayer::getIconOpacityTransition, "nativeGetIconOpacityTransition"),
            METHOD(&SymbolLayer::setIconOpacityTransition, "nativeSetIconOpacityTransition"),
            METHOD(&SymbolLayer::getIconOpacity, "nativeGetIconOpacity"),
            METHOD(&SymbolLayer::getIconColorTransition, "nativeGetIconColorTransition"),
            METHOD(&SymbolLayer::setIconColorTransition, "nativeSetIconColorTransition"),
            METHOD(&SymbolLayer::getIconColor, "nativeGetIconColor"),
            METHOD(&SymbolLayer::getIconHaloColorTransition, "nativeGetIconHaloColorTransition"),
            METHOD(&SymbolLayer::setIconHaloColorTransition, "nativeSetIconHaloColorTransition"),
            METHOD(&SymbolLayer::getIconHaloColor, "nativeGetIconHaloColor"),
            METHOD(&SymbolLayer::getIconHaloWidthTransition, "nativeGetIconHaloWidthTransition"),
            METHOD(&SymbolLayer::setIconHaloWidthTransition, "nativeSetIconHaloWidthTransition"),
            METHOD(&SymbolLayer::getIconHaloWidth, "nativeGetIconHaloWidth"),
            METHOD(&SymbolLayer::getIconHaloBlurTransition, "nativeGetIconHaloBlurTransition"),
            METHOD(&SymbolLayer::setIconHaloBlurTransition, "nativeSetIconHaloBlurTransition"),
            METHOD(&SymbolLayer::getIconHaloBlur, "nativeGetIconHaloBlur"),
            METHOD(&SymbolLayer::getIconTranslateTransition, "nativeGetIconTranslateTransition"),
            METHOD(&SymbolLayer::setIconTranslateTransition, "nativeSetIconTranslateTransition"),
            METHOD(&SymbolLayer::getIconTranslate, "nativeGetIconTranslate"),
            METHOD(&SymbolLayer::getIconTranslateAnchor, "nativeGetIconTranslateAnchor"),
            METHOD(&SymbolLayer::getTextOpacityTransition, "nativeGetTextOpacityTransition"),
            METHOD(&SymbolLayer::setTextOpacityTransition, "nativeSetTextOpacityTransition"),
            METHOD(&SymbolLayer::getTextOpacity, "nativeGetTextOpacity"),
            METHOD(&SymbolLayer::getTextColorTransition, "nativeGetTextColorTransition"),
            METHOD(&SymbolLayer::setTextColorTransition, "nativeSetTextColorTransition"),
            METHOD(&SymbolLayer::getTextColor, "nativeGetTextColor"),
            METHOD(&SymbolLayer::getTextHaloColorTransition, "nativeGetTextHaloColorTransition"),
            METHOD(&SymbolLayer::setTextHaloColorTransition, "nativeSetTextHaloColorTransition"),
            METHOD(&SymbolLayer::getTextHaloColor, "nativeGetTextHaloColor"),
            METHOD(&SymbolLayer::getTextHaloWidthTransition, "nativeGetTextHaloWidthTransition"),
            METHOD(&SymbolLayer::setTextHaloWidthTransition, "nativeSetTextHaloWidthTransition"),
            METHOD(&SymbolLayer::getTextHaloWidth, "nativeGetTextHaloWidth"),
            METHOD(&SymbolLayer::getTextHaloBlurTransition, "nativeGetTextHaloBlurTransition"),
            METHOD(&SymbolLayer::setTextHaloBlurTransition, "nativeSetTextHaloBlurTransition"),
            METHOD(&SymbolLayer::getTextHaloBlur, "nativeGetTextHaloBlur"),
            METHOD(&SymbolLayer::getTextTranslateTransition, "nativeGetTextTranslateTransition"),
            METHOD(&SymbolLayer::setTextTranslateTransition, "nativeSetTextTranslateTransition"),
            METHOD(&SymbolLayer::getTextTranslate, "nativeGetTextTranslate"),
            METHOD(&SymbolLayer::getTextTranslateAnchor, "nativeGetTextTranslateAnchor"));
    }

} // namespace android
} // namespace mbgl
