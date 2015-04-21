#ifndef MBGL_MAP_MAP_CONTEXT
#define MBGL_MAP_MAP_CONTEXT

#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/util/ptr.hpp>

#include <vector>
#include <queue>
#include <future>
#include <thread>

namespace uv {
class async;
}

namespace mbgl {

class Environment;
class View;
class MapData;
class GlyphStore;
class GlyphAtlas;
class SpriteAtlas;
class LineAtlas;
class TexturePool;
class Painter;
class Sprite;
class Style;
class Worker;

class MapContext {
public:
    MapContext(Environment&, View&, MapData&);
    ~MapContext();

    // Starts the map thread.
    void start();

    // Terminates the map thread
    void terminate();

    // Triggers a render. Can be called from any thread.
    void triggerRender();

    // Notifies the Map thread that the state has changed and an update might be necessary.
    void triggerUpdate(Update = Update::Nothing);

    // Executes the function on the thread of the MapContext.
    void invokeTask(std::function<void()>&& fn);

    // Executes the function on the thread of the MapContext and blocks until it
    // has been run.
    template <typename Fn> auto invokeSyncTask(const Fn& fn) -> decltype(fn()) {
        std::promise<decltype(fn())> promise;
        invokeTask([&fn, &promise] { promise.set_value(fn()); });
        return promise.get_future().get();
    }

    double getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol);
    void setSourceTileCacheSize(size_t size);
    void onLowMemory();

    // TODO: Make this private
    public: void updateAnnotationTiles(const std::vector<TileID>& ids);

    // These can only be called from the Map thread.
private:
    // Checks if render thread needs to pause
    void checkForPause();

    Worker& getWorker();
    util::ptr<Sprite> getSprite();
    void updateTiles();

    // Triggered by triggerUpdate();
    void update();

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    void prepare();

    // Unconditionally performs a render with the current map state.
    void render();

    // Runs the enqueued tasks.
    void processTasks();

    // Loads the style set in the data object. Called by Update::StyleInfo
    void reloadStyle();

    // Loads the actual JSON object an creates a new Style object.
    void loadStyleJSON(const std::string& json, const std::string& base);

    Environment& env;
    View& view;
    MapData& data;

    std::atomic<UpdateType> updated { static_cast<UpdateType>(Update::Nothing) };

    std::mutex mutexTask;
    std::queue<std::function<void()>> tasks;


    std::unique_ptr<uv::async> asyncUpdate;
    std::unique_ptr<uv::async> asyncRender;
    std::unique_ptr<uv::async> asyncInvoke;
    std::unique_ptr<uv::async> asyncTerminate;

    std::unique_ptr<Worker> workers;
    const std::unique_ptr<GlyphStore> glyphStore;
    const std::unique_ptr<GlyphAtlas> glyphAtlas;
    const std::unique_ptr<SpriteAtlas> spriteAtlas;
    const std::unique_ptr<LineAtlas> lineAtlas;
    const std::unique_ptr<TexturePool> texturePool;
    const std::unique_ptr<Painter> painter;

    util::ptr<Sprite> sprite;
    util::ptr<Style> style;

    size_t sourceCacheSize;
};

}

#endif
