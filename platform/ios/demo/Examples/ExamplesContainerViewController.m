//
//  ExamplesContainerViewController.m
//  Examples
//
//  Created by Jason Wray on 1/26/16.
//  Copyright © 2016 Mapbox. All rights reserved.
//

#import "ExamplesContainerViewController.h"

@interface ExamplesContainerViewController ()

@end
@implementation ExamplesContainerViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = self.exampleToLoad ?: @"Example Not Specified";

    if (NSClassFromString(self.exampleToLoad)) {
        UIViewController *childViewController = [[NSClassFromString(self.exampleToLoad) alloc] init];
        [self addChildViewController:childViewController];
        [self.view addSubview:childViewController.view];
        [childViewController didMoveToParentViewController:self];
    } else {
        UILabel *exampleNotFoundLabel = [[UILabel alloc] initWithFrame:self.view.frame];
        exampleNotFoundLabel.text = @"Example not found";
        exampleNotFoundLabel.font = [UIFont systemFontOfSize:72.f];
        exampleNotFoundLabel.adjustsFontSizeToFitWidth = YES;
        exampleNotFoundLabel.textAlignment = NSTextAlignmentCenter;
        exampleNotFoundLabel.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        exampleNotFoundLabel.backgroundColor = [UIColor redColor];
        [self.view addSubview:exampleNotFoundLabel];
    }

    self.navigationController.hidesBarsOnSwipe = YES;
    self.navigationController.hidesBarsWhenVerticallyCompact = YES;
}

@end
