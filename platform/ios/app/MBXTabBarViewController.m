#import "MBXTabBarViewController.h"

@interface MBXTabBarViewController ()

@property (strong) NSTimer *repeatingTimer;

@end

@implementation MBXTabBarViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 5 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        self->_repeatingTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 repeats:YES block:^(NSTimer * _Nonnull timer) {
            self.selectedIndex = self.selectedIndex == 1 ? 0 : 1;
        }];
    });
}

@end
