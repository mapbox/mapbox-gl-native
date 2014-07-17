#include "../common/settings_nsuserdefaults.hpp"
#include "../common/glfw_view.hpp"
#include "../common/nslog_log.hpp"

#import <Foundation/Foundation.h>

int main() {
    mbgl::Log::Set<mbgl::NSLogBackend>();

    GLFWView view;
    mbgl::Map map(view);

    // Load settings
    mbgl::Settings_NSUserDefaults settings;
    map.setLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    map.setAngle(settings.angle);
    map.setDebug(settings.debug);

    // Set access token if present
    NSString *accessToken = [[NSProcessInfo processInfo] environment][@"MAPBOX_ACCESS_TOKEN"];
    if ( ! accessToken) mbgl::Log::Warning(mbgl::Event::Setup, "No access token set. Mapbox vector tiles won't work.");
    if (accessToken) map.setAccessToken([accessToken cStringUsingEncoding:[NSString defaultCStringEncoding]]);

    // Load style
    NSString *path = [[NSBundle mainBundle] pathForResource:@"style.min" ofType:@"js"];
    NSString *json = [NSString stringWithContentsOfFile:path
                                               encoding:[NSString defaultCStringEncoding]
                                                  error:nil];
    map.setStyleJSON((std::string)[json cStringUsingEncoding:[NSString defaultCStringEncoding]]);

    int ret = view.run();

    // Save settings
    map.getLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    settings.angle = map.getAngle();
    settings.debug = map.getDebug();
    settings.save();

    return ret;
}
