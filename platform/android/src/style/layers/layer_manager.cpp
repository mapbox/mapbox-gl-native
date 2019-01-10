#include "layer_manager.hpp"

#include <mbgl/style/layer_impl.hpp>

#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {

namespace android {

LayerManagerAndroid::LayerManagerAndroid() {
   initialize();
}

LayerManagerAndroid::~LayerManagerAndroid() = default;

jni::Local<jni::Object<Layer>> LayerManagerAndroid::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Layer& layer) {
    if (JavaLayerPeerFactory* factory = getPeerFactory(layer.getTypeInfo())) {
        return factory->createJavaLayerPeer(env, map, layer);
    }
    return jni::Local<jni::Object<Layer>>();
}

jni::Local<jni::Object<Layer>> LayerManagerAndroid::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
    if (JavaLayerPeerFactory* factory = getPeerFactory(layer->getTypeInfo())) {
        return factory->createJavaLayerPeer(env, map, std::move(layer));
    }
    return jni::Local<jni::Object<Layer>>();
}

void LayerManagerAndroid::registerNative(jni::JNIEnv& env) {
    if (peerFactories.empty()) {
        return;
    }

    Layer::registerNative(env);
    for (const auto& factory: peerFactories) {
        factory->registerNative(env);
    }
}

void LayerManagerAndroid::addLayerType(std::unique_ptr<JavaLayerPeerFactory> factory) {
    assert(getFactory(factory->getLayerFactory()->getTypeInfo()) == nullptr);
    registerCoreFactory(factory->getLayerFactory());
    peerFactories.emplace_back(std::move(factory));
}

void LayerManagerAndroid::addLayerTypeCoreOnly(std::unique_ptr<LayerFactory> factory) {
    assert(getFactory(factory->getTypeInfo()) == nullptr);
    registerCoreFactory(factory.get());
    coreFactories.emplace_back(std::move(factory));
}

void LayerManagerAndroid::registerCoreFactory(mbgl::LayerFactory* factory) {
    std::string type{factory->getTypeInfo()->type};
    if (!type.empty()) {
        assert(typeToFactory.find(type) == typeToFactory.end());
        typeToFactory.emplace(std::make_pair(std::move(type), factory));
    }
}

JavaLayerPeerFactory* LayerManagerAndroid::getPeerFactory(const mbgl::style::LayerTypeInfo* typeInfo) {
    assert(typeInfo);
    for (const auto& factory: peerFactories) {
        if (factory->getLayerFactory()->getTypeInfo() == typeInfo) {
            return factory.get();
        }
    }
    return nullptr;
}

LayerFactory* LayerManagerAndroid::getFactory(const std::string& type) noexcept {
    auto search = typeToFactory.find(type);
    return (search != typeToFactory.end()) ? search->second : nullptr;
}

LayerFactory* LayerManagerAndroid::getFactory(const mbgl::style::LayerTypeInfo* info) noexcept {
    if (JavaLayerPeerFactory* peerFactory = getPeerFactory(info)) {
        return peerFactory->getLayerFactory();
    }

    for (const auto& factory: coreFactories) {
        if (factory->getTypeInfo() == info) {
            return factory.get();
        }
    }

    return nullptr;
}

// static 
LayerManagerAndroid* LayerManagerAndroid::get() noexcept {
    static LayerManagerAndroid impl;
    return &impl;
}

} // namespace android

LayerManager* LayerManager::get() noexcept {
    return android::LayerManagerAndroid::get();
}

} // namespace mbgl
