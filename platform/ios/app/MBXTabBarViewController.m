#import "MBXTabBarViewController.h"

@interface MBXTabBarViewController ()

@property (strong) NSTimer *repeatingTimer;

@end

@implementation MBXTabBarViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    _repeatingTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 repeats:YES block:^(NSTimer * _Nonnull timer) {
        self.selectedIndex = self.selectedIndex == 1 ? 0 : 1;
    }];
}

@end
