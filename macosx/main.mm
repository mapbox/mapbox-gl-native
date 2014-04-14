#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "settings.hpp"
#include "../common/map_view.cpp"

#include <cstdio>

#include <thread>

MapView *mapView = nullptr;

int main() {
    llmr::Settings_MacOSX settings;
    mapView = new MapView(settings);
    mapView->init();
    int ret = mapView->run();
    mapView->settings.sync();
    delete mapView;
    return ret;
}



namespace llmr {

void platform::cleanup() {
    // noop
}

void platform::restart() {
    if (mapView) {
        mapView->dirty = true;
        CGEventRef event = CGEventCreate(NULL);
        CGEventSetType(event, kCGEventMouseMoved);
        [[NSApplication sharedApplication] postEvent: [NSEvent eventWithCGEvent:event] atStart:NO];
    }
}

}
