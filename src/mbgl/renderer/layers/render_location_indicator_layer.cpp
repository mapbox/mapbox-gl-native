#include <array>
#include <mapbox/cheap_ruler.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/platform/gl_functions.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/layers/render_location_indicator_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/style/layers/location_indicator_layer.hpp>
#include <mbgl/style/layers/location_indicator_layer_impl.hpp>
#include <mbgl/style/layers/location_indicator_layer_properties.hpp>
#include <mbgl/util/mat4.hpp>

#include <mapbox/eternal.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/defines.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/texture_resource.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/platform/gl_functions.hpp>
#include <mbgl/renderer/image_manager.hpp>

using namespace mbgl::platform;
namespace mbgl {

struct LocationIndicatorRenderParameters {
    LocationIndicatorRenderParameters() = default;
    explicit LocationIndicatorRenderParameters(const TransformParameters& tp) : state(&tp.state) {}
    LocationIndicatorRenderParameters(const LocationIndicatorRenderParameters& o) = default;
    LocationIndicatorRenderParameters& operator=(const LocationIndicatorRenderParameters& o) = default;

    double width = 0.0;
    double height = 0.0;
    double latitude = 0.0;
    double longitude = 0.0;
    double zoom = 0.0;
    double bearing = 0.0;
    double pitch = 0.0;
    std::array<double, 16> projectionMatrix;
    const TransformState* state = nullptr;
    ImageManager* imageManager = nullptr;
    // some testing defaults, for before it gets updated via props
    double puckBearing = 0.0;
    LatLng puckPosition = {0, 0};
    double errorRadiusMeters;
    mbgl::Color errorRadiusColor{0, 0, 0, 0};
    mbgl::Color errorRadiusBorderColor{0, 0, 0, 0};
    float puckScale = 0;
    float puckHatScale = 0;
    float puckShadowScale = 0;
    float puckLayersDisplacement = 0;
    float perspectiveCompensation = 0;
    std::string puckImagePath;
    std::string puckShadowImagePath;
    std::string puckHatImagePath;
};

class RenderLocationIndicatorImpl {
protected:
    struct vec2 {
        GLfloat x = 0.0f;
        GLfloat y = 0.0f;

        vec2(GLfloat x_, GLfloat y_) : x(x_), y(y_) {}
        vec2() = default;
        explicit vec2(const Point<double>& p) : x(p.x), y(p.y) {}
        vec2(const vec2& o) = default;
        vec2(vec2&& o) = default;
        vec2& operator=(vec2&& o) = default;
        vec2& operator=(const vec2& o) = default;
        float length() const { return std::sqrt(x * x + y * y); }
        vec2 normalized() const {
            const float size = length();
            return {x / size, y / size};
        }
        void normalize() { *this = normalized(); }
        vec2 mirrored(const vec2& mirror) const {
            float k = dot(mirror) / mirror.length();
            return 2.0 * k * mirror - (*this);
        }
        float dot(const vec2& v2) const { return x * v2.x + y * v2.y; }
        vec2 rotated(float degrees) const {
            const float cs = std::cos(degrees * util::DEG2RAD);
            const float sn = std::sin(degrees * util::DEG2RAD);
            return vec2{x * cs + y * sn, x * sn + y * cs}.normalized();
        }
        float bearing() const {
            const vec2 norm = normalized();

            // From theta to bearing
            return util::wrap<float>(M_PI_2 - std::atan2(-norm.y, norm.x), 0, M_PI * 2.0) * util::RAD2DEG;
        }
        Point<double> toPoint() const { return {x, y}; }

        friend vec2 operator-(const vec2& v) { return {-v.x, -v.y}; }
        friend vec2 operator*(double a, const vec2& v) { return {GLfloat(v.x * a), GLfloat(v.y * a)}; }
        friend vec2 operator+(const vec2& v1, const vec2& v2) { return {v1.x + v2.x, v1.y + v2.y}; }
        friend vec2 operator-(const vec2& v1, const vec2& v2) { return {v1.x - v2.x, v1.y - v2.y}; }
    };

