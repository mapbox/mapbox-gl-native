#import "MBXDownloadsTableViewController.h"

#import <Mapbox/Mapbox.h>

static NSString * const MBXDownloadContextNameKey = @"Name";

static NSString * const MBXDownloadsTableViewInactiveCellReuseIdentifier = @"Inactive";
static NSString * const MBXDownloadsTableViewActiveCellReuseIdentifier = @"Active";

@implementation MGLDownloadable (MBXAdditions)

- (NSString *)name {
    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:self.context];
    NSAssert([userInfo isKindOfClass:[NSDictionary class]], @"Context of downloadable isn’t a dictionary.");
    NSString *name = userInfo[MBXDownloadContextNameKey];
    NSAssert([name isKindOfClass:[NSString class]], @"Name of downloadable isn’t a string.");
    return name;
}

@end

@interface MBXDownloadsTableViewController () <MGLDownloadableDelegate>

@property (nonatomic, strong) NS_MUTABLE_ARRAY_OF(MGLDownloadable *) *downloadables;

@end

@implementation MBXDownloadsTableViewController {
    NSUInteger _untitledRegionCount;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    __weak MBXDownloadsTableViewController *weakSelf = self;
    [[MGLDownloadController sharedController] requestDownloadablesWithCompletionHandler:^(NS_ARRAY_OF(MGLDownloadable *) *downloadables, NSError *error) {
        MBXDownloadsTableViewController *strongSelf = weakSelf;
        strongSelf.downloadables = downloadables.mutableCopy;
        [strongSelf.tableView reloadData];
        
        for (MGLDownloadable *downloadable in strongSelf.downloadables) {
            downloadable.delegate = strongSelf;
            [downloadable requestProgress];
        }
        
        if (error) {
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Can’t Find Downloads" message:@"Mapbox GL was unable to find the existing downloads." preferredStyle:UIAlertControllerStyleAlert];
            [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
            [self presentViewController:alertController animated:YES completion:^{
                [strongSelf dismissViewControllerAnimated:YES completion:nil];
            }];
        }
    }];
}

- (IBAction)addCurrentRegion:(id)sender {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Add Download" message:@"Choose a name for the download:" preferredStyle:UIAlertControllerStyleAlert];
    [alertController addTextFieldWithConfigurationHandler:nil];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil]];
    
    UIAlertAction *downloadAction = [UIAlertAction actionWithTitle:@"Download" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        MGLMapView *mapView = self.mapView;
        NSAssert(mapView, @"No map view to get the current region from.");
        
        NSString *name = alertController.textFields.firstObject.text;
        if (!name.length) {
            name = [NSString stringWithFormat:@"Untitled %lu", (unsigned long)++_untitledRegionCount];
        }
        
        MGLTilePyramidDownloadRegion *region = [[MGLTilePyramidDownloadRegion alloc] initWithStyleURL:mapView.styleURL bounds:mapView.visibleCoordinateBounds fromZoomLevel:mapView.zoomLevel toZoomLevel:mapView.maximumZoomLevel];
        NSData *context = [NSKeyedArchiver archivedDataWithRootObject:@{
            MBXDownloadContextNameKey: name,
        }];
        
        __weak MBXDownloadsTableViewController *weakSelf = self;
        [[MGLDownloadController sharedController] addDownloadableForRegion:region withContext:context completionHandler:^(MGLDownloadable *downloadable, NSError *error) {
            MBXDownloadsTableViewController *strongSelf = weakSelf;
            if (error) {
                NSString *message = [NSString stringWithFormat:@"Mapbox GL was unable to add the download “%@”.", name];
                UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Can’t Add Download" message:message preferredStyle:UIAlertControllerStyleAlert];
                [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
                [self presentViewController:alertController animated:YES completion:nil];
            } else {
                downloadable.delegate = strongSelf;
                [downloadable resume];
                
                NSIndexPath *indexPath = [NSIndexPath indexPathForRow:strongSelf.downloadables.count inSection:0];
                [strongSelf.downloadables addObject:downloadable];
                [strongSelf.tableView insertRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
            }
        }];
    }];
    [alertController addAction:downloadAction];
    alertController.preferredAction = downloadAction;
    
    [self presentViewController:alertController animated:YES completion:nil];
}

#pragma mark - Table view data source

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.downloadables.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    MGLDownloadable *downloadable = self.downloadables[indexPath.row];
    
    NSString *reuseIdentifier = downloadable.state == MGLDownloadableStateActive ? MBXDownloadsTableViewActiveCellReuseIdentifier : MBXDownloadsTableViewInactiveCellReuseIdentifier;
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:reuseIdentifier forIndexPath:indexPath];
    cell.textLabel.text = downloadable.name;
    MGLDownloadableProgress progress = downloadable.progress;
    NSString *completedString = [NSNumberFormatter localizedStringFromNumber:@(progress.countOfResourcesCompleted)
                                                                 numberStyle:NSNumberFormatterDecimalStyle];
    NSString *expectedString = [NSNumberFormatter localizedStringFromNumber:@(progress.countOfResourcesExpected)
                                                                numberStyle:NSNumberFormatterDecimalStyle];
    NSString *byteCountString = [NSByteCountFormatter stringFromByteCount:progress.countOfBytesCompleted
                                                               countStyle:NSByteCountFormatterCountStyleFile];
    NSString *statusString;
    switch (downloadable.state) {
        case MGLDownloadableStateInactive:
        case MGLDownloadableStateComplete:
            statusString = [NSString stringWithFormat:@"%@ of %@ resources (%@)",
                            completedString, expectedString, byteCountString];
            break;
            
        case MGLDownloadableStateActive:
            if (progress.countOfResourcesExpected) {
                completedString = [NSNumberFormatter localizedStringFromNumber:@(progress.countOfResourcesCompleted + 1)
                                                                   numberStyle:NSNumberFormatterDecimalStyle];
            }
            if (progress.maximumResourcesExpected > progress.countOfResourcesExpected) {
                expectedString = [@"at least " stringByAppendingString:expectedString];
            }
            statusString = [NSString stringWithFormat:@"Downloading %@ of %@ resources (%@ so far)…",
                            completedString, expectedString, byteCountString];
            break;
    }
    cell.detailTextLabel.text = statusString;
    
    return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        MGLDownloadable *downloadable = self.downloadables[indexPath.row];
        __weak MBXDownloadsTableViewController *weakSelf = self;
        [[MGLDownloadController sharedController] removeDownloadable:downloadable withCompletionHandler:^(NSError *error) {
            MBXDownloadsTableViewController *strongSelf = weakSelf;
            [strongSelf.downloadables removeObjectAtIndex:indexPath.row];
            [strongSelf.tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
        }];
    }
}

#pragma mark - Downloadable delegate

- (void)downloadable:(MGLDownloadable *)downloadable progressDidChange:(MGLDownloadableProgress)progress {
    NSUInteger index = [self.downloadables indexOfObject:downloadable];
    if (index == NSNotFound) {
        return;
    }
    
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:index inSection:0];
    [self.tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationNone];
}

- (void)downloadable:(MGLDownloadable *)downloadable didReceiveError:(NSError *)error {
    NSLog(@"Downloadable “%@” received error: %@", downloadable.name, error.localizedFailureReason);
}

- (void)downloadable:(MGLDownloadable *)downloadable didReceiveMaximumAllowedMapboxTiles:(uint64_t)maximumCount {
    NSLog(@"Downloadable “%@” reached limit of %llu tiles.", downloadable.name, maximumCount);
}

@end
