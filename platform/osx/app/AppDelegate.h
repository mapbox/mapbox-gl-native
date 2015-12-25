#import <Cocoa/Cocoa.h>

extern NSString * const MGLMapboxAccessTokenDefaultsKey;

@class MapDocument;

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (weak) IBOutlet NSWindow *preferencesWindow;

@property (copy) NSURL *pendingURL;

@end

