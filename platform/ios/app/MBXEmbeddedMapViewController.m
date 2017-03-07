#import "MBXEmbeddedMapViewController.h"
#import <Mapbox/Mapbox.h>

typedef NS_ENUM(NSInteger, MBXEmbeddedControl) {
    MBXEmbeddedControlZoom = 0,
    MBXEmbeddedControlScroll,
    MBXEmbeddedControlRotation,
    MBXEmbeddedControlPitch
};

@interface MBXEmbeddedMapViewController () <UIScrollViewDelegate>

@property (weak, nonatomic) IBOutlet UIScrollView *scrollView;
@property (weak, nonatomic) IBOutlet MGLMapView *mapView;

@end

@implementation MBXEmbeddedMapViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.scrollView.delegate = self;
    self.scrollView.contentSize = self.view.bounds.size;
}

- (IBAction)didSwitch:(UISwitch *)controlSwitch {
    [self switchControl:controlSwitch.tag];
}

- (IBAction)rotation:(UIRotationGestureRecognizer *)rotationGesture {
    self.mapView.transform = CGAffineTransformRotate(rotationGesture.view.transform, rotationGesture.rotation);
}

- (void)switchControl:(MBXEmbeddedControl) control {
    switch (control) {
        case MBXEmbeddedControlZoom:
            self.mapView.zoomEnabled = !self.mapView.zoomEnabled;
            break;
        case MBXEmbeddedControlScroll:
            self.mapView.scrollEnabled = !self.mapView.scrollEnabled;
            break;
        case MBXEmbeddedControlRotation:
            self.mapView.rotateEnabled = !self.mapView.rotateEnabled;
            break;
        case MBXEmbeddedControlPitch:
            self.mapView.pitchEnabled = !self.mapView.pitchEnabled;
            break;
    }
}

- (BOOL)statusForControl:(MBXEmbeddedControl) control {
    switch (control) {
        case MBXEmbeddedControlZoom:
            return self.mapView.zoomEnabled;
        case MBXEmbeddedControlScroll:
            return self.mapView.scrollEnabled;
        case MBXEmbeddedControlRotation:
            return self.mapView.rotateEnabled;
        case MBXEmbeddedControlPitch:
            return self.mapView.pitchEnabled;
    }
}

#pragma mark UIScrollViewDelegate methods

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView {
    return self.mapView;
}

#pragma mark Class method

+ (NSString *)titleForControl:(MBXEmbeddedControl) control {
    switch (control) {
        case MBXEmbeddedControlZoom:
            return @"Zoom Enabled";
        case MBXEmbeddedControlScroll:
            return @"Scroll Enabled";
            break;
        case MBXEmbeddedControlRotation:
            return @"Rotation Enabled";
            break;
        case MBXEmbeddedControlPitch:
            return @"Pitch Enabled";
            break;
    }
}

@end
