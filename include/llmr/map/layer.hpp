#ifndef LLMR_MAP_LAYER
#define LLMR_MAP_LAYER

#include <string>

namespace llmr {

class Bucket;
class Tile;

class Layer {
public:
    Layer(const std::string& name, const std::shared_ptr<Bucket>& bucket)
        : name(name),
          bucket(bucket) {}

public:
    std::string name;
    std::shared_ptr<Bucket> bucket;

private:
    // Make noncopyable
    Layer(const Layer&) = delete;
    Layer(const Layer&&) = delete;
    Layer& operator=(const Layer&) = delete;
    Layer& operator=(const Layer && ) = delete;
};

}

#endif
