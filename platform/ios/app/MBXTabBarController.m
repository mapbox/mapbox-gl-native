//
//  MBXTabBarController.m
//  iosapp
//
//  Created by Julian Rex on 3/28/19.
//  Copyright © 2019 Mapbox. All rights reserved.
//

#import "MBXTabBarController.h"

@interface MBXTabBarController ()
@property (nonatomic) NSInteger tab;
@end

@implementation MBXTabBarController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    if (NSProcessInfo.processInfo.environment[@"AUTOMATE"]) {
        [NSTimer scheduledTimerWithTimeInterval:0.25
                                        repeats:YES block:^(NSTimer * _Nonnull timer) {
                                            if (self.presentedViewController == nil) {
                                                self.tab = !self.tab;
                                                [self setSelectedIndex:self.tab];
                                            }
                                        }];
    }
}
@end
