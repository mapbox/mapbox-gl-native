#import <UIKit/UIKit.h>
#import <KIF/KIF.h>

@class MGLTViewController, MGLMapView;

@interface KIFTestActor (MapboxGL)

@property (nonatomic, readonly) UIWindow *window;
@property (nonatomic, readonly) MGLTViewController *viewController;
@property (nonatomic, readonly) MGLMapView *mapView;
@property (nonatomic, readonly) UIView *compass;

@end
