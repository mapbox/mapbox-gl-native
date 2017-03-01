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
    mapView.zoomLevel = MIN(self.maximumZoomLevel, MAX(self.minimumZoomLevel, mapView.zoomLevel));
}

@end

@implementation MBXOfflinePacksTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    [[MGLOfflineStorage sharedOfflineStorage] addObserver:self forKeyPath:@"packs" options:NSKeyValueObservingOptionInitial context:NULL];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(offlinePackProgressDidChange:) name:MGLOfflinePackProgressChangedNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(offlinePackDidReceiveError:) name:MGLOfflinePackErrorNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(offlinePackDidReceiveMaximumAllowedMapboxTiles:) name:MGLOfflinePackMaximumMapboxTilesReachedNotification object:nil];
}

- (void)dealloc {
    [[MGLOfflineStorage sharedOfflineStorage] removeObserver:self forKeyPath:@"packs"];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NS_DICTIONARY_OF(NSString *, id) *)change context:(void *)context {
    if ([keyPath isEqualToString:@"packs"]) {
        NSKeyValueChange changeKind = [change[NSKeyValueChangeKindKey] unsignedIntegerValue];
        NSIndexSet *indices = change[NSKeyValueChangeIndexesKey];
        NSMutableArray *indexPaths;
        if (indices) {
            indexPaths = [NSMutableArray arrayWithCapacity:indices.count];
            [indices enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL * _Nonnull stop) {
                [indexPaths addObject:[NSIndexPath indexPathForRow:idx inSection:0]];
            }];
        }
        switch (changeKind) {
            case NSKeyValueChangeInsertion:
                [self.tableView insertRowsAtIndexPaths:indexPaths withRowAnimation:UITableViewRowAnimationAutomatic];
                break;

            case NSKeyValueChangeRemoval:
                [self.tableView deleteRowsAtIndexPaths:indexPaths withRowAnimation:UITableViewRowAnimationAutomatic];
                break;

            case NSKeyValueChangeReplacement:
                [self.tableView reloadRowsAtIndexPaths:indexPaths withRowAnimation:UITableViewRowAnimationAutomatic];
                break;

            default:
                [self.tableView reloadData];

                for (MGLOfflinePack *pack in [MGLOfflineStorage sharedOfflineStorage].packs) {
                    if (pack.state == MGLOfflinePackStateUnknown) {
                        [pack requestProgress];
                    }
                }

                break;
        }
    } else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}

- (IBAction)addCurrentRegion:(id)sender {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Add Offline Pack" message:@"Choose a name for the pack:" preferredStyle:UIAlertControllerStyleAlert];
    [alertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = [NSString stringWithFormat:@"%@", MGLStringFromCoordinateBounds(self.mapView.visibleCoordinateBounds)];
    }];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil]];

    UIAlertAction *downloadAction = [UIAlertAction actionWithTitle:@"Download" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        MGLMapView *mapView = self.mapView;
        NSAssert(mapView, @"No map view to get the current region from.");

        UITextField *nameField = alertController.textFields.firstObject;
        NSString *name = nameField.text;
        if (!name.length) {
            name = nameField.placeholder;
        }

        MGLTilePyramidOfflineRegion *region = [[MGLTilePyramidOfflineRegion alloc] initWithStyleURL:mapView.styleURL bounds:mapView.visibleCoordinateBounds fromZoomLevel:mapView.zoomLevel toZoomLevel:mapView.maximumZoomLevel];
        NSData *context = [NSKeyedArchiver archivedDataWithRootObject:@{
            MBXOfflinePackContextNameKey: name,
        }];

        [[MGLOfflineStorage sharedOfflineStorage] addPackForRegion:region withContext:context completionHandler:^(MGLOfflinePack *pack, NSError *error) {
            if (error) {
                NSString *message = [NSString stringWithFormat:@"Mapbox GL was unable to add the offline pack “%@”.", name];
                UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Can’t Add Offline Pack" message:message preferredStyle:UIAlertControllerStyleAlert];
                [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
                [self presentViewController:alertController animated:YES completion:nil];
            } else {
                [pack resume];
            }
        }];
    }];
    [alertController addAction:downloadAction];
    if ([alertController respondsToSelector:@selector(setPreferredAction:)]) {
        alertController.preferredAction = downloadAction;
    }

    [self presentViewController:alertController animated:YES completion:nil];
}