    struct Shader {
        virtual ~Shader() { release(); }
        void release() {
            if (!program) return;
            MBGL_CHECK_ERROR(glDetachShader(program, vertexShader));
            MBGL_CHECK_ERROR(glDetachShader(program, fragmentShader));
            MBGL_CHECK_ERROR(glDeleteShader(vertexShader));
            MBGL_CHECK_ERROR(glDeleteShader(fragmentShader));
            MBGL_CHECK_ERROR(glDeleteProgram(program));
            program = vertexShader = fragmentShader = 0;
        }
        void initialize(const GLchar* const& vsSource, const GLchar* const& fsSource) {
            if (program) return;
            program = MBGL_CHECK_ERROR(glCreateProgram());
            vertexShader = MBGL_CHECK_ERROR(glCreateShader(GL_VERTEX_SHADER));
            fragmentShader = MBGL_CHECK_ERROR(glCreateShader(GL_FRAGMENT_SHADER));
            MBGL_CHECK_ERROR(glShaderSource(vertexShader, 1, &vsSource, nullptr));
            MBGL_CHECK_ERROR(glCompileShader(vertexShader));
            MBGL_CHECK_ERROR(glAttachShader(program, vertexShader));
            MBGL_CHECK_ERROR(glShaderSource(fragmentShader, 1, &fsSource, nullptr));
            MBGL_CHECK_ERROR(glCompileShader(fragmentShader));
            MBGL_CHECK_ERROR(glAttachShader(program, fragmentShader));
            MBGL_CHECK_ERROR(glLinkProgram(program));
            pullLocations();
        }
        virtual void bind() { MBGL_CHECK_ERROR(glUseProgram(program)); }
        void detach() { MBGL_CHECK_ERROR(glUseProgram(0)); }
        virtual void pullLocations(){};

        GLuint program = 0;
        GLuint vertexShader = 0;
        GLuint fragmentShader = 0;
    };

    struct SimpleShader : public Shader {
        // Note that custom layers need to draw geometry with a z value of 1 to take advantage of
        // depth-based fragment culling.
        const GLchar* vertexShaderSource = R"MBGL_SHADER(
#ifdef GL_ES
precision highp float;
#endif

attribute vec2 a_pos;
uniform mat4 u_matrix;
void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}
)MBGL_SHADER";

        const GLchar* fragmentShaderSource = R"MBGL_SHADER(
#ifdef GL_ES
precision highp float;
#endif

uniform vec4 u_color;
void main() {
    gl_FragColor = u_color;
}
)MBGL_SHADER";

        void initialize() { Shader::initialize(SimpleShader::vertexShaderSource, SimpleShader::fragmentShaderSource); }

        void pullLocations() override {
            a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
            u_color = MBGL_CHECK_ERROR(glGetUniformLocation(program, "u_color"));
            u_matrix = MBGL_CHECK_ERROR(glGetUniformLocation(program, "u_matrix"));
        }
        void bind() override {
            SimpleShader::initialize();
            Shader::bind();
        }

        GLuint a_pos = 0;
        GLuint u_color = 0;
        GLuint u_matrix = 0;
    };

    struct TexturedShader : public Shader {
        const GLchar* vertexShaderSource = R"MBGL_SHADER(
#ifdef GL_ES
precision highp float;
#endif

attribute vec2 a_pos;
attribute vec2 a_texCoord;
uniform mat4 u_matrix;
varying vec2 v_texCoord;
void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    v_texCoord = a_texCoord;
}
)MBGL_SHADER";

        const GLchar* fragmentShaderSource = R"MBGL_SHADER(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_image;
