#import "MBXBenchViewController.h"

#import "MBXBenchAppDelegate.h"

#import <Mapbox/Mapbox.h>
#import "MGLMapView_Private.h"

#include "locations.hpp"

#include <chrono>

@interface MBXBenchViewController () <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MBXBenchViewController

#pragma mark - Setup

+ (void)initialize
{
    if (self == [MBXBenchViewController class])
    {
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{
            @"MBXUserTrackingMode": @(MGLUserTrackingModeNone),
            @"MBXShowsUserLocation": @NO,
            @"MBXDebug": @NO,
        }];
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Use a local style and local assets if they’ve been downloaded.
    NSURL *tileSourceURL = [[NSBundle mainBundle] URLForResource:@"mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6" withExtension:nil subdirectory:@"tiles"];
    NSURL *url = [NSURL URLWithString:tileSourceURL ? @"asset://styles/streets-v8.json" : @"mapbox://styles/mapbox/streets-v8"];
    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:url];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.delegate = self;
    self.mapView.zoomEnabled = NO;
    self.mapView.scrollEnabled = NO;
    self.mapView.rotateEnabled = NO;
    self.mapView.userInteractionEnabled = YES;

    [self.view addSubview:self.mapView];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];

    if ([MGLAccountManager accessToken].length) {
        [self startBenchmarkIteration];
    } else {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Access Token" message:@"Enter your Mapbox access token to load Mapbox-hosted tiles and styles:" preferredStyle:UIAlertControllerStyleAlert];
        [alertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
            textField.keyboardType = UIKeyboardTypeURL;
            textField.autocorrectionType = UITextAutocorrectionTypeNo;
            textField.autocapitalizationType = UITextAutocapitalizationTypeNone;
        }];

        [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            [self startBenchmarkIteration];
        }]];
        UIAlertAction *OKAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            UITextField *textField = alertController.textFields.firstObject;
            NSString *accessToken = textField.text;
            [[NSUserDefaults standardUserDefaults] setObject:accessToken forKey:MBXMapboxAccessTokenDefaultsKey];
            [MGLAccountManager setAccessToken:accessToken];
            [self.mapView reloadStyle:self];

            [self startBenchmarkIteration];
        }];
        [alertController addAction:OKAction];

        if ([alertController respondsToSelector:@selector(setPreferredAction:)]) {
            alertController.preferredAction = OKAction;
        }
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

size_t idx = 0;
enum class State { None, WaitingForAssets, WarmingUp, Benchmarking } state = State::None;
int frames = 0;
std::chrono::steady_clock::time_point started;
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
        double totalFPS = 0;
        size_t colWidth = 0;
        for (const auto& row : result) {
            colWidth = std::max(row.first.size(), colWidth);
        }
        for (const auto& row : result) {
            NSLog(@"| %-*s | %4.1f fps |", int(colWidth), row.first.c_str(), row.second);
            totalFPS += row.second;
        }
        NSLog(@"Total FPS: %4.1f", totalFPS);
        NSLog(@"Average FPS: %4.1f", totalFPS / result.size());
        exit(0);
    }
}

- (void)mapViewDidFinishRenderingFrame:(MGLMapView *)mapView fullyRendered:(__unused BOOL)fullyRendered
{
    if (state == State::Benchmarking)
    {
        frames++;
        if (frames >= benchmarkDuration)
        {
            state = State::None;

            // Report FPS
            const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - started).count() ;
            const auto fps = double(frames * 1e6) / duration;
            result.emplace_back(mbgl::bench::locations[idx].name, fps);
            NSLog(@"- FPS: %.1f", fps);

            // Start benchmarking the next location.
            idx++;
            [self startBenchmarkIteration];
        } else {
            [mapView setNeedsGLDisplay];
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
            started = std::chrono::steady_clock::now();
            NSLog(@"- Benchmarking for %d frames...", benchmarkDuration);
        }
        [mapView setNeedsGLDisplay];
        return;
    }

    else if (state == State::WaitingForAssets)
    {
        if ([mapView isFullyLoaded])
        {
            // Start the benchmarking timer.
            state = State::WarmingUp;
            [self.mapView didReceiveMemoryWarning];
            NSLog(@"- Warming up for %d frames...", warmupDuration);
            [mapView setNeedsGLDisplay];
        }
        return;
    }
}

- (NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskLandscape;
}

@end
