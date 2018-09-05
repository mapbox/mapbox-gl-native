#import "MMETrustKitProvider.h"
#import "MMEHashProvider.h"
#import "MMEEventLogger.h"
#import "TrustKit.h"

@implementation MMETrustKitProvider

+ (TrustKit *)trustKitWithUpdatedConfiguration:(MMEEventsConfiguration *)configuration {

    if (![MMEEventLogger.sharedLogger isEnabled]) {
        void (^loggerBlock)(NSString *) = ^void(NSString *message){};
        [TrustKit setLoggerBlock:loggerBlock];
    }
    
    MMEHashProvider *hashProvider = [[MMEHashProvider alloc] init];
    if (configuration) {
        [hashProvider updateHashesWithConfiguration:configuration];
    }
    
    NSDictionary *trustKitConfig =
    @{
      kTSKSwizzleNetworkDelegates: @NO,
      kTSKPinnedDomains: @{
              /* Production */
              @"events.mapbox.com" : @{
                      kTSKEnforcePinning:@YES,
                      kTSKDisableDefaultReportUri:@YES,
                      kTSKPublicKeyAlgorithms : @[kTSKAlgorithmRsa2048],
                      kTSKPublicKeyHashes :
                          hashProvider.comHashes
                      },
              @"events.mapbox.cn" : @{
                      kTSKEnforcePinning:@YES,
                      kTSKDisableDefaultReportUri:@YES,
                      kTSKPublicKeyAlgorithms : @[kTSKAlgorithmRsa2048],
                      kTSKPublicKeyHashes :
                          hashProvider.cnHashes
                      },
              /* Staging */
              @"api-events-staging.tilestream.net" : @{
                      kTSKEnforcePinning:@YES,
                      kTSKPublicKeyAlgorithms : @[kTSKAlgorithmRsa2048],
                      kTSKPublicKeyHashes : @[
                              // Digicert, SHA1 Fingerprint=C6:1B:FE:8C:59:8F:29:F0:36:2E:88:BB:A2:CD:08:3B:F6:59:08:22
                              @"3euxrJOrEZI15R4104UsiAkDqe007EPyZ6eTL/XxdAY=",
                              // Stub: TrustKit requires 2 hashes for every endpoint
                              @"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=",
                              ]
                      },
              @"api.mapbox.com" : @{
                      kTSKExcludeSubdomainFromParentPolicy: @YES
                      },
              }
      };
    return [[TrustKit alloc] initWithConfiguration:trustKitConfig];
}

@end
