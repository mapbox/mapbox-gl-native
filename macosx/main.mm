#include <mbgl/platform/log.hpp>
#include "../platform/default/default_styles.hpp"
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/darwin/settings_nsuserdefaults.hpp>
#include <mbgl/platform/darwin/reachability.h>
#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/storage/network_status.hpp>

#include <mbgl/util/geo.hpp>

#import <Foundation/Foundation.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop

#include <iostream>

namespace po = boost::program_options;


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

    mbgl::LatLng latLng = mbgl::LatLng(0, 0);
    double zoom = 0, bearing = 0;
    bool hasCenter = false, hasZoom = false, hasBearing = false;

    NSString *centerString = [params objectForKey:@"center"];
    if (centerString) {
        NSArray *latLngValues = [centerString componentsSeparatedByString:@","];
        if ([latLngValues count] == 2) {
            latLng.latitude  = [latLngValues[0] doubleValue];
            latLng.longitude = [latLngValues[1] doubleValue];
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
            [self map]->setLatLngZoom(latLng, zoom);
        } else if (hasCenter) {
            [self map]->setLatLng(latLng);
        } else if (hasZoom) {
            [self map]->setZoom(zoom);
        }

        if (hasBearing) {
            [self map]->setBearing(bearing);
        }
    }
}
@end

// Returns the path to the default cache database on this system.
const std::string &defaultCacheDatabase() {
    static const std::string path = []() -> std::string {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(
            NSApplicationSupportDirectory, NSUserDomainMask, YES);
        if ([paths count] == 0) {
            // Disable the cache if we don't have a location to write.
            return "";
        }

        NSString *p = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"Mapbox GL"];

        if (![[NSFileManager defaultManager] createDirectoryAtPath:p
                                       withIntermediateDirectories:YES
                                                        attributes:nil
                                                             error:nil]) {
            // Disable the cache if we couldn't create the directory.
            return "";
        }

        return [[p stringByAppendingPathComponent:@"cache.db"] UTF8String];
    }();
    return path;
}

int main(int argc, char* argv[]) {
    bool fullscreen = false;
    bool benchmark = false;
    std::string style;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("fullscreen,f", po::bool_switch(&fullscreen)->default_value(fullscreen), "Fullscreen mode")
        ("style,s", po::value(&style)->value_name("json"), "Map stylesheet")
        ("benchmark,b", po::bool_switch(&benchmark)->default_value(benchmark), "Benchmark mode")
    ;

    try {
        auto parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        po::variables_map vm;
        po::store(parsed, vm);
        po::notify(vm);
    } catch(std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl << desc;
        exit(1);
    }

    if (benchmark) {
        mbgl::Log::Info(mbgl::Event::General, "BENCHMARK MODE: Some optimizations are disabled.");
    }

    GLFWView view(fullscreen, benchmark);

    mbgl::SQLiteCache cache(defaultCacheDatabase());
    mbgl::DefaultFileSource fileSource(&cache);

    // Set access token if present
    NSString *accessToken = [[NSProcessInfo processInfo] environment][@"MAPBOX_ACCESS_TOKEN"];
    if (!accessToken) mbgl::Log::Warning(mbgl::Event::Setup, "No access token set. Mapbox vector tiles won't work.");
    if (accessToken) fileSource.setAccessToken([accessToken cStringUsingEncoding:[NSString defaultCStringEncoding]]);

    mbgl::Map map(view, fileSource);

    URLHandler *handler = [[URLHandler alloc] init];
    [handler setMap:&map];
    NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
    [appleEventManager setEventHandler:handler andSelector:@selector(handleGetURLEvent:withReplyEvent:) forEventClass:kInternetEventClass andEventID:kAEGetURL];

    // Notify map object when network reachability status changes.
    MGLReachability* reachability = [MGLReachability reachabilityForInternetConnection];
    reachability.reachableBlock = ^(MGLReachability *) {
        mbgl::NetworkStatus::Reachable();
    };
    [reachability startNotifier];

    // Load settings
    mbgl::Settings_NSUserDefaults settings;
    map.setLatLngZoom(mbgl::LatLng(settings.latitude, settings.longitude), settings.zoom);
    map.setBearing(settings.bearing);
    map.setDebug(settings.debug);

    view.setChangeStyleCallback([&map, &view] () {
        static uint8_t currentStyleIndex;

        if (++currentStyleIndex == mbgl::util::defaultStyles.size()) {
            currentStyleIndex = 0;
        }

        const auto& newStyle = mbgl::util::defaultStyles[currentStyleIndex];
        map.setStyleURL(newStyle.first);
        view.setWindowTitle(newStyle.second);

        mbgl::Log::Info(mbgl::Event::Setup, std::string("Changed style to: ") + newStyle.first);
    });


    // Load style
    if (style.empty()) {
        const auto& newStyle = mbgl::util::defaultStyles.front();
        style = newStyle.first;
        view.setWindowTitle(newStyle.second);
    }

    map.setStyleURL(style);

    view.run();

    [reachability stopNotifier];

    // Save settings
    mbgl::LatLng latLng = map.getLatLng();
    settings.latitude = latLng.latitude;
    settings.longitude = latLng.longitude;
    settings.zoom = map.getZoom();
    settings.bearing = map.getBearing();
    settings.debug = map.getDebug();
    settings.save();

    return 0;
}
