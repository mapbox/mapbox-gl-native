#pragma once

#include <mbgl/style/style.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/observer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/light_observer.hpp>
#include <mbgl/sprite/sprite_loader_observer.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/collection.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/geo.hpp>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace mbgl {

class Scheduler;
class FileSource;
class SpriteLoader;

namespace style {

class Style::Impl : public SpriteLoaderObserver,
                    public SourceObserver,
                    public LayerObserver,
                    public LightObserver,
                    public util::noncopyable {
public:
    Impl(Scheduler&, FileSource&, float pixelRatio);
    ~Impl() override;

    void setJSON(const std::string&);

    void setObserver(Observer*);

    bool isLoaded() const;

    std::exception_ptr getLastError() const {
        return lastError;
    }

    std::vector<      Source*> getSources();
    std::vector<const Source*> getSources() const;
    Source* getSource(const std::string& id) const;

    void addSource(std::unique_ptr<Source>);
    std::unique_ptr<Source> removeSource(const std::string& sourceID);

    std::vector<      Layer*> getLayers();
    std::vector<const Layer*> getLayers() const;
    Layer* getLayer(const std::string& id) const;

    Layer* addLayer(std::unique_ptr<Layer>,
                    optional<std::string> beforeLayerID = {});
    std::unique_ptr<Layer> removeLayer(const std::string& layerID);

    std::string getName() const;
    LatLng getDefaultLatLng() const;
    double getDefaultZoom() const;
    double getDefaultBearing() const;
    double getDefaultPitch() const;

    TransitionOptions getTransitionOptions() const;
    void setTransitionOptions(const TransitionOptions&);

    void setLight(std::unique_ptr<Light>);
    Light* getLight() const;

    const style::Image* getImage(const std::string&) const;
    void addImage(std::unique_ptr<style::Image>);
    void removeImage(const std::string&);

    const std::string& getGlyphURL() const;

    Immutable<std::vector<Immutable<Image::Impl>>> getImageImpls() const;
    Immutable<std::vector<Immutable<Source::Impl>>> getSourceImpls() const;
    Immutable<std::vector<Immutable<Layer::Impl>>> getLayerImpls() const;

    void dumpDebugLogs() const;

    bool mutated = false;
    bool loaded = false;
    bool spriteLoaded = false;

private:
    Scheduler& scheduler;
    FileSource& fileSource;
    std::unique_ptr<SpriteLoader> spriteLoader;
    std::string glyphURL;

    Collection<style::Image> images;
    Collection<Source> sources;
    Collection<Layer> layers;
    TransitionOptions transitionOptions;
    std::unique_ptr<Light> light;

    // Defaults
    std::string name;
    LatLng defaultLatLng;
    double defaultZoom = 0;
    double defaultBearing = 0;
    double defaultPitch = 0;

    // SpriteLoaderObserver implementation.
    void onSpriteLoaded(std::vector<std::unique_ptr<Image>>&&) override;
    void onSpriteError(std::exception_ptr) override;

    // SourceObserver implementation.
    void onSourceLoaded(Source&) override;
    void onSourceChanged(Source&) override;
    void onSourceError(Source&, std::exception_ptr) override;
    void onSourceDescriptionChanged(Source&) override;

    // LayerObserver implementation.
    void onLayerChanged(Layer&) override;

    // LightObserver implementation.
    void onLightChanged(const Light&) override;

    Observer nullObserver;
    Observer* observer = &nullObserver;

    std::exception_ptr lastError;
};

} // namespace style
} // namespace mbgl
