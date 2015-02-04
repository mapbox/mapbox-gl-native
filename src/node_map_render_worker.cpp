#include "node_map_render_worker.hpp"
#include "node_file_source.hpp"

#include <mbgl/util/image.hpp>

namespace node_mbgl {

NodeMap::RenderWorker::RenderWorker(NodeMap *nodeMap_, std::unique_ptr<RenderOptions> options_,
                           NanCallback *callback_)
    : NanAsyncWorker(callback_), nodeMap(nodeMap_), options(std::move(options_)) {
    nodeMap->Ref();
}

NodeMap::RenderWorker::~RenderWorker() {
    nodeMap->Unref();
}

void NodeMap::RenderWorker::Execute() {
    try {
        nodeMap->view.resize(options->width, options->height, options->ratio);
        nodeMap->map.setAccessToken(options->accessToken);
        nodeMap->map.resize(options->width, options->height, options->ratio);
        nodeMap->map.setClasses(options->classes);
        nodeMap->map.setLonLatZoom(options->longitude, options->latitude, options->zoom);
        nodeMap->map.setBearing(options->bearing);

        // Run the loop. It will terminate when we don't have any further listeners.
        nodeMap->map.run();

        const unsigned int width = options->width * options->ratio;
        const unsigned int height = options->height * options->ratio;
        auto pixels = nodeMap->view.readPixels();
        image = mbgl::util::compress_png(width, height, pixels.get());
    } catch (mbgl::exception &ex) {
        SetErrorMessage(ex.what());
    }
}

void NodeMap::RenderWorker::WorkComplete() {
    NanAsyncWorker::WorkComplete();

    // Continue processing remaining items in the queue.
    nodeMap->processNext();
}

void NodeMap::RenderWorker::HandleOKCallback() {
    NanScope();

    v8::Local<v8::Value> argv[] = {
        NanNull(),
        NanNewBufferHandle(image.data(), image.length())
    };

    callback->Call(2, argv);
};

} // ns node_mbgl
