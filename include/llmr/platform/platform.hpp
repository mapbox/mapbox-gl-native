#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

namespace llmr {

class tile;

class platform {
public:
    platform(void *obj) : obj(obj) {}

    const char *shaderSource(const char *name, const char *extension);
    void restart();
    void request(tile *tile);

private:
    void *obj;
};

}



#endif
