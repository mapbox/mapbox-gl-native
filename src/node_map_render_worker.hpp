#ifndef NODE_MBGL_NODE_MAP_RENDER_WORKER
#define NODE_MBGL_NODE_MAP_RENDER_WORKER

#include <nan.h>

#include "node_map.hpp"

namespace node_mbgl {

struct NodeMap::RenderOptions {
    double zoom = 0;
    double bearing = 0;
    double latitude = 0;
    double longitude = 0;
    unsigned int width = 512;
    unsigned int height = 512;
    float ratio = 1.0f;
    std::string accessToken;
    std::vector<std::string> classes;
};

class NodeMap::RenderWorker : public NanAsyncWorker {
public:
    RenderWorker(NodeMap *map, std::unique_ptr<RenderOptions> options, NanCallback *callback);
    ~RenderWorker();

    void Execute();
    void HandleOKCallback();
    void WorkComplete();

private:
    NodeMap *nodeMap;
    std::unique_ptr<RenderOptions> options;
    std::string image;
};

} // ns node_mbgl

#endif // NODE_MBGL_RENDER_WORKER