#pragma mark - Table view data source

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [MGLOfflineStorage sharedOfflineStorage].packs.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    MGLOfflinePack *pack = [MGLOfflineStorage sharedOfflineStorage].packs[indexPath.row];

    NSString *reuseIdentifier = pack.state == MGLOfflinePackStateActive ? MBXOfflinePacksTableViewActiveCellReuseIdentifier : MBXOfflinePacksTableViewInactiveCellReuseIdentifier;
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:reuseIdentifier forIndexPath:indexPath];
    [self updateTableViewCell:cell atIndexPath:indexPath forPack:pack];

    return cell;
}

- (void)updateTableViewCell:(UITableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath forPack:(MGLOfflinePack *)pack {
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
                expectedString = [NSString stringWithFormat:@"at least %@", expectedString];
            }
            statusString = [NSString stringWithFormat:@"Downloading %@ of %@ resources (%@ so far)…",
                            completedString, expectedString, byteCountString];
            break;

        case MGLOfflinePackStateInvalid:
            NSAssert(NO, @"Invalid offline pack at index path %@", indexPath);
            break;
    }
    cell.detailTextLabel.text = statusString;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        MGLOfflinePack *pack = [MGLOfflineStorage sharedOfflineStorage].packs[indexPath.row];
        [[MGLOfflineStorage sharedOfflineStorage] removePack:pack withCompletionHandler:nil];
    }
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];

    MGLOfflinePack *pack = [MGLOfflineStorage sharedOfflineStorage].packs[indexPath.row];
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
            break;

        case MGLOfflinePackStateActive:
            [pack suspend];
            break;

        case MGLOfflinePackStateInvalid:
            NSAssert(NO, @"Invalid offline pack at index path %@", indexPath);
            break;
    }
}

#pragma mark - Offline pack delegate

- (void)offlinePackProgressDidChange:(NSNotification *)notification {
    MGLOfflinePack *pack = notification.object;
    NSAssert([pack isKindOfClass:[MGLOfflinePack class]], @"MGLOfflineStorage notification has a non-pack object.");

    NSUInteger index = [[MGLOfflineStorage sharedOfflineStorage].packs indexOfObject:pack];
    if (index == NSNotFound) {
        return;
    }

    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:index inSection:0];
    UITableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
    [self updateTableViewCell:cell atIndexPath:indexPath forPack:pack];
}

- (void)offlinePackDidReceiveError:(NSNotification *)notification {
    MGLOfflinePack *pack = notification.object;
    NSAssert([pack isKindOfClass:[MGLOfflinePack class]], @"MGLOfflineStorage notification has a non-pack object.");

    NSError *error = notification.userInfo[MGLOfflinePackUserInfoKeyError];
    NSAssert([error isKindOfClass:[NSError class]], @"MGLOfflineStorage notification has a non-error error.");

    NSString *message = [NSString stringWithFormat:@"Mapbox GL encountered an error while downloading the offline pack “%@”: %@", pack.name, error.localizedFailureReason];
    if (error.code == MGLErrorCodeConnectionFailed) {
        NSLog(@"%@", message);
    } else {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Error Downloading Offline Pack" message:message preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

- (void)offlinePackDidReceiveMaximumAllowedMapboxTiles:(NSNotification *)notification {
    MGLOfflinePack *pack = notification.object;
    NSAssert([pack isKindOfClass:[MGLOfflinePack class]], @"MGLOfflineStorage notification has a non-pack object.");

    uint64_t maximumCount = [notification.userInfo[MGLOfflinePackUserInfoKeyMaximumCount] unsignedLongLongValue];
    NSLog(@"Offline pack “%@” reached limit of %llu tiles.", pack.name, maximumCount);
}

@end