varying vec2 v_texCoord;
void main() {
    vec4 color = texture2D(u_image, v_texCoord);
    gl_FragColor = color;
}
)MBGL_SHADER";

        void initialize() { Shader::initialize(vertexShaderSource, fragmentShaderSource); }

        void pullLocations() override {
            a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
            a_texCoord = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_texCoord"));
            u_image = MBGL_CHECK_ERROR(glGetUniformLocation(program, "u_image"));
            u_matrix = MBGL_CHECK_ERROR(glGetUniformLocation(program, "u_matrix"));
        }
        void bind() override {
            TexturedShader::initialize();
            Shader::bind();
        }

        GLuint a_pos = 0;
        GLuint a_texCoord = 0;
        GLuint u_image = 0;
        GLuint u_matrix = 0;
    };

    struct Buffer {
        virtual ~Buffer() { release(); }
        void release() {
            if (!bufferId) return;
            MBGL_CHECK_ERROR(glDeleteBuffers(1, &bufferId));
            bufferId = 0;
        }
        void initialize() {
            if (!bufferId) MBGL_CHECK_ERROR(glGenBuffers(1, &bufferId));
        }
        void bind(const GLenum target = GL_ARRAY_BUFFER) {
            initialize();
            MBGL_CHECK_ERROR(glBindBuffer(target, bufferId));
        }
        void detach(const GLenum target = GL_ARRAY_BUFFER) { MBGL_CHECK_ERROR(glBindBuffer(target, 0)); }
        template <typename T, std::size_t N>
        void upload(const std::array<T, N>& data) {
            bind();
            MBGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, N * sizeof(T), data.data(), GL_STATIC_DRAW));
            size = static_cast<unsigned int>(N * sizeof(T));
            elements = N;
        }
        template <typename T>
        void upload(const std::vector<T>& data) {
            bind();
            MBGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW));
            size = data.size() * sizeof(T);
            elements = data.size();
        }

        GLuint bufferId = 0;
        unsigned int size = 0;
        unsigned int elements = 0;
    };

public:
    struct Texture {
        ~Texture() { release(); }
        void release() {
            MBGL_CHECK_ERROR(glDeleteTextures(1, &texId));
            image = nullptr;
        }
        /*
            Assign can be called any time. Conversely, upload must be called with a bound gl context.
        */
        void assign(const Immutable<style::Image::Impl>* img) {
            if ((img && &img->get()->image == image) || (!img && !image)) return;
            imageDirty = true;
            image = (img) ? &img->get()->image : nullptr;
            width = height = 0;
            pixelRatio = 1.0f;
            if (img) {
                sharedImage = *img; // keep reference until uploaded
                width = image->size.width;
                height = image->size.height;
                pixelRatio = img->get()->pixelRatio;
            } else {
                sharedImage = nullopt;
            }
        }

        void upload() {
            if (!imageDirty) return;
            imageDirty = false;
            initialize();

            MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texId));
            if (!image || !image->valid()) {
                MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
            } else {
                MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D,
                                              0,
                                              GL_RGBA,
                                              image->size.width,
                                              image->size.height,
                                              0,
                                              GL_RGBA,
                                              GL_UNSIGNED_BYTE,
                                              image->data.get()));
                MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
                MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
                MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
                MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
                MBGL_CHECK_ERROR(glGenerateMipmap(GL_TEXTURE_2D));
                if (RenderLocationIndicatorImpl::anisotropicFilteringAvailable)
                    MBGL_CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16));
            }
            detach();
            sharedImage = nullopt;
        }
        void initialize() {
            if (texId != 0) return;
            MBGL_CHECK_ERROR(glGenTextures(1, &texId));
        }
        void bind(int textureUnit = -1) {
            initialize();
            if (!image && !imageDirty) return;

            upload();
            if (textureUnit >= 0) MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + textureUnit));
            MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texId));
        }
        void detach() { MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, 0)); }
        bool isValid() { return imageDirty || image; }
        GLuint texId = 0;
        const mbgl::PremultipliedImage* image = nullptr;
        optional<Immutable<style::Image::Impl>> sharedImage;
        bool imageDirty = false;
        size_t width = 0;
        size_t height = 0;
        float pixelRatio = 1.0f;
    };

    RenderLocationIndicatorImpl() : ruler(0, mapbox::cheap_ruler::CheapRuler::Meters) {}

    static bool hasAnisotropicFiltering() {
        const auto* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) { // glGetString(GL_EXTENSIONS) is deprecated in OpenGL Desktop 3.0+. But OpenGL 3.0+
                                    // has anisotropic filtering.
            return true;
        } else {
            if (strstr(extensions, "GL_EXT_texture_filter_anisotropic") != nullptr) return true;
        }
        return false;
    }
    void initialize() {
        // Check if anisotropic filtering is available
        if (initialized) return;
        initialized = true;
        if (hasAnisotropicFiltering()) anisotropicFilteringAvailable = true;
        simpleShader.initialize();
        texturedShader.initialize();
        texCoords = {{{0.0f, 1.0f},
                      {0.0f, 0.0f},
                      {1.0f, 0.0f},
                      {1.0f, 1.0f}}}; // Quads will be drawn as triangle fans. so bl, tl, tr, br
        texCoordsBuffer.upload(texCoords);
    }

    void render(const mbgl::LocationIndicatorRenderParameters& params) {
        initialize();
        drawRadius(params);
        drawShadow();
        drawPuck();
        drawHat();
    }

    void release() {
        if (!simpleShader.program) return;
        for (const auto& t : textures) t.second->release();
        buffer.release();
        circleBuffer.release();
        puckBuffer.release();
        hatBuffer.release();
        texCoordsBuffer.release();
        simpleShader.release();
        texturedShader.release();
    }

    void updatePuckGeometry(const mbgl::LocationIndicatorRenderParameters& params) {
        if (params.projectionMatrix != oldParams.projectionMatrix) positionChanged = true;
        if (params.puckPosition != oldParams.puckPosition) {
            positionChanged = true;
            ruler = mapbox::cheap_ruler::CheapRuler(params.puckPosition.latitude(),
                                                    mapbox::cheap_ruler::CheapRuler::Meters);
        } else if (params.puckBearing != oldParams.puckBearing ||
                   params.puckLayersDisplacement != oldParams.puckLayersDisplacement ||
                   params.perspectiveCompensation != oldParams.perspectiveCompensation ||
                   params.puckScale != oldParams.puckScale || params.puckHatScale != oldParams.puckHatScale ||
                   params.puckShadowScale != oldParams.puckShadowScale)
            bearingChanged = true; // changes puck geometry but not necessarily the location
        if (params.errorRadiusMeters != oldParams.errorRadiusMeters) radiusChanged = true;
        setTextureFromImageID(params.puckImagePath, texPuck, params);
        setTextureFromImageID(params.puckShadowImagePath, texShadow, params);
        setTextureFromImageID(params.puckHatImagePath, texPuckHat, params);

        projectionCircle = params.projectionMatrix;
        const Point<double> positionMercator = project(params.puckPosition, *params.state);
        mat4 translation;
        matrix::identity(translation);
        matrix::translate(translation, translation, positionMercator.x, positionMercator.y, 0.0);
        matrix::multiply(projectionCircle, projectionCircle, translation);

        if (positionChanged) {
            updateRadius(params);
            updatePuck(params);
            positionChanged = false;
        } else {
            if (radiusChanged) {
                updateRadius(params);
            }
            if (bearingChanged) {
                updatePuck(params);
            }
        }
        oldParams = params;
    }

