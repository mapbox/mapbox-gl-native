#pragma once

#include <mbgl/style/layer.hpp>

#include <vector>

namespace mbgl {
namespace style {
class LayerProperties;
}  // namespace style
class GeometryTileLayer;
class LayerFactory;
class RenderLayer;
class Bucket;
class BucketParameters;
class Layout;
class LayoutParameters;

/**
 * @brief A singleton class responsible for creating layer instances.
 * 
 * The LayerManager has implementation per platform. The LayerManager implementation
 * defines what layer types are available and it can also disable annotations.
 * 
 * Linker excludes the unreachable code for the disabled annotations and layers
 * from the binaries, significantly reducing their size.
 */
class LayerManager {
public:
    /**
     * @brief A singleton getter.
     * 
     * @return LayerManager* 
     */
    static LayerManager* get() noexcept;

    /// Returns a new Layer instance on success call; returns `nullptr` otherwise.
    std::unique_ptr<style::Layer> createLayer(const std::string& type, const std::string& id,
                                              const style::conversion::Convertible& value, style::conversion::Error& error) noexcept;
    /// Returns a new RenderLayer instance on success call; returns `nullptr` otherwise.
    std::unique_ptr<RenderLayer> createRenderLayer(Immutable<style::Layer::Impl>) noexcept;
    /// Returns a new Bucket instance on success call; returns `nullptr` otherwise.
    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<Immutable<style::LayerProperties>>&) noexcept;
    /// Returns a new Layout instance on success call; returns `nullptr` otherwise. 
    std::unique_ptr<Layout> createLayout(const LayoutParameters&, std::unique_ptr<GeometryTileLayer>,
                                         const std::vector<Immutable<style::LayerProperties>>&) noexcept;

    /**
     * @brief a build-time flag to enable/disable annotations in mapbox-gl-native core.
     * 
     * At the moment, the annotations implementation in core is creating concrete
     * layer instances apart from LayerManager/LayerFactory code path.
     * 
     * So, annotations must be disabled if the LayerManager implementation does
     * not provide line, fill or symbol layers (those, used by the annotations 
     * implementation).
     * 
     * Note: in future, annotations implemantation will be moved from the core to platform 
     * SDK (see https://github.com/mapbox/mapbox-plugins-android/tree/master/plugin-annotation)
     * and this flag won't be needed any more.
     */
    static const bool annotationsEnabled;

protected:
    virtual ~LayerManager() = default;
    virtual LayerFactory* getFactory(const std::string& type) noexcept = 0;
    virtual LayerFactory* getFactory(const style::LayerTypeInfo*) noexcept = 0;
};

} // namespace mbgl
