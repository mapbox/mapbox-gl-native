#import "MBXAnnotationViewBenchViewController.h"

#import <Mapbox/Mapbox.h>
#import "MGLMapView_Private.h"

#include "locations.hpp"

#include <chrono>

@interface CustomAnnotationView : MGLAnnotationView
@end

@implementation CustomAnnotationView

- (void)layoutSubviews {
    [super layoutSubviews];

    // Force the annotation view to maintain a constant size when the map is tilted.
    self.scalesWithViewingDistance = YES;

    // Use CALayer’s corner radius to turn this view into a circle.
    self.layer.cornerRadius = self.frame.size.width / 2;
    self.layer.borderWidth = 2;
    self.layer.borderColor = [UIColor whiteColor].CGColor;
}

@end


@interface MBXAnnotationViewBenchViewController () <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MBXAnnotationViewBenchViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:[NSURL URLWithString:@"asset://styles/empty.json"]];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.delegate = self;
    self.mapView.userInteractionEnabled = NO;

    [self.view addSubview:self.mapView];
}

- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView {
    [self fillMapWithAnnotations];
}

- (void)mapView:(MGLMapView *)mapView didAddAnnotationViews:(NSArray<MGLAnnotationView *> *)annotationViews {
    NSLog(@"Added %lu annotation views", (unsigned long)annotationViews.count);
    [self startBenchmarkIteration];
}

- (void)fillMapWithAnnotations {
    NSMutableArray *annotations = [NSMutableArray array];

    for (NSInteger latitude = -89; latitude <= 90; latitude += 10)
    {
        for (NSInteger longitude = -180; longitude <= 180; longitude += 2)
        {
            //if (annotations.count >= 3000) break;

            MGLPointAnnotation *annotation = [[MGLPointAnnotation alloc] init];
            annotation.coordinate = CLLocationCoordinate2DMake(latitude, longitude);
            [annotations addObject:annotation];
        }
    }

    [self.mapView addAnnotations:annotations];
}

- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id<MGLAnnotation>)annotation {
    // Use the point annotation’s longitude value (as a string) as the reuse identifier for its view.
    NSString *reuseIdentifier = [NSString stringWithFormat:@"%f", annotation.coordinate.longitude];

    // For better performance, always try to reuse existing annotations.
    CustomAnnotationView *annotationView = [mapView dequeueReusableAnnotationViewWithIdentifier:reuseIdentifier];

    // If there’s no reusable annotation view available, initialize a new one.
    if (!annotationView) {
        annotationView = [[CustomAnnotationView alloc] initWithReuseIdentifier:reuseIdentifier];
        annotationView.frame = CGRectMake(0, 0, 40, 40);

        // Set the annotation view’s background color to a value determined by its longitude.
        CGFloat hue = (CGFloat)annotation.coordinate.longitude / 100;
        annotationView.backgroundColor = [UIColor colorWithHue:hue saturation:0.5 brightness:1 alpha:1];
    }

    return annotationView;
}


size_t idx = 0;
enum class State { None, WarmingUp, Benchmarking } state = State::None;
int frames = 0;
std::chrono::steady_clock::time_point started;
std::vector<std::pair<std::string, double>> result;


- (void)startBenchmarkIteration
{
    if (mbgl::bench::locations.size() > idx) {
        const auto& location = mbgl::bench::locations[idx];

        if (state != State::Benchmarking) state = State::WarmingUp;

        MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:CLLocationCoordinate2DMake(location.latitude, location.longitude)
                                                                fromDistance:location.zoom * 1000000
                                                                       pitch:30
                                                                     heading:location.bearing];
        [self.mapView setCamera:camera withDuration:1.0 animationTimingFunction:NULL completionHandler:^{
            // Start benchmarking the next location.
            idx++;
            [self startBenchmarkIteration];
        }];

        NSLog(@"Benchmarking \"%s\"", location.name.c_str());
    } else {
        // Do nothing. The benchmark is completed.
        state = State::None;
        NSLog(@"Benchmark completed.");
        NSLog(@"Result:");
        // Report FPS
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - started).count() ;
        const auto fps = double(frames * 1e6) / duration;
        NSLog(@"Total frames: %.1d", frames);
        NSLog(@"FPS: %.1f", fps);
        exit(0);
    }
}

- (void)mapViewDidFinishRenderingFrame:(MGLMapView *)mapView fullyRendered:(__unused BOOL)fullyRendered
{
    if (state == State::Benchmarking)
    {
        frames++;

        return;
    }
    else if (state == State::WarmingUp)
    {
        frames = 0;
        state = State::Benchmarking;
        started = std::chrono::steady_clock::now();

        return;
    }
}
@end
