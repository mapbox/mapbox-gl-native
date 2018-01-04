#pragma once

#include <mbgl/style/transition_options.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/util/geo.hpp>

#include <string>
#include <vector>
#include <memory>

namespace mbgl {

class FileSource;
class Scheduler;

namespace style {

class Light;
class Image;
class Source;
class Layer;

class Style {
public:
    Style(Scheduler&, FileSource&, float pixelRatio);
    ~Style();

    void loadJSON(const std::string&);
    void loadURL(const std::string&);

    std::string getJSON() const;
    std::string getURL() const;

    // Defaults
    std::string getName() const;
    CameraOptions getDefaultCamera() const;

    // TransitionOptions
    TransitionOptions getTransitionOptions() const;
    void setTransitionOptions(const TransitionOptions&);

    // Light
          Light* getLight();
    const Light* getLight() const;

    void setLight(std::unique_ptr<Light>);

    // Images
    const Image* getImage(const std::string&) const;
    void addImage(std::unique_ptr<Image>);
    void removeImage(const std::string&);

    // Sources
    std::vector<      Source*> getSources();
    std::vector<const Source*> getSources() const;

          Source* getSource(const std::string&);
    const Source* getSource(const std::string&) const;

    void addSource(std::unique_ptr<Source>);
    std::unique_ptr<Source> removeSource(const std::string& sourceID);

    // Layers
    std::vector<      Layer*> getLayers();
    std::vector<const Layer*> getLayers() const;

          Layer* getLayer(const std::string&);
    const Layer* getLayer(const std::string&) const;

    void addLayer(std::unique_ptr<Layer>, const optional<std::string>& beforeLayerID = {});
    std::unique_ptr<Layer> removeLayer(const std::string& layerID);

    // Private implementation
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace style
} // namespace mbgl
