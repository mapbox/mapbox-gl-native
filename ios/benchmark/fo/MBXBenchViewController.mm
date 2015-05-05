#import "MBXBenchViewController.h"

#import <mbgl/ios/MGLMapView.h>
#import <mbgl/ios/MGLMapView_private.h>

#include "locations.hpp"

#include <chrono>

@interface MBXBenchViewController () <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MBXBenchViewController

#pragma mark - Setup

- (void)viewDidLoad
{
    [super viewDidLoad];

    NSURL* url = [[NSURL alloc] initWithString:@"asset://styles/mapbox-streets-v7.json"];
    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds accessToken:nil styleURL:url];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.delegate = self;
    self.mapView.zoomEnabled = NO;
    self.mapView.scrollEnabled = NO;
    self.mapView.rotateEnabled = NO;
    self.mapView.userInteractionEnabled = YES;
    [self.mapView setDebugActive:NO];

    [self startBenchmarkIteration];

    [self.view addSubview:self.mapView];

}

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
template <typename Duration>
inline int64_t Microseconds(Duration d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

size_t idx = 0;
enum class State { None, WaitingForAssets, WarmingUp, Benchmarking } state = State::None;
int frames = 0;
TimePoint started;
std::vector<std::pair<std::string, double>> result;

static const int warmupDuration = 20; // frames
static const int benchmarkDuration = 200; // frames

- (void)startBenchmarkIteration
{
    if (mbgl::bench::locations.size() > idx) {
        const auto& location = mbgl::bench::locations[idx];
        [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(location.latitude, location.longitude) zoomLevel:location.zoom animated:NO];
        self.mapView.direction = location.bearing;
        state = State::WaitingForAssets;
        NSLog(@"Benchmarking \"%s\"", location.name.c_str());
        NSLog(@"- Loading assets...");
    } else {
        // Do nothing. The benchmark is completed.
        NSLog(@"Benchmark completed.");
        NSLog(@"Result:");
        size_t colWidth = 0;
        for (const auto& row : result) {
            colWidth = std::max(row.first.size(), colWidth);
        }
        for (const auto& row : result) {
            NSLog(@"| %-*s | %4.1f fps |", int(colWidth), row.first.c_str(), row.second);
        }
        exit(0);
    }
}

- (void)mapViewDidFinishRenderingMap:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered
{
    (void)mapView;
    (void)fullyRendered;

    if (state == State::Benchmarking)
    {
        frames++;
        if (frames >= benchmarkDuration)
        {
            state = State::None;

            // Report FPS
            const auto duration = Microseconds(Clock::now() - started);
            const auto fps = double(frames * 1e6) / duration;
            result.emplace_back(mbgl::bench::locations[idx].name, fps);
            NSLog(@"- FPS: %.1f", fps);

            // Start benchmarking the next location.
            idx++;
            [self startBenchmarkIteration];
        } else {
            [mapView invalidate];
        }
        return;
    }

    else if (state == State::WarmingUp)
    {
        frames++;
        if (frames >= warmupDuration)
        {
            frames = 0;
            state = State::Benchmarking;
            started = Clock::now();
            NSLog(@"- Benchmarking for %d frames...", benchmarkDuration);
        }
        [mapView invalidate];
        return;
    }

    else if (state == State::WaitingForAssets)
    {
        if ([mapView isFullyLoaded])
        {
            // Start the benchmarking timer.
            state = State::WarmingUp;
            [self.mapView emptyMemoryCache];
            NSLog(@"- Warming up for %d frames...", warmupDuration);
            [mapView invalidate];
        }
        return;
    }
}

- (NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskLandscape;
}

@end
