#include <llmr/llmr.hpp>
#include <GLFW/glfw3.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include <llmr/platform/platform.hpp>
#include <llmr/map/tile.hpp>
#include "settings.hpp"

#include <thread>

NSString *const MBXNeedsRenderNotification = @"MBXNeedsRenderNotification";

class MapView {
public:
    MapView() :
        dirty(true),
        tracking(false),
        rotating(false),
        last_click(-1),
        settings(),
        map(settings) {
    }

    void init() {
        if (!glfwInit()) {
            fprintf(stderr, "Failed to initialize glfw\n");
            exit(1);
        }

        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_DEPTH_BITS, 16);

        window = glfwCreateWindow(1024, 768, "llmr", NULL, NULL);
        if (!window) {
            glfwTerminate();
            fprintf(stderr, "Failed to initialize window\n");
            exit(1);
        }

        glfwSetWindowUserPointer(window, this);
        glfwMakeContextCurrent(window);

        settings.load();
        map.setup();

        resize(window, 0, 0);

        glfwSwapInterval(1);

        map.loadSettings();

        glfwSetCursorPosCallback(window, mousemove);
        glfwSetMouseButtonCallback(window, mouseclick);
        glfwSetWindowSizeCallback(window, resize);
        glfwSetFramebufferSizeCallback(window, resize);
        glfwSetScrollCallback(window, scroll);
        glfwSetCharCallback(window, character);
        glfwSetKeyCallback(window, key);

        renderObserver = [[NSNotificationCenter defaultCenter] addObserverForName:MBXNeedsRenderNotification
         object:nil
         queue:[NSOperationQueue mainQueue]
        usingBlock: ^ (NSNotification * notification) {
            dirty = true;
        }];
    }

    static void character(GLFWwindow *window, unsigned int codepoint) {

    }


    static void key(GLFWwindow *window, int key, int scancode, int action, int mods) {
        MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);

        if (action == GLFW_RELEASE) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                case GLFW_KEY_TAB:
                    mapView->map.toggleDebug();
                    break;
                case GLFW_KEY_R:
                    if (!mods) mapView->map.resetPosition();
                    break;
                case GLFW_KEY_N:
                    if (!mods) mapView->map.resetNorth();
                    break;
            }
        }
    }


    static void scroll(GLFWwindow *window, double xoffset, double yoffset) {
        MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);
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

        mapView->map.scaleBy(scale, mapView->last_x, mapView->last_y);
    }

    static void resize(GLFWwindow *window, int, int) {
        MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);

        mapView->map.resize(width, height, fb_width, fb_height);
    }

    static void mouseclick(GLFWwindow *window, int button, int action, int modifiers) {
        MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);

        if (button == GLFW_MOUSE_BUTTON_RIGHT || (button == GLFW_MOUSE_BUTTON_LEFT && modifiers & GLFW_MOD_CONTROL)) {
            mapView->rotating = action == GLFW_PRESS;
            if (mapView->rotating) {
                mapView->start_x = mapView->last_x;
                mapView->start_y = mapView->last_y;
            }
        } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
            mapView->tracking = action == GLFW_PRESS;

            if (action == GLFW_RELEASE) {
                double now = glfwGetTime();
                if (now - mapView->last_click < 0.4) {
                    mapView->map.scaleBy(2.0, mapView->last_x, mapView->last_y);
                }
                mapView->last_click = now;
            }
        }
    }

    static void mousemove(GLFWwindow *window, double x, double y) {
        MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);
        if (mapView->tracking) {
            mapView->map.moveBy(x - mapView->last_x, y - mapView->last_y);
        } else if (mapView->rotating) {
            mapView->map.rotateBy(mapView->start_x, mapView->start_y, mapView->last_x, mapView->last_y, x, y);
        }
        mapView->last_x = x;
        mapView->last_y = y;
    }

    int run() {
        while (!glfwWindowShouldClose(window)) {
            if (dirty) {
                try {
                    dirty = render();
                } catch (std::exception& ex) {
                    fprintf(stderr, "exception: %s\n", ex.what());
                }
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
        return map.render();
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
        [[NSNotificationCenter defaultCenter] removeObserver:renderObserver];
        glfwTerminate();
    }

public:
    bool dirty;
    double last_x, last_y;
    bool tracking;

    double start_x, start_y;
    bool rotating;

    double last_click;

    GLFWwindow *window;
    llmr::Settings_MacOSX settings;
    llmr::Map map;

    id renderObserver;
};

MapView *mapView;
NSOperationQueue *queue;

namespace llmr {
namespace platform {

void restart(void *) {
    mapView->dirty = true;
}

void request_http(std::string url, std::function<void(Response&)> background_function, std::function<void()> foreground_callback)
{
    NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithURL:[NSURL URLWithString:@(url.c_str())] completionHandler:^(NSData *data, NSURLResponse *response, NSError *error)
    {
        Response res;

        if ( ! error)
        {
            res.code = [(NSHTTPURLResponse *)response statusCode];
            res.body = { (const char *)[data bytes], [data length] };
        }

        background_function(res);

        dispatch_async(dispatch_get_main_queue(), ^(void)
        {
            foreground_callback();
        });
    }];

    [task resume];
}

void request_http_tile(std::string url, tile_ptr tile_object, std::function<void(Response&)> background_function, std::function<void()> foreground_callback)
{
    NSString *latestZoom = [@(tile_object->id.z) stringValue];

    [[NSURLSession sharedSession] getTasksWithCompletionHandler:^(NSArray *dataTasks, NSArray *uploadTasks, NSArray *downloadTasks)
    {
        for (NSURLSessionDownloadTask *task in downloadTasks)
            if (task.taskDescription && ! [task.taskDescription isEqualToString:latestZoom])
                [task cancel];
    }];

    NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithURL:[NSURL URLWithString:@(url.c_str())] completionHandler:^(NSData *data, NSURLResponse *response, NSError *error)
    {
        if (error && [[error domain] isEqualToString:NSURLErrorDomain] && [error code] == NSURLErrorCancelled)
            return;

        Response res;

        if ( ! error)
        {
            res.code = [(NSHTTPURLResponse *)response statusCode];
            res.body = { (const char *)[data bytes], [data length] };
        }

        background_function(res);

        dispatch_async(dispatch_get_main_queue(), ^(void)
        {
            foreground_callback();
        });

        if ( ! error)
            [[NSNotificationCenter defaultCenter] postNotificationName:MBXNeedsRenderNotification object:nil];
    }];

    task.taskDescription = [@(tile_object->id.z) stringValue];

    [task resume];
}

double time() {
    return glfwGetTime();
}

}
}

int main() {
    mapView = new MapView();
    mapView->init();
    int ret = mapView->run();
    mapView->settings.sync();
    delete mapView;
    return ret;
}
