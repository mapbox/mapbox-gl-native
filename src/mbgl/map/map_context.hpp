#ifndef MBGL_MAP_MAP_CONTEXT
#define MBGL_MAP_MAP_CONTEXT

#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/signal.hpp>

#include <vector>
#include <queue>
#include <future>

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

    // Starts the async handles.
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

    // These can only be called from the Map thread.
private:
    Worker& getWorker();
    util::ptr<Sprite> getSprite();
    void updateTiles();

    // TODO: Make these private
    public: void updateAnnotationTiles(const std::vector<TileID>& ids);

    // Triggered by triggerUpdate();
    private: void update();

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    public: void prepare();

    // Unconditionally performs a render with the current map state.
    public: void render();

    // Runs the enqueued tasks.
    private: void processTasks();

private:
    // Loads the style set in the data object. Called by Update::StyleInfo
    private: void reloadStyle();

    // Loads the actual JSON object an creates a new Style object.
    private: void loadStyleJSON(const std::string& json, const std::string& base);

    // TODO: Make all of these private
public:
    private: Environment& env;
    private: View& view;
    private: MapData& data;

    private: std::atomic<UpdateType> updated { static_cast<UpdateType>(Update::Nothing) };

    private: std::mutex mutexTask;
    private: std::queue<std::function<void()>> tasks;


    public: std::unique_ptr<uv::async> asyncUpdate;
    public: std::unique_ptr<uv::async> asyncRender;
    public: std::unique_ptr<uv::async> asyncInvoke;
    public: std::unique_ptr<uv::async> asyncTerminate;

    public: std::unique_ptr<Worker> workers;
    private: const std::unique_ptr<GlyphStore> glyphStore;
    private: const std::unique_ptr<GlyphAtlas> glyphAtlas;
    private: const std::unique_ptr<SpriteAtlas> spriteAtlas;
    private: const std::unique_ptr<LineAtlas> lineAtlas;
    private: const std::unique_ptr<TexturePool> texturePool;
    public: const std::unique_ptr<Painter> painter;
    public: util::ptr<Sprite> sprite;
    public: util::ptr<Style> style;

    // Used to signal that rendering completed.
    public: util::Signal rendered;

    // TODO: document this
    public: bool terminating = false;
};

}

#endif
