#include <llmr/llmr.hpp>
#include <GLFW/glfw3.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include <llmr/platform/platform.hpp>
#include "settings.hpp"

#include <thread>

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
    }

    static void character(GLFWwindow *window, unsigned int codepoint) {

    }


    static void key(GLFWwindow *window, int key, int scancode, int action, int mods) {
        MapView *view = (MapView *)glfwGetWindowUserPointer(window);

        if (action == GLFW_RELEASE) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                case GLFW_KEY_TAB:
                    view->map.toggleDebug();
                    break;
                case GLFW_KEY_R:
                    if (!mods) view->map.resetPosition();
                    break;
                case GLFW_KEY_N:
                    if (!mods) view->map.resetNorth();
                    break;
            }
        }
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

        view->map.scaleBy(scale, view->last_x, view->last_y);
    }

    static void resize(GLFWwindow *window, int, int) {
        MapView *view = (MapView *)glfwGetWindowUserPointer(window);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);

        view->map.resize(width, height, fb_width, fb_height);
    }

    static void mouseclick(GLFWwindow *window, int button, int action, int modifiers) {
        MapView *view = (MapView *)glfwGetWindowUserPointer(window);

        if (button == GLFW_MOUSE_BUTTON_RIGHT || (button == GLFW_MOUSE_BUTTON_LEFT && modifiers & GLFW_MOD_CONTROL)) {
            view->rotating = action == GLFW_PRESS;
            if (view->rotating) {
                view->start_x = view->last_x;
                view->start_y = view->last_y;
            }
        } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
            view->tracking = action == GLFW_PRESS;

            if (action == GLFW_RELEASE) {
                double now = glfwGetTime();
                if (now - view->last_click < 0.4) {
                    view->map.scaleBy(2.0, view->last_x, view->last_y);
                }
                view->last_click = now;
            }
        }
    }

    static void mousemove(GLFWwindow *window, double x, double y) {
        MapView *view = (MapView *)glfwGetWindowUserPointer(window);
        if (view->tracking) {
            view->map.moveBy(x - view->last_x, y - view->last_y);
        } else if (view->rotating) {
            view->map.rotateBy(view->start_x, view->start_y, view->last_x, view->last_y, x, y);
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
};

MapView *view;

namespace llmr {
namespace platform {

void restart(void *) {
    view->dirty = true;
}

void async(std::function<void()> fn, std::function<void()> cb) {
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void){
        fn();
        dispatch_async(dispatch_get_main_queue(), ^(void){
            cb();
        });
    });
}

void request_http(std::string url, std::function<void(Response&)> func) {
    NSMutableURLRequest *urlRequest = [NSMutableURLRequest
                                       requestWithURL:[NSURL
                                               URLWithString:[NSString
                                                       stringWithUTF8String:url.c_str()]]];

    [NSURLConnection
     sendAsynchronousRequest:urlRequest
     queue:[NSOperationQueue mainQueue]
     completionHandler: ^ (NSURLResponse* response, NSData* data, NSError* error) {
        if (error == nil) {
            NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
            Response res;
            res.code = [httpResponse statusCode];
            res.body = { (const char *)[data bytes], [data length] };
            func(res);
        } else {
            Response res;
            func(res);
        }
    }];
}

void request_http(std::string url, std::function<void(Response&)> func, std::function<void()> cb) {
        NSMutableURLRequest *urlRequest = [NSMutableURLRequest
                                       requestWithURL:[NSURL
                                               URLWithString:[NSString
                                                       stringWithUTF8String:url.c_str()]]];

    [NSURLConnection
     sendAsynchronousRequest:urlRequest
     queue:[NSOperationQueue mainQueue]
     completionHandler: ^ (NSURLResponse* response, NSData* data, NSError* error) {
        Response res;
        if (error == nil) {
            NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
            res.code = [httpResponse statusCode];
            res.body = { (const char *)[data bytes], [data length] };
        }

        func(res);
        cb();
    }];
}


double time() {
    return glfwGetTime();
}

}
}

int main() {
    view = new MapView();
    view->init();
    int ret = view->run();
    delete view;
    return ret;
}