protected:
    static ScreenCoordinate latLngToScreenCoordinate(const LatLng& p, const TransformState& s) {
        LatLng unwrappedLatLng = p.wrapped();
        unwrappedLatLng.unwrapForShortestPath(s.getLatLng(LatLng::Wrapped));
        ScreenCoordinate point = s.latLngToScreenCoordinate(unwrappedLatLng);
        point.y = s.getSize().height - point.y;
        return point;
    }

    static Point<double> project(const LatLng& c, const TransformState& s) {
        LatLng unwrappedLatLng = c.wrapped();
        unwrappedLatLng.unwrapForShortestPath(s.getLatLng(LatLng::Wrapped));
        return Projection::project(unwrappedLatLng, s.getScale());
    }

    static Point<double> unproject(const LatLng& c, const TransformState& s) {
        LatLng unwrappedLatLng = c.wrapped();
        unwrappedLatLng.unwrapForShortestPath(s.getLatLng(LatLng::Wrapped));
        return Projection::project(unwrappedLatLng, s.getScale());
    }

    void updateRadius(const mbgl::LocationIndicatorRenderParameters& params) {
        const TransformState& s = *params.state;
        const unsigned long numVtxCircumference = circle.size() - 1;
        const float bearingStep = 360.0f / float(numVtxCircumference - 1); // first and last points are the same
        const mapbox::cheap_ruler::point centerPoint(params.puckPosition.longitude(), params.puckPosition.latitude());
        Point<double> center = project(params.puckPosition, s);
        circle[0] = {0, 0};

        double mapBearing = util::wrap(util::RAD2DEG * params.bearing, 0.0, util::DEGREES_MAX);
        for (unsigned long i = 1; i <= numVtxCircumference; ++i) {
            const float bearing_ = float(i - 1) * bearingStep - mapBearing;
            Point<double> poc = ruler.destination(centerPoint, params.errorRadiusMeters, bearing_);
            circle[i] = vec2(project(LatLng(poc.y, poc.x), s) - center);
        }
        radiusChanged = false;
    }

    // Size in "map pixels" for a screen pixel
    static float pixelSizeToWorldSizeH(const LatLng& pos, const TransformState& s) {
        ScreenCoordinate posScreen = latLngToScreenCoordinate(pos, s);
        ScreenCoordinate posScreenLeftPx = posScreen;
        posScreenLeftPx.x -= 1;
        LatLng posLeftPx = screenCoordinateToLatLng(posScreenLeftPx, s);
        Point<double> posMerc = project(pos, s);
        Point<double> posLeftPxMerc = project(posLeftPx, s);
        return vec2(posMerc - posLeftPxMerc).length();
    }

    static vec2 verticalDirectionMercator(const LatLng& pos, const TransformState& s) {
        ScreenCoordinate posScreen = latLngToScreenCoordinate(pos, s);
        Point<double> posMerc = project(pos, s);
        return verticalDirectionMercator(posScreen, posMerc, s);
    }

    static vec2 verticalDirectionMercator(const ScreenCoordinate& pos, Point<double> posMerc, const TransformState& s) {
        ScreenCoordinate screenDy = pos;
        screenDy.y -= 1;
        LatLng posDy = screenCoordinateToLatLng(screenDy, s);
        Point<double> posMercDy = project(posDy, s);
        return verticalDirectionMercator(posMerc, posMercDy);
    }

    static vec2 verticalDirectionMercator(const Point<double>& posMerc, const Point<double>& posMercDy) {
        Point<double> verticalShiftMercator = posMercDy - posMerc;
        vec2 res(verticalShiftMercator);
        return res.normalized();
    }

    static Point<double> hatShadowShiftVector(const LatLng& position,
                                              const mbgl::LocationIndicatorRenderParameters& params) {
        const TransformState& s = *params.state;
        ScreenCoordinate posScreen = latLngToScreenCoordinate(position, s);
        posScreen.y = params.height - 1; // moving it to bottom
        Point<double> posMerc = project(screenCoordinateToLatLng(posScreen, s), s);
        vec2 verticalShiftAtPos = verticalDirectionMercator(posScreen, posMerc, s);
        return {verticalShiftAtPos.x, verticalShiftAtPos.y};
    }

    static vec2 directionAtPositionScreen(const LatLng& position, float bearing, const TransformState& s) {
        const double scale = s.getScale();
        const vec2 rot = vec2(0.0, -1.0).rotated(-bearing);
        Point<double> posMerc = project(position, s);
        Point<double> posMercDelta = posMerc + rot.toPoint();
        ScreenCoordinate posScreen = latLngToScreenCoordinate(position, s);
        ScreenCoordinate posScreenDelta = latLngToScreenCoordinate(Projection::unproject(posMercDelta, scale), s);
        return vec2(posScreenDelta - posScreen).normalized();
    }

    void updatePuck(const mbgl::LocationIndicatorRenderParameters& params) { return updatePuckPerspective(params); }

    void updatePuckPerspective(const mbgl::LocationIndicatorRenderParameters& params) {
        const TransformState& s = *params.state;
        projectionPuck = projectionCircle; // Duplicated as it might change, depending on what puck style is chosen.
        const mapbox::cheap_ruler::point centerPoint(params.puckPosition.longitude(), params.puckPosition.latitude());
        static constexpr float bearings[]{
            225.0f, 315.0f, 45.0f, 135.0f}; // Quads will be drawn as triangle fans. so bl, tl, tr, br
#ifndef M_SQRT2
        static constexpr const float M_SQRT2 = std::sqrt(2.0f);
#endif
        // The puck has to stay square at all zoom levels. CheapRuler::destination does not guarantee this at low zoom
        // levels, so the extent has to be produced in mercator space
        const double tilt = s.getPitch();

        // Point<double> verticalShiftAtCenter { float(std::sin(util::DEG2RAD * util::wrap<float>(-t.getBearing() *
        // util::RAD2DEG, 0.0f, 360.0f) )),
        //                                      -float(std::cos(util::DEG2RAD * util::wrap<float>(-t.getBearing() *
        //                                      util::RAD2DEG, 0.0f, 360.0f))) };
        // would be correct only in the vertical center of the map. As soon as position goes away from that line,
        // the shift direction is skewed by the perspective projection.
        // So the way to have a shift aligned to the screen vertical axis is to find this direction in screen space, and
        // convert it back to map space. This would yield an always straight up shift. However, going further (= the
        // opposite direction of where the lines are converging in the projection) might produce an even more realistic
        // effect. But in this case, it empirically seems that the largest shift that look acceptable is what is
        // obtained at the bottom of the window, avoiding the wider converging lines that pass by the edge of the screen
        // going toward the top.

        Point<double> verticalShift = hatShadowShiftVector(params.puckPosition, params);
        const float horizontalScaleFactor =
            (1.0f - params.perspectiveCompensation) +
            util::clamp(pixelSizeToWorldSizeH(params.puckPosition, s), 0.8f, 100.1f) *
                params.perspectiveCompensation; // Compensation factor for the perspective deformation
        //     ^ clamping this to 0.8 to avoid growing the puck too much close to the camera.
        const double shadowRadius =
            ((texShadow) ? texShadow->width / texShadow->pixelRatio : 0.0) * params.puckShadowScale * M_SQRT2 * 0.5 *
            horizontalScaleFactor; // Technically it's not the radius, but the half diagonal of the quad.
        const double puckRadius = ((texPuck) ? texPuck->width / texPuck->pixelRatio : 0.0) * params.puckScale *
                                  M_SQRT2 * 0.5 * horizontalScaleFactor;
        const double hatRadius = ((texPuckHat) ? texPuckHat->width / texPuckHat->pixelRatio : 0.0) *
                                 params.puckHatScale * M_SQRT2 * 0.5 * horizontalScaleFactor;

        for (unsigned long i = 0; i < 4; ++i) {
            const auto b = util::wrap<float>(params.puckBearing + bearings[i], 0.0f, 360.0f);

            const Point<double> cornerDirection{float(std::sin(util::DEG2RAD * b)),
                                                -float(std::cos(util::DEG2RAD * b))};

            Point<double> shadowOffset = cornerDirection * shadowRadius;
            Point<double> puckOffset = cornerDirection * puckRadius;
            Point<double> hatOffset = cornerDirection * hatRadius;

            shadowGeometry[i] =
                vec2(shadowOffset + (verticalShift * (tilt * -params.puckLayersDisplacement * horizontalScaleFactor)));
            puckGeometry[i] = vec2(puckOffset);
            hatGeometry[i] =
                vec2(hatOffset + (verticalShift * (tilt * params.puckLayersDisplacement * horizontalScaleFactor)));
        }

        bearingChanged = false;
    }

    void drawRadius(const mbgl::LocationIndicatorRenderParameters& params) {
        if (!(params.errorRadiusMeters > 0.0) ||
            (params.errorRadiusColor.a == 0.0 && params.errorRadiusBorderColor.a == 0.0))
            return;

        simpleShader.bind();
        mbgl::gl::bindUniform(simpleShader.u_color, params.errorRadiusColor);
        mbgl::gl::bindUniform(simpleShader.u_matrix, projectionCircle);

        circleBuffer.upload(circle);
        MBGL_CHECK_ERROR(glEnableVertexAttribArray(simpleShader.a_pos));
        MBGL_CHECK_ERROR(glVertexAttribPointer(simpleShader.a_pos, 2, GL_FLOAT, GL_FALSE, 0, nullptr));

        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_FAN, 0, circle.size()));
        if (params.errorRadiusBorderColor.a > 0.0f) {
            mbgl::gl::bindUniform(simpleShader.u_color, params.errorRadiusBorderColor);
            MBGL_CHECK_ERROR(glLineWidth(1.0f));
            MBGL_CHECK_ERROR(glDrawArrays(GL_LINE_STRIP, 1, circle.size() - 1));
        }
        MBGL_CHECK_ERROR(glDisableVertexAttribArray(simpleShader.a_pos));
        circleBuffer.detach();
        simpleShader.detach();
    }

    void drawQuad(Buffer& buf, std::array<vec2, 4>& data, std::shared_ptr<Texture>& texture) {
        if (!texture || !texture->isValid()) return;
        texturedShader.bind();
        texture->bind(0);
        glUniform1i(texturedShader.u_image, 0);
        mbgl::gl::bindUniform(texturedShader.u_matrix, projectionPuck);

        buf.bind();
        buf.upload(data);
        MBGL_CHECK_ERROR(glEnableVertexAttribArray(texturedShader.a_pos));
        MBGL_CHECK_ERROR(glVertexAttribPointer(texturedShader.a_pos, 2, GL_FLOAT, GL_FALSE, 0, nullptr));

        texCoordsBuffer.bind();
        MBGL_CHECK_ERROR(glEnableVertexAttribArray(texturedShader.a_texCoord));
        MBGL_CHECK_ERROR(glVertexAttribPointer(texturedShader.a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr));

        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));
        texture->detach();
        texCoordsBuffer.detach();
        texturedShader.detach();
    }

    void drawShadow() { drawQuad(shadowBuffer, shadowGeometry, texShadow); }

    void drawPuck() { drawQuad(puckBuffer, puckGeometry, texPuck); }

    void drawHat() { drawQuad(hatBuffer, hatGeometry, texPuckHat); }

    static LatLng screenCoordinateToLatLng(const ScreenCoordinate& p,
                                           const TransformState& s,
                                           LatLng::WrapMode wrapMode = LatLng::Wrapped) {
        ScreenCoordinate flippedPoint = p;
        flippedPoint.y = s.getSize().height - flippedPoint.y;
        return s.screenCoordinateToLatLng(flippedPoint, wrapMode);
    }

    void setTextureFromImageID(const std::string& imagePath,
                               std::shared_ptr<Texture>& tex,
                               const mbgl::LocationIndicatorRenderParameters& params) {
        if (textures.find(imagePath) == textures.end()) {
            std::shared_ptr<Texture> tx = std::make_shared<Texture>();
            if (!imagePath.empty() && params.imageManager)
                tx->assign(params.imageManager->getSharedImage(imagePath));
            else
                tx->assign(nullptr);
            textures[imagePath] = tx;
        } else {
            const Immutable<style::Image::Impl>* ima = params.imageManager->getSharedImage(imagePath);
            const mbgl::PremultipliedImage* img = (ima) ? &ima->get()->image : nullptr;
            if (textures.at(imagePath)->image != img) // image for the ID might have changed.
                textures.at(imagePath)->assign(ima);
        }
        tex = textures.at(imagePath);
    }

    std::map<std::string, std::shared_ptr<Texture>> textures;
    mapbox::cheap_ruler::CheapRuler ruler;
    SimpleShader simpleShader;
    TexturedShader texturedShader;
    Buffer buffer;
    Buffer circleBuffer;
    Buffer shadowBuffer;
    Buffer puckBuffer;
    Buffer hatBuffer;
    Buffer texCoordsBuffer;
    std::shared_ptr<Texture> texShadow;
    std::shared_ptr<Texture> texPuck;
    std::shared_ptr<Texture> texPuckHat;

    std::array<vec2, 73> circle; // 72 points + position
    std::array<vec2, 4> shadowGeometry;
    std::array<vec2, 4> puckGeometry;
    std::array<vec2, 4> hatGeometry;
    std::array<vec2, 4> texCoords;
    mbgl::mat4 projectionCircle;
    mbgl::mat4 projectionPuck;

    bool positionChanged = false;
    bool radiusChanged = false;
    bool bearingChanged = false;
    mbgl::LocationIndicatorRenderParameters oldParams;
    bool initialized = false;

