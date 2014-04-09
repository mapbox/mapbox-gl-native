#ifndef LLMR_MAP_TRANSFORM_COMMANDS
#define LLMR_MAP_TRANSFORM_COMMANDS

#include <cstdint>

namespace llmr {

struct TransformNullCommand {
};

struct TransformMoveByCommand {
    int16_t delta_x, delta_y;
};

struct TransformScaleByCommand {
    double delta;
    int16_t center_x, center_y;
};

struct TransformRotateByCommand {
    int16_t anchor_x, anchor_y;
    int16_t start_x, start_y;
    int16_t end_x, end_y;
};

struct TransformLonLatCommand {
    double lon, lat;
};

struct TransformScaleCommand {
    double scale;
    int16_t center_x, center_y;
};

struct TransformAngleCommand {
    double angle;
};

struct TransformResizeCommand {
    uint16_t width, height;
    uint16_t fb_width, fb_height;
};

struct TransformTransformCommand {
    bool value;
};

struct TransformCommand {
    enum Type : uint8_t { Null, MoveBy, ScaleBy, RotateBy, LonLat, Scale, Angle, Resize, Transform } type;
    float duration; // milliseconds
    union {
        TransformMoveByCommand moveBy;
        TransformScaleByCommand scaleBy;
        TransformRotateByCommand rotateBy;
        TransformLonLatCommand lonLat;
        TransformScaleCommand scale;
        TransformAngleCommand angle;
        TransformResizeCommand resize;
        TransformTransformCommand transform;
    };

    inline TransformCommand()
        : type(Null), duration(0) {}
    inline TransformCommand(const TransformMoveByCommand& moveBy, float duration = 0) :
        type(MoveBy), duration(duration), moveBy(moveBy) {}
    inline TransformCommand(const TransformScaleByCommand& scaleBy, float duration = 0) :
        type(ScaleBy), duration(duration), scaleBy(scaleBy) {}
    inline TransformCommand(const TransformRotateByCommand& rotateBy, float duration = 0) :
        type(RotateBy), duration(duration), rotateBy(rotateBy) {}
    inline TransformCommand(const TransformLonLatCommand& lonLat, float duration = 0) :
        type(LonLat), duration(duration), lonLat(lonLat) {}
    inline TransformCommand(const TransformScaleCommand& scale, float duration = 0) :
        type(Scale), duration(duration), scale(scale) {}
    inline TransformCommand(const TransformAngleCommand& angle, float duration = 0) :
        type(Angle), duration(duration), angle(angle) {}
    inline TransformCommand(const TransformResizeCommand& resize, float duration = 0) :
        type(Resize), duration(duration), resize(resize) {}
    inline TransformCommand(const TransformTransformCommand& transform, float duration = 0) :
        type(Transform), duration(duration), transform(transform) {}
};

}

#endif

