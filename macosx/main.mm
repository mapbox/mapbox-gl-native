#include <llmr/llmr.hpp>
#include <GLFW/glfw3.h>
#import <Foundation/Foundation.h>
#include <llmr/platform/platform.hpp>
#include <llmr/map/tile.hpp>

class MapView {
public:
    MapView() :
        dirty(true),
        tracking(false),
        platform(new llmr::platform(this)),
        map(new llmr::map(platform)) {
        if (!glfwInit()) {
            fprintf(stderr, "Failed to initialize glfw\n");
            exit(1);
        }

        window = glfwCreateWindow(640, 480, "llmr", NULL, NULL);
        if (!window) {
            glfwTerminate();
            fprintf(stderr, "Failed to initialize window\n");
            exit(1);
        }

        glfwMakeContextCurrent(window);

        map->setup();


        int width, height;
        glfwGetWindowSize(window, &width, &height);
        map->resize(width, height);

        glfwSwapInterval(1);


        glfwSetCursorPosCallback(window, mousemove);
        glfwSetMouseButtonCallback(window, mouseclick);
        glfwSetWindowSizeCallback(window, resize);
        glfwSetScrollCallback(window, scroll);

        glfwSetWindowUserPointer(window, this);
    }

    static void scroll(GLFWwindow *window, double xoffset, double yoffset) {
        MapView *view = (MapView *)glfwGetWindowUserPointer(window);
        double delta = yoffset * 40;

        bool is_wheel = delta != 0 && fmod(delta, 4.000244140625) == 0;

        double absdelta = delta < 0 ? -delta : delta;
        double scale = 2.0 / (1.0 + exp(-absdelta / 100.0));

        // Make the scroll wheel a bit slower.
        if (!is_wheel) {
            scale = (scale - 1.0) / 2.0 + 1.0;
        }

        // Zooming out.
        if (delta < 0 && scale != 0) {
            scale = 1.0 / scale;
        }

        view->map->scaleBy(scale, view->last_x, view->last_y);
    }

    static void resize(GLFWwindow *window, int width, int height) {
        MapView *view = (MapView *)glfwGetWindowUserPointer(window);
        view->map->resize(width, height);
    }

    static void mouseclick(GLFWwindow *window, int button, int action, int modifiers) {
        MapView *view = (MapView *)glfwGetWindowUserPointer(window);
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            view->tracking = action == GLFW_PRESS;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            view->rotating = action == GLFW_PRESS;
            if (view->rotating) {
                view->start_x = view->last_x;
                view->start_y = view->last_y;
            }
        }
    }

    static void mousemove(GLFWwindow *window, double x, double y) {
        MapView *view = (MapView *)glfwGetWindowUserPointer(window);
        if (view->tracking) {
            view->map->moveBy(x - view->last_x, y - view->last_y);
        } else if (view->rotating) {
            view->map->rotateBy(view->start_x, view->start_y, x - view->last_x, y - view->last_y);
        }
        view->last_x = x;
        view->last_y = y;
    }

    int run() {
        while (!glfwWindowShouldClose(window)) {
            if (dirty) {
                dirty = render();
                glfwSwapBuffers(window);
                fps();
            }

            if (dirty) {
                glfwPollEvents();
            } else {
                glfwWaitEvents();
            }
        }

        return 0;
    }

    bool render() {
        return map->render();
    }

    void fps() {
        static int frames = 0;
        static double time_elapsed = 0;

        frames++;
        double current_time = glfwGetTime();

        if (current_time - time_elapsed >= 1) {
            fprintf(stderr, "FPS: %4.2f\n", frames / (current_time - time_elapsed));
            time_elapsed = current_time;
            frames = 0;
        }
    }

    ~MapView() {
        delete map;
        delete platform;
        glfwTerminate();
    }

public:
    bool dirty;
    double last_x, last_y;
    bool tracking;

    double start_x, start_y;
    bool rotating;

    GLFWwindow *window;
    llmr::platform *platform;
    llmr::map *map;
};

void llmr::platform::restart() {
    ((MapView *)obj)->dirty = true;
}

void llmr::platform::request(tile *tile) {
    fprintf(stderr, "request %d/%d/%d\n", tile->z, tile->x, tile->y);

    fprintf(stderr, "requesting tile\n");
    NSString *urlTemplate = @"http://api.tiles.mapbox.com/v3/mapbox.mapbox-streets-v4/%d/%d/%d.vector.pbf";
    NSString *urlString = [NSString
                           stringWithFormat:urlTemplate,
                           tile->z,
                           tile->x,
                           tile->y];
    NSURL *url = [NSURL URLWithString:urlString];
    NSLog(@"Requesting %@", urlString);

    NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:url];

    NSOperationQueue *queue = [[NSOperationQueue alloc] init];
    [NSURLConnection
     sendAsynchronousRequest:urlRequest
     queue:queue
     completionHandler: ^ (NSURLResponse * response,
                           NSData * data,
    NSError * error) {
        if (error == nil) {
            tile->setData((uint8_t *)[data bytes], [data length]);
            if (tile->parse()) {
                dispatch_async(dispatch_get_main_queue(), ^ {
                    ((MapView *)obj)->map->tileLoaded(tile);
                });
                return;
            }
        }

        dispatch_async(dispatch_get_main_queue(), ^ {
            ((MapView *)obj)->map->tileFailed(tile);
        });
    }];
}


int main() {
    MapView view;
    return view.run();
}
