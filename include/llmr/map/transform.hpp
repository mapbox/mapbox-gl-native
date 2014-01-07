#ifndef LLMR_MAP_TRANSFORM
#define LLMR_MAP_TRANSFORM

namespace llmr {

class tile;

class transform {
public:
    transform();

    void moveBy(double dx, double dy);
    void scaleBy(double ds, double cx, double cy);

public:
    double x, y;
    double scale;
};

}

#endif
