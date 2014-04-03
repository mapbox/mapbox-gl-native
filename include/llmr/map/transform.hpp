#ifndef LLMR_MAP_TRANSFORM
#define LLMR_MAP_TRANSFORM

#include <llmr/util/vec.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/animation.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/map/tile.hpp>

#include <llmr/map/transform_commands.hpp>

#include <forward_list>

namespace llmr {

struct box;

    class Transform : private util::noncopyable {
public:
    Transform();

    void operator()(const TransformMoveByCommand &cmd, float duration);
    void operator()(const TransformScaleByCommand &cmd, float duration);
    void operator()(const TransformRotateByCommand &cmd, float duration);
    void operator()(const TransformLonLatCommand &cmd, float duration);
    void operator()(const TransformScaleCommand &cmd, float duration);
    void operator()(const TransformAngleCommand &cmd, float duration);
    void operator()(const TransformResizeCommand &cmd);

    // // Animations
    // bool needsAnimation() const;
    // void updateAnimations();
    // void cancelAnimations();

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

    inline bool isAnimating() const {
        return rotating || scaling || panning;
    }

    // // Animations
    // void startPanning();
    // void stopPanning();
    // void startRotating();
    // void stopRotating();
    // void startScaling();
    // void stopScaling();

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

    float pixelRatio = 1;

    bool rotating = false;
    bool scaling = false;
    bool panning = false;

private:
    double x = 0, y = 0; // pixel values of the map center in the current scale
    double angle = 0;
    double scale = 1;

    double min_scale = pow(2, 0);
    double max_scale = pow(2, 20);

    // cache values for spherical mercator math
    double zc, Bc, Cc;

    // std::forward_list<std::shared_ptr<util::animation>> animations;
    // std::shared_ptr<util::animation> scale_timeout;
    // std::shared_ptr<util::animation> rotate_timeout;
    // std::shared_ptr<util::animation> pan_timeout;
};

}

#endif
