#import "MBXOfflinePacksTableViewController.h"

#import <Mapbox/Mapbox.h>

static NSString * const MBXOfflinePackContextNameKey = @"Name";

static NSString * const MBXOfflinePacksTableViewInactiveCellReuseIdentifier = @"Inactive";
static NSString * const MBXOfflinePacksTableViewActiveCellReuseIdentifier = @"Active";

@implementation MGLOfflinePack (MBXAdditions)

- (NSString *)name {
    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:self.context];
    NSAssert([userInfo isKindOfClass:[NSDictionary class]], @"Context of offline pack isn’t a dictionary.");
    NSString *name = userInfo[MBXOfflinePackContextNameKey];
    NSAssert([name isKindOfClass:[NSString class]], @"Name of offline pack isn’t a string.");
    return name;
}

@end

@implementation MGLTilePyramidOfflineRegion (MBXAdditions)

- (void)applyToMapView:(MGLMapView *)mapView {
    mapView.styleURL = self.styleURL;
    [mapView setVisibleCoordinateBounds:self.bounds];
}

@end

@interface MBXOfflinePacksTableViewController () <MGLOfflinePackDelegate>

@property (nonatomic, strong) NS_MUTABLE_ARRAY_OF(MGLOfflinePack *) *offlinePacks;

@end

@implementation MBXOfflinePacksTableViewController {
    NSUInteger _untitledRegionCount;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    __weak MBXOfflinePacksTableViewController *weakSelf = self;
    [[MGLOfflineStorage sharedOfflineStorage] getPacksWithCompletionHandler:^(NS_ARRAY_OF(MGLOfflinePack *) *packs, NSError *error) {
        MBXOfflinePacksTableViewController *strongSelf = weakSelf;
        strongSelf.offlinePacks = packs.mutableCopy;
        [strongSelf.tableView reloadData];
        
        for (MGLOfflinePack *pack in strongSelf.offlinePacks) {
            pack.delegate = strongSelf;
            [pack requestProgress];
        }
        
        if (error) {
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Can’t Find Offline Packs" message:@"Mapbox GL was unable to find the existing offline packs." preferredStyle:UIAlertControllerStyleAlert];
            [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
            [self presentViewController:alertController animated:YES completion:^{
                [strongSelf dismissViewControllerAnimated:YES completion:nil];
            }];
        }
    }];
}

- (IBAction)addCurrentRegion:(id)sender {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Add Offline Pack" message:@"Choose a name for the pack:" preferredStyle:UIAlertControllerStyleAlert];
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
            MBXOfflinePackContextNameKey: name,
        }];
        
        __weak MBXOfflinePacksTableViewController *weakSelf = self;
        [[MGLOfflineStorage sharedOfflineStorage] addPackForRegion:region withContext:context completionHandler:^(MGLOfflinePack *pack, NSError *error) {
            MBXOfflinePacksTableViewController *strongSelf = weakSelf;
            if (error) {
                NSString *message = [NSString stringWithFormat:@"Mapbox GL was unable to add the offline pack “%@”.", name];
                UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Can’t Add Offline Pack" message:message preferredStyle:UIAlertControllerStyleAlert];
                [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
                [self presentViewController:alertController animated:YES completion:nil];
            } else {
                pack.delegate = strongSelf;
                [pack resume];
                
                NSIndexPath *indexPath = [NSIndexPath indexPathForRow:strongSelf.offlinePacks.count inSection:0];
                [strongSelf.offlinePacks addObject:pack];
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
    return self.offlinePacks.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    MGLOfflinePack *pack = self.offlinePacks[indexPath.row];
    
    NSString *reuseIdentifier = pack.state == MGLOfflinePackStateActive ? MBXOfflinePacksTableViewActiveCellReuseIdentifier : MBXOfflinePacksTableViewInactiveCellReuseIdentifier;
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:reuseIdentifier forIndexPath:indexPath];
    cell.textLabel.text = pack.name;
    MGLOfflinePackProgress progress = pack.progress;
    NSString *completedString = [NSNumberFormatter localizedStringFromNumber:@(progress.countOfResourcesCompleted)
                                                                 numberStyle:NSNumberFormatterDecimalStyle];
    NSString *expectedString = [NSNumberFormatter localizedStringFromNumber:@(progress.countOfResourcesExpected)
                                                                numberStyle:NSNumberFormatterDecimalStyle];
    NSString *byteCountString = [NSByteCountFormatter stringFromByteCount:progress.countOfBytesCompleted
                                                               countStyle:NSByteCountFormatterCountStyleFile];
    NSString *statusString;
    switch (pack.state) {
        case MGLOfflinePackStateUnknown:
            statusString = @"Calculating progress…";
            break;
            
        case MGLOfflinePackStateInactive:
            statusString = [NSString stringWithFormat:@"%@ of %@ resources (%@)",
                            completedString, expectedString, byteCountString];
            break;
            
        case MGLOfflinePackStateComplete:
            statusString = [NSString stringWithFormat:@"%@ resources (%@)",
                            completedString, byteCountString];
            break;
            
        case MGLOfflinePackStateActive:
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
            
        case MGLOfflinePackStateInvalid:
            NSAssert(NO, @"Invalid offline pack at index path %@", indexPath);
            break;
    }
    cell.detailTextLabel.text = statusString;
    
    return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        MGLOfflinePack *pack = self.offlinePacks[indexPath.row];
        [self.offlinePacks removeObjectAtIndex:indexPath.row];
        [self.tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
        [[MGLOfflineStorage sharedOfflineStorage] removePack:pack withCompletionHandler:nil];
    }
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    MGLOfflinePack *pack = self.offlinePacks[indexPath.row];
    switch (pack.state) {
        case MGLOfflinePackStateUnknown:
            break;
            
        case MGLOfflinePackStateComplete:
            if ([pack.region respondsToSelector:@selector(applyToMapView:)]) {
                [pack.region performSelector:@selector(applyToMapView:) withObject:self.mapView];
            }
            [self performSegueWithIdentifier:@"ReturnToMap" sender:self];
            break;
            
        case MGLOfflinePackStateInactive:
            [pack resume];
            [tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
            break;
            
        case MGLOfflinePackStateActive:
            [pack suspend];
            [tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
            break;
            
        case MGLOfflinePackStateInvalid:
            NSAssert(NO, @"Invalid offline pack at index path %@", indexPath);
            break;
    }
}

#pragma mark - Offline pack delegate

- (void)offlinePack:(MGLOfflinePack *)pack progressDidChange:(MGLOfflinePackProgress)progress {
    NSUInteger index = [self.offlinePacks indexOfObject:pack];
    if (index == NSNotFound) {
        return;
    }
    
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:index inSection:0];
    [self.tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationNone];
}

- (void)offlinePack:(MGLOfflinePack *)pack didReceiveError:(NSError *)error {
    NSLog(@"Offline pack “%@” received error: %@", pack.name, error.localizedFailureReason);
}

- (void)offlinePack:(MGLOfflinePack *)pack didReceiveMaximumAllowedMapboxTiles:(uint64_t)maximumCount {
    NSLog(@"Offline pack “%@” reached limit of %llu tiles.", pack.name, maximumCount);
}

@end
