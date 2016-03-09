#import "MBXDownloadsTableViewController.h"

#import <Mapbox/Mapbox.h>

static NSString * const MBXOfflineTaskContextNameKey = @"Name";

static NSString * const MBXDownloadsTableViewInactiveCellReuseIdentifier = @"Inactive";
static NSString * const MBXDownloadsTableViewActiveCellReuseIdentifier = @"Active";

@implementation MGLOfflineTask (MBXAdditions)

- (NSString *)name {
    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:self.context];
    NSAssert([userInfo isKindOfClass:[NSDictionary class]], @"Context of offline task isn’t a dictionary.");
    NSString *name = userInfo[MBXOfflineTaskContextNameKey];
    NSAssert([name isKindOfClass:[NSString class]], @"Name of offline task isn’t a string.");
    return name;
}

@end

@implementation MGLTilePyramidOfflineRegion (MBXAdditions)

- (void)applyToMapView:(MGLMapView *)mapView {
    mapView.styleURL = self.styleURL;
    [mapView setVisibleCoordinateBounds:self.bounds];
}

@end

@interface MBXDownloadsTableViewController () <MGLOfflineTaskDelegate>

@property (nonatomic, strong) NS_MUTABLE_ARRAY_OF(MGLOfflineTask *) *offlineTasks;

@end

@implementation MBXDownloadsTableViewController {
    NSUInteger _untitledRegionCount;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    __weak MBXDownloadsTableViewController *weakSelf = self;
    [[MGLOfflineStorage sharedOfflineStorage] getTasksWithCompletionHandler:^(NS_ARRAY_OF(MGLOfflineTask *) *tasks, NSError *error) {
        MBXDownloadsTableViewController *strongSelf = weakSelf;
        strongSelf.offlineTasks = tasks.mutableCopy;
        [strongSelf.tableView reloadData];
        
        for (MGLOfflineTask *task in strongSelf.offlineTasks) {
            task.delegate = strongSelf;
            [task requestProgress];
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
        
        MGLTilePyramidOfflineRegion *region = [[MGLTilePyramidOfflineRegion alloc] initWithStyleURL:mapView.styleURL bounds:mapView.visibleCoordinateBounds fromZoomLevel:mapView.zoomLevel toZoomLevel:mapView.maximumZoomLevel];
        NSData *context = [NSKeyedArchiver archivedDataWithRootObject:@{
            MBXOfflineTaskContextNameKey: name,
        }];
        
        __weak MBXDownloadsTableViewController *weakSelf = self;
        [[MGLOfflineStorage sharedOfflineStorage] addTaskForRegion:region withContext:context completionHandler:^(MGLOfflineTask *task, NSError *error) {
            MBXDownloadsTableViewController *strongSelf = weakSelf;
            if (error) {
                NSString *message = [NSString stringWithFormat:@"Mapbox GL was unable to add the download “%@”.", name];
                UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Can’t Add Download" message:message preferredStyle:UIAlertControllerStyleAlert];
                [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
                [self presentViewController:alertController animated:YES completion:nil];
            } else {
                task.delegate = strongSelf;
                [task resume];
                
                NSIndexPath *indexPath = [NSIndexPath indexPathForRow:strongSelf.offlineTasks.count inSection:0];
                [strongSelf.offlineTasks addObject:task];
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
    return self.offlineTasks.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    MGLOfflineTask *task = self.offlineTasks[indexPath.row];
    
    NSString *reuseIdentifier = task.state == MGLOfflineTaskStateActive ? MBXDownloadsTableViewActiveCellReuseIdentifier : MBXDownloadsTableViewInactiveCellReuseIdentifier;
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:reuseIdentifier forIndexPath:indexPath];
    cell.textLabel.text = task.name;
    MGLOfflineTaskProgress progress = task.progress;
    NSString *completedString = [NSNumberFormatter localizedStringFromNumber:@(progress.countOfResourcesCompleted)
                                                                 numberStyle:NSNumberFormatterDecimalStyle];
    NSString *expectedString = [NSNumberFormatter localizedStringFromNumber:@(progress.countOfResourcesExpected)
                                                                numberStyle:NSNumberFormatterDecimalStyle];
    NSString *byteCountString = [NSByteCountFormatter stringFromByteCount:progress.countOfBytesCompleted
                                                               countStyle:NSByteCountFormatterCountStyleFile];
    NSString *statusString;
    switch (task.state) {
        case MGLOfflineTaskStateUnknown:
            statusString = @"Calculating progress…";
            break;
            
        case MGLOfflineTaskStateInactive:
            statusString = [NSString stringWithFormat:@"%@ of %@ resources (%@)",
                            completedString, expectedString, byteCountString];
            break;
            
        case MGLOfflineTaskStateComplete:
            statusString = [NSString stringWithFormat:@"%@ resources (%@)",
                            completedString, byteCountString];
            break;
            
        case MGLOfflineTaskStateActive:
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
            
        case MGLOfflineTaskStateInvalid:
            NSAssert(NO, @"Invalid offline task at index path %@", indexPath);
            break;
    }
    cell.detailTextLabel.text = statusString;
    
    return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        MGLOfflineTask *task = self.offlineTasks[indexPath.row];
        __weak MBXDownloadsTableViewController *weakSelf = self;
        [[MGLOfflineStorage sharedOfflineStorage] removeTask:task withCompletionHandler:^(NSError *error) {
            MBXDownloadsTableViewController *strongSelf = weakSelf;
            [strongSelf.offlineTasks removeObjectAtIndex:indexPath.row];
            [strongSelf.tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
        }];
    }
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    MGLOfflineTask *task = self.offlineTasks[indexPath.row];
    switch (task.state) {
        case MGLOfflineTaskStateUnknown:
            break;
            
        case MGLOfflineTaskStateComplete:
            if ([task.region respondsToSelector:@selector(applyToMapView:)]) {
                [task.region performSelector:@selector(applyToMapView:) withObject:self.mapView];
            }
            [self performSegueWithIdentifier:@"ReturnToMap" sender:self];
            break;
            
        case MGLOfflineTaskStateInactive:
            [task resume];
            [tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
            break;
            
        case MGLOfflineTaskStateActive:
            [task suspend];
            [tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
            break;
            
        case MGLOfflineTaskStateInvalid:
            NSAssert(NO, @"Invalid offline task at index path %@", indexPath);
            break;
    }
}

#pragma mark - Offline task delegate

- (void)offlineTask:(MGLOfflineTask *)task progressDidChange:(MGLOfflineTaskProgress)progress {
    NSUInteger index = [self.offlineTasks indexOfObject:task];
    if (index == NSNotFound) {
        return;
    }
    
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:index inSection:0];
    [self.tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationNone];
}

- (void)offlineTask:(MGLOfflineTask *)task didReceiveError:(NSError *)error {
    NSLog(@"Offline task “%@” received error: %@", task.name, error.localizedFailureReason);
}

- (void)offlineTask:(MGLOfflineTask *)task didReceiveMaximumAllowedMapboxTiles:(uint64_t)maximumCount {
    NSLog(@"Offline task “%@” reached limit of %llu tiles.", task.name, maximumCount);
}

@end
