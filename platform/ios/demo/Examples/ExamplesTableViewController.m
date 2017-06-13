//
//  ExamplesTableViewController.m
//  Examples
//
//  Created by Jason Wray on 1/26/16.
//  Copyright © 2016 Mapbox. All rights reserved.
//

#import "ExamplesTableViewController.h"
#import "Examples.h"
#import "ExamplesContainerViewController.h"

NSString *const MBXSegueTableToExample = @"TableToExampleSegue";

@interface ExamplesTableViewController ()

@property (nonatomic) NSArray *examples;

@end
@implementation ExamplesTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"" style:self.navigationItem.backBarButtonItem.style target:nil action:nil];

    // do this ourselves, as automatic doesn't work with fast swipes
    self.clearsSelectionOnViewWillAppear = NO;

    // testing: explicitly jump to an example, later defined in prepareForSegue
    //[self performSegueWithIdentifier:MBXSegueTableToExample sender:self];
}

-(void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:animated];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (!self.examples) {
        self.examples = [Examples list];
    }

    return self.examples.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"ExampleCell" forIndexPath:indexPath];
    
    cell.textLabel.text = self.examples[indexPath.row];
    
    return cell;
}

#pragma mark - Navigation

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:MBXSegueTableToExample]) {
        if ([sender isKindOfClass:[UITableViewCell class]]) {
            UITableViewCell *senderCell = sender;
            ExamplesContainerViewController *destinationVC = [segue destinationViewController];
            destinationVC.exampleToLoad = senderCell.textLabel.text;
        } /* else {
            ExamplesContainerViewController *destinationVC = [segue destinationViewController];
            destinationVC.exampleToLoad = MBXExampleOfflinePack;
        } */
    }
}

@end
