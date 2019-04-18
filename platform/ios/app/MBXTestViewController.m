//
//  MBXTestViewController.m
//  iosapp
//
//  Created by Jordan on 4/17/19.
//  Copyright © 2019 Mapbox. All rights reserved.
//

#import "MBXTestViewController.h"

@interface MBXTestViewController ()

@end

@implementation MBXTestViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (IBAction)dismissVC:(id)sender {
    NSLog(@"BYEEEEE");
    [self dismissViewControllerAnimated:NO completion:nil];
}

@end
