// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "symbol_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace android {

    inline mbgl::style::SymbolLayer& toSymbolLayer(mbgl::style::Layer& layer) {
        return static_cast<mbgl::style::SymbolLayer&>(layer);
    }

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    SymbolLayer::SymbolLayer(jni::JNIEnv& env, jni::String& layerId, jni::String& sourceId)
        : Layer(std::make_unique<mbgl::style::SymbolLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
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
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getSymbolPlacement()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getSymbolSpacing(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getSymbolSpacing()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getSymbolAvoidEdges(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getSymbolAvoidEdges()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getSymbolSortKey(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getSymbolSortKey()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getSymbolZOrder(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getSymbolZOrder()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconAllowOverlap(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconAllowOverlap()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconIgnorePlacement(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconIgnorePlacement()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconOptional(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconOptional()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconRotationAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconRotationAlignment()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconSize(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconSize()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconTextFit(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconTextFit()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconTextFitPadding(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconTextFitPadding()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconImage(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconImage()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconRotate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconRotate()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconPadding(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconPadding()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconKeepUpright(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconKeepUpright()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconOffset(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconOffset()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconAnchor()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconPitchAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconPitchAlignment()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextPitchAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextPitchAlignment()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextRotationAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextRotationAlignment()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextField(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextField()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextFont(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextFont()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextSize(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextSize()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextMaxWidth(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextMaxWidth()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextLineHeight(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextLineHeight()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextLetterSpacing(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextLetterSpacing()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextJustify(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextJustify()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextRadialOffset(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextRadialOffset()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextVariableAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextVariableAnchor()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextAnchor()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextMaxAngle(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextMaxAngle()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextWritingMode(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextWritingMode()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextRotate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextRotate()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextPadding(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextPadding()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextKeepUpright(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextKeepUpright()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextTransform(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextTransform()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextOffset(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextOffset()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextAllowOverlap(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextAllowOverlap()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextIgnorePlacement(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextIgnorePlacement()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextOptional(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextOptional()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getIconOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setIconOpacityTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getIconColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setIconColorTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconHaloColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconHaloColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconHaloColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getIconHaloColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconHaloColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setIconHaloColorTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconHaloWidth(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconHaloWidth()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconHaloWidthTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getIconHaloWidthTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconHaloWidthTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setIconHaloWidthTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconHaloBlur(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconHaloBlur()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconHaloBlurTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getIconHaloBlurTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconHaloBlurTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setIconHaloBlurTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconTranslate()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getIconTranslateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getIconTranslateTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setIconTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setIconTranslateTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getIconTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getIconTranslateAnchor()));
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getTextOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setTextOpacityTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getTextColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setTextColorTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextHaloColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextHaloColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextHaloColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getTextHaloColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextHaloColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setTextHaloColorTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextHaloWidth(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextHaloWidth()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextHaloWidthTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getTextHaloWidthTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextHaloWidthTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setTextHaloWidthTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextHaloBlur(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextHaloBlur()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextHaloBlurTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getTextHaloBlurTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextHaloBlurTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setTextHaloBlurTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextTranslate()));
    }

    jni::Local<jni::Object<TransitionOptions>> SymbolLayer::getTextTranslateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toSymbolLayer(layer).getTextTranslateTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void SymbolLayer::setTextTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toSymbolLayer(layer).setTextTranslateTransition(options);
    }

    jni::Local<jni::Object<>> SymbolLayer::getTextTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toSymbolLayer(layer).getTextTranslateAnchor()));
    }


    // SymbolJavaLayerPeerFactory

    SymbolJavaLayerPeerFactory::~SymbolJavaLayerPeerFactory() = default;

    namespace {
        jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv& env, Layer* layer) {
            static auto& javaClass = jni::Class<SymbolLayer>::Singleton(env);
            static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
            return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(layer));
        }
    }  // namespace

    jni::Local<jni::Object<Layer>> SymbolJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Layer& layer) {
        assert(layer.baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new SymbolLayer(map, toSymbolLayer(layer)));
    }

    jni::Local<jni::Object<Layer>> SymbolJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
        assert(layer->baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new SymbolLayer(map, std::unique_ptr<mbgl::style::SymbolLayer>(static_cast<mbgl::style::SymbolLayer*>(layer.release()))));
    }

    void SymbolJavaLayerPeerFactory::registerNative(jni::JNIEnv& env) {
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
            METHOD(&SymbolLayer::getSymbolSortKey, "nativeGetSymbolSortKey"),
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
            METHOD(&SymbolLayer::getTextRadialOffset, "nativeGetTextRadialOffset"),
            METHOD(&SymbolLayer::getTextVariableAnchor, "nativeGetTextVariableAnchor"),
            METHOD(&SymbolLayer::getTextAnchor, "nativeGetTextAnchor"),
            METHOD(&SymbolLayer::getTextMaxAngle, "nativeGetTextMaxAngle"),
            METHOD(&SymbolLayer::getTextWritingMode, "nativeGetTextWritingMode"),
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
