#import "OfflinePackExample.h"
@import Mapbox;

NSString *const MBXExampleOfflinePack = @"OfflinePackExample";

@interface OfflinePackExample () <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) UIProgressView *progressView;
@end

@implementation OfflinePackExample

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:[MGLStyle darkStyleURLWithVersion:9]];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.tintColor = [UIColor lightGrayColor];
    self.mapView.delegate = self;
    [self.view addSubview:self.mapView];

    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(22.27933, 114.16281)
                            zoomLevel:13
                             animated:NO];

    // Setup offline pack notification handlers.
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(offlinePackProgressDidChange:) name:MGLOfflinePackProgressChangedNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(offlinePackDidReceiveError:) name:MGLOfflinePackErrorNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(offlinePackDidReceiveMaximumAllowedMapboxTiles:) name:MGLOfflinePackMaximumMapboxTilesReachedNotification object:nil];
}

- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView {
    // Start downloading tiles and resources for z13-16.
    [self startOfflinePackDownload];
}

- (void)dealloc {
    // Remove offline pack observers.
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)startOfflinePackDownload {
    // Create a region that includes the current viewport and any tiles needed to view it when zoomed further in.
    // Because tile count grows exponentially with the maximum zoom level, you should be conservative with your `toZoomLevel` setting.
    id <MGLOfflineRegion> region = [[MGLTilePyramidOfflineRegion alloc] initWithStyleURL:self.mapView.styleURL bounds:self.mapView.visibleCoordinateBounds fromZoomLevel:self.mapView.zoomLevel toZoomLevel:16];

    // Store some data for identification purposes alongside the downloaded resources.
    NSDictionary *userInfo = @{ @"name": @"My Offline Pack" };
    NSData *context = [NSKeyedArchiver archivedDataWithRootObject:userInfo];

    // Create and register an offline pack with the shared offline storage object.
    [[MGLOfflineStorage sharedOfflineStorage] addPackForRegion:region withContext:context completionHandler:^(MGLOfflinePack *pack, NSError *error) {
        if (error != nil) {
            // The pack couldn’t be created for some reason.
            NSLog(@"Error: %@", error.localizedFailureReason);
        } else {
            // Start downloading.
            [pack resume];
        }
    }];
}

#pragma mark - MGLOfflinePack notification handlers

- (void)offlinePackProgressDidChange:(NSNotification *)notification {
    MGLOfflinePack *pack = notification.object;

    // Get the associated user info for the pack; in this case, `name = My Offline Pack`
    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:pack.context];

    MGLOfflinePackProgress progress = pack.progress;
    // or [notification.userInfo[MGLOfflinePackProgressUserInfoKey] MGLOfflinePackProgressValue]
    uint64_t completedResources = progress.countOfResourcesCompleted;
    uint64_t expectedResources = progress.countOfResourcesExpected;

    // Calculate current progress percentage.
    float progressPercentage = (float)completedResources / expectedResources;

    // Setup the progress bar.
    if (!self.progressView) {
        self.progressView = [[UIProgressView alloc] initWithProgressViewStyle:UIProgressViewStyleDefault];
        CGSize frame = self.view.bounds.size;
        self.progressView.frame = CGRectMake(frame.width / 4, frame.height * 0.75, frame.width / 2, 10);
        [self.view addSubview:self.progressView];
    }

    [self.progressView setProgress:progressPercentage animated:YES];

    // If this pack has finished, print its size and resource count.
    if (completedResources == expectedResources) {
        NSString *byteCount = [NSByteCountFormatter stringFromByteCount:progress.countOfBytesCompleted countStyle:NSByteCountFormatterCountStyleMemory];
        NSLog(@"Offline pack “%@” completed: %@, %llu resources", userInfo[@"name"], byteCount, completedResources);
    } else {
        // Otherwise, print download/verification progress.
        NSLog(@"Offline pack “%@” has %llu of %llu resources — %.2f%%.", userInfo[@"name"], completedResources, expectedResources, progressPercentage * 100);
    }
}

- (void)offlinePackDidReceiveError:(NSNotification *)notification {
    MGLOfflinePack *pack = notification.object;
    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:pack.context];
    NSError *error = notification.userInfo[MGLOfflinePackUserInfoKeyError];
    NSLog(@"Offline pack “%@” received error: %@", userInfo[@"name"], error.localizedFailureReason);
}

- (void)offlinePackDidReceiveMaximumAllowedMapboxTiles:(NSNotification *)notification {
    MGLOfflinePack *pack = notification.object;
    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:pack.context];
    uint64_t maximumCount = [notification.userInfo[MGLOfflinePackUserInfoKeyMaximumCount] unsignedLongLongValue];
    NSLog(@"Offline pack “%@” reached limit of %llu tiles.", userInfo[@"name"], maximumCount);
}

@end