public:
    mbgl::LocationIndicatorRenderParameters parameters;
    static bool anisotropicFilteringAvailable;
};

bool RenderLocationIndicatorImpl::anisotropicFilteringAvailable = false;

using namespace style;
namespace {

inline const LocationIndicatorLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == LocationIndicatorLayer::Impl::staticTypeInfo());
    return static_cast<const LocationIndicatorLayer::Impl&>(*impl);
}
} // namespace

RenderLocationIndicatorLayer::RenderLocationIndicatorLayer(Immutable<style::LocationIndicatorLayer::Impl> _impl)
    : RenderLayer(makeMutable<LocationIndicatorLayerProperties>(std::move(_impl))),
      renderImpl(new RenderLocationIndicatorImpl()),
      unevaluated(impl(baseImpl).paint.untransitioned()) {
    assert(gfx::BackendScope::exists());
}

RenderLocationIndicatorLayer::~RenderLocationIndicatorLayer() {
    if (!contextDestroyed) MBGL_CHECK_ERROR(renderImpl->release());
}

void RenderLocationIndicatorLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderLocationIndicatorLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    passes = RenderPass::Translucent;
    auto properties = makeMutable<LocationIndicatorLayerProperties>(
        staticImmutableCast<LocationIndicatorLayer::Impl>(baseImpl), unevaluated.evaluate(parameters));
    const auto& evaluated = properties->evaluated;
    auto& layout = impl(baseImpl).layout;

    properties->renderPasses = mbgl::underlying_type(passes);

    // paint
    renderImpl->parameters.errorRadiusColor = evaluated.get<style::AccuracyRadiusColor>();
    renderImpl->parameters.errorRadiusBorderColor = evaluated.get<style::AccuracyRadiusBorderColor>();
    renderImpl->parameters.errorRadiusMeters = evaluated.get<style::AccuracyRadius>();
    renderImpl->parameters.puckScale = evaluated.get<style::BearingImageSize>();
    renderImpl->parameters.puckHatScale = evaluated.get<style::TopImageSize>();
    renderImpl->parameters.puckShadowScale = evaluated.get<style::ShadowImageSize>();
    renderImpl->parameters.puckBearing = evaluated.get<style::Bearing>().getAngle();
    renderImpl->parameters.puckLayersDisplacement = evaluated.get<style::ImageTiltDisplacement>();
    renderImpl->parameters.perspectiveCompensation = evaluated.get<style::PerspectiveCompensation>();

    const std::array<double, 3> pos = evaluated.get<style::Location>();
    renderImpl->parameters.puckPosition = LatLng{pos[0], pos[1]};

    // layout
    if (!layout.get<style::BearingImage>().isUndefined())
        renderImpl->parameters.puckImagePath = layout.get<style::BearingImage>().asConstant().id();
    if (!layout.get<style::ShadowImage>().isUndefined())
        renderImpl->parameters.puckShadowImagePath = layout.get<style::ShadowImage>().asConstant().id();
    if (!layout.get<style::TopImage>().isUndefined())
        renderImpl->parameters.puckHatImagePath = layout.get<style::TopImage>().asConstant().id();

    evaluatedProperties = std::move(properties);
}

