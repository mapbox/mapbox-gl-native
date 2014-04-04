#ifndef LLMR_MAP_TRANSFORM
#define LLMR_MAP_TRANSFORM

#include <llmr/util/vec.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/transition.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/map/tile.hpp>

#include <llmr/map/transform_commands.hpp>

#include <forward_list>

namespace llmr {

struct box;

    class Transform : private util::noncopyable {
public:
    Transform();

    void operator()(const TransformResizeCommand &cmd);
    void operator()(const TransformMoveByCommand &cmd, float duration);
    void operator()(const TransformScaleByCommand &cmd, float duration);
    void operator()(const TransformRotateByCommand &cmd, float duration);
    void operator()(const TransformLonLatCommand &cmd, float duration);
    void operator()(const TransformScaleCommand &cmd, float duration);
    void operator()(const TransformAngleCommand &cmd, float duration);
    void operator()(const TransformTransformCommand &cmd, float duration);

    // Animations
    bool isAnimating() const;
    bool needsTransitions() const;
    void updateTransitions();
    void cancelTransitions();

    // Getters
    void matrixFor(mat4& matrix, const Tile::ID& id) const;
    float getZoom() const;
    float getNormalizedZoom() const;
    int32_t getIntegerZoom() const;
    double getScale() const;
    double getAngle() const;
    void getLonLat(double& lon, double& lat) const;
    void getLonLatZoom(double& lon, double& lat, double& zoom) const;

    inline uint16_t getWidth() const { return width; }
    inline uint16_t getHeight() const { return height; }
    inline uint16_t getFramebufferWidth() const { return fb_width; }
    inline uint16_t getFramebufferHeight() const { return fb_height; }
    inline float getPixelRatio() const { return pixelRatio; }

    // Temporary
    box mapCornersToBox(uint32_t z) const;

private:
    void setScaleXY(double new_scale, double xn, double yn, float duration = 0);
    double pixel_x() const;
    double pixel_y() const;

private:
    // logical dimensions
    uint16_t width = 0;
    uint16_t height = 0;

    // physical (framebuffer) dimensions
    uint16_t fb_width = 0;
    uint16_t fb_height = 0;

    // Ratio of physical pixels to logical pixels.
    float pixelRatio = 1;

    // position/orientation
    double x = 0, y = 0; // pixel values of the map center in the current scale
    double angle = 0;
    double scale = 1;

    const double min_scale = pow(2, 0);
    const double max_scale = pow(2, 20);

    // cache values for spherical mercator math
    double zc, Bc, Cc;

    // Animations
    std::forward_list<std::shared_ptr<util::transition>> transitions;
    std::shared_ptr<util::transition> transform_timeout;

    // This is true while the user is transforming the map (e.g. panning,
    // zooming, rotating). Once the user stops, or a timeout occurs, this will
    // be reset to false.
    bool transforming = false;
};

}

#endif
