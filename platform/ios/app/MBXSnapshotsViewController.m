#import "MBXSnapshotsViewController.h"

#import <Mapbox/Mapbox.h>

@interface MBXSnapshotsViewController ()

// Top row
@property (weak, nonatomic) IBOutlet UIImageView *snapshotImageViewTL;
@property (weak, nonatomic) IBOutlet UIImageView *snapshotImageViewTM;
@property (weak, nonatomic) IBOutlet UIImageView *snapshotImageViewTR;

// Bottom row
@property (weak, nonatomic) IBOutlet UIImageView *snapshotImageViewBL;
@property (weak, nonatomic) IBOutlet UIImageView *snapshotImageViewBM;
@property (weak, nonatomic) IBOutlet UIImageView *snapshotImageViewBR;

@end

@implementation MBXSnapshotsViewController {
    // Top row
    MGLMapSnapshotter* topLeftSnapshotter;
    MGLMapSnapshotter* topCenterSnapshotter;
    MGLMapSnapshotter* topRightSnapshotter;
    
    // Bottom row
    MGLMapSnapshotter* bottomLeftSnapshotter;
    MGLMapSnapshotter* bottomCenterSnapshotter;
    MGLMapSnapshotter* bottomRightSnapshotter;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    // Start snapshotters
    topLeftSnapshotter = [self startSnapshotterForImageView:_snapshotImageViewTL coordinates:CLLocationCoordinate2DMake(37.7184, -122.4365)];
    topCenterSnapshotter = [self startSnapshotterForImageView:_snapshotImageViewTM coordinates:CLLocationCoordinate2DMake(38.8936, -77.0146)];
    topRightSnapshotter = [self startSnapshotterForImageView:_snapshotImageViewTR coordinates:CLLocationCoordinate2DMake(-13.1356, -74.2442)];

    bottomLeftSnapshotter = [self startSnapshotterForImageView:_snapshotImageViewBL coordinates:CLLocationCoordinate2DMake(52.5072, 13.4247)];
    bottomCenterSnapshotter = [self startSnapshotterForImageView:_snapshotImageViewBM coordinates:CLLocationCoordinate2DMake(60.2118, 24.6754)];
    bottomRightSnapshotter = [self startSnapshotterForImageView:_snapshotImageViewBR coordinates:CLLocationCoordinate2DMake(31.2780, 121.4286)];
}

- (MGLMapSnapshotter*) startSnapshotterForImageView:(UIImageView*) imageView coordinates:(CLLocationCoordinate2D) coordinates  {
    // Create snapshot options
    MGLMapCamera* mapCamera = [[MGLMapCamera alloc] init];
    mapCamera.pitch = 20;
    mapCamera.centerCoordinate = coordinates;
    MGLMapSnapshotOptions* options = [[MGLMapSnapshotOptions alloc] initWithStyleURL:[MGLStyle satelliteStreetsStyleURL] camera:mapCamera size:CGSizeMake(imageView.frame.size.width, imageView.frame.size.height)];
    options.zoomLevel = 10;
    
    // Create and start the snapshotter
    __weak UIImageView *weakImageView = imageView;
    MGLMapSnapshotter* snapshotter = [[MGLMapSnapshotter alloc] initWithOptions:options];
    [snapshotter startWithCompletionHandler: ^(MGLMapSnapshot* snapshot, NSError *error) {
        if (error) {
            NSLog(@"Could not load snapshot: %@", [error localizedDescription]);
        } else {
            weakImageView.image = snapshot.image;
        }
    }];
    
    return snapshotter;
}


@end
