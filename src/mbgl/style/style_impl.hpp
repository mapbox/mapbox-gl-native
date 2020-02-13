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
#include <mbgl/annotation/annotation_manager.hpp>

#include <mbgl/map/camera.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/geo.hpp>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

namespace mbgl {

class FileSource;
class AsyncRequest;
class SpriteLoader;

namespace style {

class StyleImpl : public SpriteLoaderObserver,
                    public SourceObserver,
                    public LayerObserver,
                    public LightObserver,
                    public util::noncopyable {
public:
    StyleImpl(std::shared_ptr<FileSource>, float pixelRatio);
    ~StyleImpl() override;

    void loadJSON(const std::string&);
    void loadURL(const std::string&);

    std::string getJSON() const;
    std::string getURL() const;

    void addObserver(Observer* observer);
    void removeObserver(Observer* observer);

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
    CameraOptions getDefaultCamera() const;

    TransitionOptions getTransitionOptions() const;
    void setTransitionOptions(const TransitionOptions&);

    void setLight(std::unique_ptr<Light>);
    Light* getLight() const;

    optional<Immutable<style::Image::Impl>> getImage(const std::string&) const;
    void addImage(std::unique_ptr<style::Image>);
    void removeImage(const std::string&);

    const std::string& getGlyphURL() const;

    using ImageImpls = std::vector<Immutable<Image::Impl>>;
    Immutable<ImageImpls> getImageImpls() const;
    Immutable<std::vector<Immutable<Source::Impl>>> getSourceImpls() const;
    Immutable<std::vector<Immutable<Layer::Impl>>> getLayerImpls() const;

    void dumpDebugLogs() const;

private:
    void parse(const std::string&);
public:
    bool mutated = false;
    bool loaded = false;
    bool spriteLoaded = false;
    AnnotationManager annotationManager; // ToDo: move annotations into an own style, stackable inside the View (=Map), so that
                                         // they can change independently of the rest of the style and be combined with different styles arbitrarily.
    std::shared_ptr<FileSource> fileSource;

private:
    std::string url;
    std::string json;

    std::unique_ptr<AsyncRequest> styleRequest;
    std::unique_ptr<SpriteLoader> spriteLoader;

    std::string glyphURL;
    Immutable<ImageImpls> images = makeMutable<ImageImpls>();
    CollectionWithPersistentOrder<Source> sources;
    Collection<Layer> layers;
    TransitionOptions transitionOptions;
    std::unique_ptr<Light> light;

    // Defaults
    std::string name;
    CameraOptions defaultCamera;
    std::set<Observer*> observers;

    // SpriteLoaderObserver implementation.
    void onSpriteLoaded(std::vector<Immutable<style::Image::Impl>>) override;
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

    std::exception_ptr lastError;
};

} // namespace style
} // namespace mbgl
