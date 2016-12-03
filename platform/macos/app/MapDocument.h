#import <Cocoa/Cocoa.h>

@class MGLMapView;

@interface MapDocument : NSDocument

@property (weak) IBOutlet MGLMapView *mapView;

- (IBAction)showStyle:(id)sender;
- (IBAction)chooseCustomStyle:(id)sender;

- (IBAction)reload:(id)sender;

@end
