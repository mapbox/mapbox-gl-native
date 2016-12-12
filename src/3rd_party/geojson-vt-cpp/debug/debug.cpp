#include <GLFW/glfw3.h>
#include <memory>
#include "../bench/util.hpp"
#include <mapbox/geojson.hpp>
#include <mapbox/geojsonvt.hpp>

using namespace mapbox::geojsonvt;

template <typename T>
void drawLine(const T points) {
    glColor4f(1, 0, 0, 1);
    glBegin(GL_LINE_STRIP);
    for (const auto& pt : points) {
        glVertex2s(pt.x, pt.y);
    }
    glEnd();
}

struct DrawFeature {
    void operator()(const mapbox::geometry::point<int16_t>&) {
    }

    void operator()(const mapbox::geometry::line_string<int16_t>& points) {
        drawLine(points);
    }

    void operator()(const mapbox::geometry::linear_ring<int16_t>& points) {
        drawLine(points);
    }

    void operator()(const mapbox::geometry::geometry<int16_t>& geometry) {
        mapbox::geometry::geometry<int16_t>::visit(geometry, DrawFeature{});
    }

    template <class T>
    void operator()(const T& vector) {
        for (const auto& e : vector) {
            operator()(e);
        }
    }
};

int main() {
    if (glfwInit() == 0) {
        return -1;
    }

    static int width = 768;
    static int fbWidth = width;
    static int height = 768;
    static int fbHeight = height;

    GLFWwindow* window =
        glfwCreateWindow(width, height, "GeoJSON VT — Drop a GeoJSON file", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return -1;
    }

    static std::unique_ptr<GeoJSONVT> vt;

    static bool dirty = true;
    static struct TileID {
        int z;
        int x;
        int y;
    } pos = { 0, 0, 0 };

    enum class Horizontal { Left, Right, Outside };
    enum class Vertical { Top, Bottom, Outside };

    static Horizontal horizontal = Horizontal::Outside;
    static Vertical vertical = Vertical::Outside;

    Tile* tile = nullptr;

    static const auto updateTile = [&] {
        const std::string name =
            std::to_string(pos.z) + "/" + std::to_string(pos.x) + "/" + std::to_string(pos.y);
        if (vt) {
            Timer tileTimer;
            tile = const_cast<Tile*>(&vt->getTile(pos.z, pos.x, pos.y));
            tileTimer("tile " + name);
            glfwSetWindowTitle(window, (std::string{ "GeoJSON VT — " } + name).c_str());
        }
        dirty = true;
    };

    static const auto loadGeoJSON = [&](const std::string& filename) {
        Timer timer;
        const std::string data = loadFile(filename);
        timer("loadFile");

        const auto features =
            mapbox::geojson::parse(data).get<mapbox::geojson::feature_collection>();
        timer("parse into geometry");

        vt = std::make_unique<GeoJSONVT>(features);
        timer("generate tile index");
        updateTile();
    };

    static const auto updateLocation = [](const Horizontal newHorizontal,
                                          const Vertical newVertical) {
        if (newHorizontal != horizontal || newVertical != vertical) {
            dirty = true;
            horizontal = newHorizontal;
            vertical = newVertical;
        }
    };

    glfwSetDropCallback(window, [](GLFWwindow*, int count, const char* name[]) {
        if (count >= 1) {
            loadGeoJSON(name[0]);
        }
    });

    glfwSetKeyCallback(
        window, [](GLFWwindow* w, const int key, const int, const int action, const int) {
            if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
                glfwSetWindowShouldClose(w, 1);
            }
            if ((key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_ESCAPE) && action == GLFW_RELEASE) {
                // zoom out
                if (pos.z > 0) {
                    pos.z--;
                    pos.x /= 2;
                    pos.y /= 2;
                    updateTile();
                }
            }
        });

    glfwSetWindowSizeCallback(window, [](GLFWwindow*, const int w, const int h) {
        width = w;
        height = h;
        dirty = true;
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, const int w, const int h) {
        fbWidth = w;
        fbHeight = h;
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow*, const double x, const double y) {
        updateLocation((x > 0 && x < width) ? (x * 2 < width ? Horizontal::Left : Horizontal::Right)
                                            : Horizontal::Outside,
                       (y > 0 && y < height) ? (y * 2 < height ? Vertical::Top : Vertical::Bottom)
                                             : Vertical::Outside);
    });

    glfwSetCursorEnterCallback(window, [](GLFWwindow*, int entered) {
        if (entered == 0) {
            updateLocation(Horizontal::Outside, Vertical::Outside);
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow*, int button, int action, int) {
        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
            // zoom into a particular tile
            if (pos.z < 18 && horizontal != Horizontal::Outside && vertical != Vertical::Outside) {
                pos.z++;
                pos.x *= 2;
                pos.y *= 2;
                if (horizontal == Horizontal::Right) {
                    pos.x++;
                }
                if (vertical == Vertical::Bottom) {
                    pos.y++;
                }
                updateTile();
            }
        } else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
            // zoom out
            if (pos.z > 0) {
                pos.z--;
                pos.x /= 2;
                pos.y /= 2;
                updateTile();
            }
        }
    });

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    glfwMakeContextCurrent(window);

    const auto drawTile = [](bool active) {
        if (active) {
            glLineWidth(2);
            glColor4f(0, 0, 1, 1);
        } else {
            glLineWidth(1);
            glColor4f(0, 1, 0, 1);
        }

        glBegin(GL_LINE_STRIP);
        glVertex3f(0, 0, -static_cast<int>(active));
        glVertex3f(0, 4096, -static_cast<int>(active));
        glVertex3f(4096, 4096, -static_cast<int>(active));
        glVertex3f(4096, 0, -static_cast<int>(active));
        glVertex3f(0, 0, -static_cast<int>(active));
        glEnd();
    };

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    loadGeoJSON("data/countries.geojson");

    while (glfwWindowShouldClose(window) == 0) {
        if (dirty) {
            dirty = false;

            if (vt) {
                glClearColor(1, 1, 1, 1);
            } else {
                glClearColor(0.8, 0.8, 0.8, 1);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, fbWidth, fbHeight);

            if (tile != nullptr) {
                // main tile
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(-128, 4096 + 128, 4096 + 128, -128, 10, -10);

                glLineWidth(1);

                for (const auto& feature : tile->features) {
                    mapbox::geometry::geometry<int16_t>::visit(feature.geometry, DrawFeature{});
                }

                // top left
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(-256, 8192 + 256, 8192 + 256, -256, 10, -10);
                drawTile(horizontal == Horizontal::Left && vertical == Vertical::Top);

                // top right
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(-4096 - 256, 8192 + 256 - 4096, 8192 + 256, -256, 10, -10);
                drawTile(horizontal == Horizontal::Right && vertical == Vertical::Top);

                // bottom left
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(-256, 8192 + 256, 8192 + 256 - 4096, -256 - 4096, 10, -10);
                drawTile(horizontal == Horizontal::Left && vertical == Vertical::Bottom);

                // bottom right
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(-4096 - 256, 8192 + 256 - 4096, 8192 + 256 - 4096, -256 - 4096, 10, -10);
                drawTile(horizontal == Horizontal::Right && vertical == Vertical::Bottom);
            } else {
            }

            glfwSwapBuffers(window);
        }

        glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}