bool RenderLocationIndicatorLayer::hasTransition() const {
    return unevaluated.hasTransition();
}
bool RenderLocationIndicatorLayer::hasCrossfade() const {
    return false;
}

void RenderLocationIndicatorLayer::markContextDestroyed() {
    contextDestroyed = true;
}

void RenderLocationIndicatorLayer::prepare(const LayerPrepareParameters& p) {
    renderImpl->parameters.imageManager = &p.imageManager;
    const TransformState& state = p.state;
    renderImpl->parameters.state = &state;

    renderImpl->parameters.width = state.getSize().width;
    renderImpl->parameters.height = state.getSize().height;
    renderImpl->parameters.latitude = state.getLatLng().latitude();
    renderImpl->parameters.longitude = state.getLatLng().longitude();
    renderImpl->parameters.zoom = state.getZoom();
    renderImpl->parameters.bearing = -state.getBearing() * util::RAD2DEG;
    renderImpl->parameters.pitch = state.getPitch();
    mat4 projMatrix;
    state.getProjMatrix(projMatrix);
    renderImpl->parameters.projectionMatrix = projMatrix;

    renderImpl->updatePuckGeometry(renderImpl->parameters);
}

void RenderLocationIndicatorLayer::render(PaintParameters& paintParameters) {
    auto& glContext = static_cast<gl::Context&>(paintParameters.context);

    // Reset GL state to a known state so the CustomLayer always has a clean slate.
    glContext.bindVertexArray = 0;
    glContext.setDepthMode(paintParameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly));
    glContext.setStencilMode(gfx::StencilMode::disabled());
    glContext.setColorMode(paintParameters.colorModeForRenderPass()); // this is gfx::ColorMode::alphaBlended()
    glContext.setCullFaceMode(gfx::CullFaceMode::disabled());

    MBGL_CHECK_ERROR(renderImpl->render(renderImpl->parameters));

    // Reset the view back to our original one, just in case the CustomLayer changed
    // the viewport or Framebuffer.
    paintParameters.backend.getDefaultRenderable().getResource<gl::RenderableResource>().bind();
    glContext.setDirtyState();
}

} // namespace mbgl
