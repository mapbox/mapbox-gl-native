#include "../common/settings_nsuserdefaults.hpp"
#include "../common/glfw_view.hpp"

int main() {
    GLFWView view;
    llmr::Map map(view);

    // Load settings
    llmr::Settings_NSUserDefaults settings;
    map.setLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    map.setAngle(settings.angle);
    map.setDebug(settings.debug);

    // fprintf(stderr, "lon: %f, lat: %f, zoom: %f, angle: %f, debug: %d\n", settings.l)

    int ret = view.run();

    // Save settings
    map.getLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    settings.angle = map.getAngle();
    settings.debug = map.getDebug();
    settings.save();

    return ret;
}
