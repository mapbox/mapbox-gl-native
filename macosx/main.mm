#include "../common/settings_nsuserdefaults.hpp"
#include "../common/glfw_view.hpp"
#include "../common/nslog_log.hpp"

#import <Foundation/Foundation.h>

@interface URLHandler : NSObject
@property (nonatomic) mbgl::Map *map;

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event
           withReplyEvent:(NSAppleEventDescriptor *)replyEvent;
@end

@implementation URLHandler

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event
           withReplyEvent:(NSAppleEventDescriptor *)replyEvent {
    (void)replyEvent;

    NSString* urlString = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
    NSURL *url = [NSURL URLWithString:urlString];
    NSMutableDictionary *params = [[NSMutableDictionary alloc] init];
    for (NSString *param in [[url query] componentsSeparatedByString:@"&"]) {
        NSArray *parts = [param componentsSeparatedByString:@"="];
        if([parts count] < 2) continue;
        [params setObject:[parts objectAtIndex:1] forKey:[parts objectAtIndex:0]];
    }

    double latitude = 0, longitude = 0, zoom = 0, bearing = 0;
    bool hasCenter = false, hasZoom = false, hasBearing = false;

    NSString *centerString = [params objectForKey:@"center"];
    if (centerString) {
        NSArray *latlon = [centerString componentsSeparatedByString:@","];
        if ([latlon count] == 2) {
            latitude = [[latlon objectAtIndex:0] doubleValue];
            longitude = [[latlon objectAtIndex:1] doubleValue];
            hasCenter = true;
        }
    }

    NSString *zoomString = [params objectForKey:@"zoom"];
    if (zoomString) {
        zoom = [zoomString doubleValue];
        hasZoom = true;
    }

    NSString *bearingString = [params objectForKey:@"bearing"];
    if (bearingString) {
        bearing = [bearingString doubleValue];
        hasBearing = true;
    }

    if ([self map]) {
        if (hasCenter && hasZoom) {
            [self map]->setLonLatZoom(longitude, latitude, zoom);
        } else if (hasCenter) {
            [self map]->setLonLat(longitude, latitude);
        } else if (hasZoom) {
            [self map]->setZoom(zoom);
        }

        if (hasBearing) {
            [self map]->setBearing(bearing);
        }
    }
}
@end

int main() {
    mbgl::Log::Set<mbgl::NSLogBackend>();

    GLFWView view;
    mbgl::Map map(view);

    URLHandler *handler = [[URLHandler alloc] init];
    [handler setMap:&map];
    NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
    [appleEventManager setEventHandler:handler andSelector:@selector(handleGetURLEvent:withReplyEvent:) forEventClass:kInternetEventClass andEventID:kAEGetURL];

    // Load settings
    mbgl::Settings_NSUserDefaults settings;
    map.setLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    map.setBearing(settings.bearing);
    map.setDebug(settings.debug);


    // Set access token if present
    NSString *accessToken = [[NSProcessInfo processInfo] environment][@"MAPBOX_ACCESS_TOKEN"];
    if ( ! accessToken) mbgl::Log::Warning(mbgl::Event::Setup, "No access token set. Mapbox vector tiles won't work.");
    if (accessToken) map.setAccessToken([accessToken cStringUsingEncoding:[NSString defaultCStringEncoding]]);

    // Load style
    const std::string path([[[NSBundle mainBundle] pathForResource:@"style" ofType:@"json" inDirectory:@"styles/bright"] UTF8String]);

    map.setStyleURL(std::string("file://") + path);

    int ret = view.run();

    // Save settings
    map.getLonLatZoom(settings.longitude, settings.latitude, settings.zoom);
    settings.bearing = map.getBearing();
    settings.debug = map.getDebug();
    settings.save();

    return ret;
}
