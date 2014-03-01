#include <llmr/llmr.hpp>
#include <GLFW/glfw3.h>
#include <curl/curl.h>
#include <llmr/platform/platform.hpp>
#include "settings.hpp"
#include <future>
#include <list>
#include <forward_list>
#include <chrono>
#include <atomic>

#include <pthread.h>

namespace llmr {
namespace platform {

struct Request {
    pthread_t thread;
    std::string url;
    std::atomic<bool> done;
    std::atomic<bool> cancel;
    std::function<void(Response&)> background_function;
    std::function<void()> foreground_callback;
};

}
}

pthread_mutex_t curl_share_mutex = PTHREAD_MUTEX_INITIALIZER;
CURLSH *curl_share;
std::forward_list<llmr::platform::Request *> requests;


void curl_share_lock(CURL *, curl_lock_data, curl_lock_access, void *) {
    pthread_mutex_lock(&curl_share_mutex);
}

void curl_share_unlock(CURL *, curl_lock_data, void *) {
    pthread_mutex_unlock(&curl_share_mutex);
}

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
            bool& dirty = this->dirty;
            requests.remove_if([&dirty](llmr::platform::Request * req) {
                if (req->done) {
                    req->foreground_callback();
                    pthread_join(req->thread, nullptr);
                    delete req;
                    dirty = true;
                    return true;
                } else {
                    return false;
                }
            });

            if (dirty) {
                try {
                    dirty = render();
                } catch (std::exception& ex) {
                    fprintf(stderr, "exception: %s\n", ex.what());
                }
                glfwSwapBuffers(window);
                fps();
            }

            if (!requests.empty() || dirty) {
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

MapView *mapView = nullptr;

namespace llmr {
namespace platform {

void restart() {
    if (mapView) {
        mapView->dirty = true;
    }
}

static size_t curl_write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}


static int curl_progress_callback(void *ptr, double dltotal, double dlnow, double ultotal, double ulnow) {
    Request *req = static_cast<Request *>(ptr);
    return req->cancel;
}

void *request_http(void *ptr) {
    Request *req = static_cast<Request *>(ptr);
    Response res;

    // TODO: use curl multi to be able to cancel

    CURL *curl;
    CURLcode code;
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, req->url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res.body);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, curl_progress_callback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, req);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
        curl_easy_setopt(curl, CURLOPT_SHARE, curl_share);
        code = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.code);
        curl_easy_cleanup(curl);

        if (code != CURLE_ABORTED_BY_CALLBACK) {
            req->background_function(res);
        }
    }

    req->done = true;

    pthread_exit(nullptr);
}

Request *request_http(std::string url, std::function<void(Response&)> background_function, std::function<void()> foreground_callback) {
    Request *req = new Request();
    req->url = url;
    req->background_function = background_function;
    req->foreground_callback = foreground_callback;
    requests.push_front(req);
    int rc = pthread_create(&req->thread, nullptr, request_http, (void *)req);
    if (rc) {
        fprintf(stderr, "http request failed\n");
        return nullptr;
    }

    return req;
}

void cancel_request_http(Request *request) {
    auto it = std::find(requests.begin(), requests.end(), request);
    if (it != requests.end()) {
        Request *req = *it;
        req->cancel = true;
    }
}

double time() {
    return glfwGetTime();
}

}
}


int main() {
    curl_global_init(CURL_GLOBAL_ALL);
    curl_share = curl_share_init();
    curl_share_setopt(curl_share, CURLSHOPT_LOCKFUNC, curl_share_lock);
    curl_share_setopt(curl_share, CURLSHOPT_UNLOCKFUNC, curl_share_unlock);

    mapView = new MapView();
    mapView->init();
    int ret = mapView->run();
    mapView->settings.sync();
    delete mapView;

    curl_share_cleanup(curl_share);
    pthread_exit(NULL);
    return ret;
}
