#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/layer_impl.hpp>

#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

namespace mbgl {

class PaintParameters;
class TransformState;
class RenderTile;
class RenderLayer;
class RenderedQueryOptions;
class SourceQueryOptions;
class Tile;
class RenderSourceObserver;
class TileParameters;
class CollisionIndex;

class RenderSource : protected TileObserver {
public:
    static std::unique_ptr<RenderSource> create(Immutable<style::Source::Impl>);

    // Check whether this source is of the given subtype.
    template <class T>
    bool is() const;

    // Dynamically cast this source to the given subtype.
    template <class T>
    T* as() {
        return is<T>() ? reinterpret_cast<T*>(this) : nullptr;
    }

    template <class T>
    const T* as() const {
        return is<T>() ? reinterpret_cast<const T*>(this) : nullptr;
    }

    bool isEnabled() const;
    virtual bool isLoaded() const = 0;

    virtual void update(Immutable<style::Source::Impl>,
                        const std::vector<Immutable<style::Layer::Impl>>&,
                        bool needsRendering,
                        bool needsRelayout,
                        const TileParameters&) = 0;

    virtual void startRender(PaintParameters&) = 0;
    virtual void finishRender(PaintParameters&) = 0;

    // Returns an unsorted list of RenderTiles.
    virtual std::vector<std::reference_wrapper<RenderTile>> getRenderTiles() = 0;

    virtual std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::vector<const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const = 0;

    virtual std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const = 0;

    virtual void reduceMemoryUse() = 0;

    virtual void dumpDebugLogs() const = 0;

    void setObserver(RenderSourceObserver*);

    Immutable<style::Source::Impl> baseImpl;

protected:
    RenderSource(Immutable<style::Source::Impl>);
    RenderSourceObserver* observer;

    bool enabled = false;

    void onTileChanged(Tile&) override;
    void onTileError(Tile&, std::exception_ptr) final;
};

} // namespace mbgl
