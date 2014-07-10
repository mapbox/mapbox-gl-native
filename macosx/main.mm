#include "../common/settings_nsuserdefaults.hpp"
#include "../common/glfw_view.hpp"

#import <Foundation/Foundation.h>

int main() {
    GLFWView view;
    llmr::Map map(view);

    // Load settings
    llmr::Settings_NSUserDefaults settings;
    map.setLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    map.setAngle(settings.angle);
    map.setDebug(settings.debug);

    // Set access token if present
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token == nullptr) {
        fprintf(stderr, "[WARNING] no access token set. mapbox.com tiles won't work.\n");
    } else {
        map.setAccessToken(std::string(token));
    }

    // Load style
    NSString *path = [[NSBundle mainBundle] pathForResource:@"style.min" ofType:@"js"];
    NSString *json = [NSString stringWithContentsOfFile:path
                                               encoding:[NSString defaultCStringEncoding]
                                                  error:nil];
    map.setStyleJSON((std::string)[json cStringUsingEncoding:[NSString defaultCStringEncoding]]);

    // fprintf(stderr, "lon: %f, lat: %f, zoom: %f, angle: %f, debug: %d\n", settings.l)

    int ret = view.run();

    // Save settings
    map.getLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    settings.angle = map.getAngle();
    settings.debug = map.getDebug();
    settings.save();

    return ret;
}
