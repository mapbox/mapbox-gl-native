#include <emscripten/emscripten.h>
#include <GL/glfw.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
 #include <unistd.h>

#include <llmr/llmr.hpp>
#include <llmr/map/tile.hpp>

bool dirty = true;

class MapView;

static MapView *view;

class MapView {
public:
    MapView() :
        dirty(true),
        platform(new llmr::platform(this)),
        map(new llmr::map(platform)) {

        // Initialize GLFW
        if (!glfwInit()) {
            fprintf(stderr, "Failed to initialize GLFW\n");
            exit(1);
        }

        int width, height;
        emscripten_get_canvas_size(&width, &height, nullptr);

        glfwSetMousePosCallback(mousemove);
        glfwSetMouseButtonCallback(mouseclick);
        glfwSetMouseWheelCallback(scroll);

        // Open a window and create its OpenGL context
        if (!glfwOpenWindow(width, height, 8, 8, 8, 8, 16, 8, GLFW_WINDOW)) {
            fprintf(stderr, "Failed to open GLFW window\n");

            glfwTerminate();
            exit(1);
        }

        map->setup();
        map->resize(width, height);

    }

    ~MapView() {
        delete map;
        delete platform;
        glfwTerminate();
    }

    static void mousemove(int x, int y) {
        if (view->tracking) {
            view->map->moveBy(x - view->last_x, y - view->last_y);
        }
        view->last_x = x;
        view->last_y = y;
    }

    static void scroll(float pos) {
        double delta = pos;

        // bool is_wheel = delta != 0 && fmod(delta, 4.000244140625) == 0;

        double absdelta = delta < 0 ? -delta : delta;
        double scale = 2.0 / (1.0 + exp(-absdelta / 100.0));

        // Make the scroll wheel a bit slower.
        // if (!is_wheel) {
        //     scale = (scale - 1.0) / 2.0 + 1.0;
        // }

        // Zooming out.
        if (delta < 0 && scale != 0) {
            scale = 1.0 / scale;
        }

        view->map->scaleBy(scale, view->last_x, view->last_y);
    }

    static void mouseclick(int button, int action) {
        if (button == GLFW_MOUSE_BUTTON_1) {
            view->tracking = action == GLFW_PRESS;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            fprintf(stderr, "right mouse\n");
        }
    }

    int run() {
        emscripten_set_main_loop(render, 60, 1);
        return 0;
    }

    static void render() {
        if (view->dirty) {
            view->map->render();
            // glClearColor(1, 1, 0, 1);
            // glClear(GL_COLOR_BUFFER_BIT);

            view->dirty = false;
        }
    }

    bool dirty;
    double last_x, last_y;
    bool tracking;


    llmr::platform *platform;
    llmr::map *map;

};


void llmr::platform::restart() {
    view->dirty = true;
}

void ontileload(void* custom, void* bytes, int length) {
    fprintf(stderr, "data loaded successfully: length: %d\n", length);
    llmr::tile *tile = (llmr::tile *)custom;
    tile->setData((uint8_t *)bytes, length);
    if (tile->parse()) {
        view->map->tileLoaded(tile);
        return;
    }
}

void ontileerror(void* custom) {
    fprintf(stderr, "data load error\n");
    llmr::tile *tile = (llmr::tile *)custom;
    view->map->tileFailed(tile);
}

void llmr::platform::request(tile *tile) {
    const char *urlTemplate = "http://api.tiles.mapbox.com/v3/mapbox.mapbox-streets-v4/%d/%d/%d.vector.pbf";
    char urlString[255];
    snprintf(urlString, 255, urlTemplate, tile->z, tile->x, tile->y);
    fprintf(stderr, "requesting %s\n", urlString);

    emscripten_async_wget_data(urlString, tile, ontileload, ontileerror);
}


int main() {
    view = new MapView();
    int ret = view->run();
    delete view;
    return ret;
}
