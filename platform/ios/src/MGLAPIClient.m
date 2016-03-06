#import "MGLAPIClient.h"
#import "NSBundle+MGLAdditions.h"
#import "MGLAccountManager.h"

static NSString * const MGLAPIClientUserAgentBase = @"MapboxEventsiOS";
static NSString * const MGLAPIClientBaseURL = @"https://api.mapbox.com";
static NSString * const MGLAPIClientEventsPath = @"events/v2";

static NSString * const MGLAPIClientHeaderFieldUserAgentKey = @"User-Agent";
static NSString * const MGLAPIClientHeaderFieldContentTypeKey = @"Content-Type";
static NSString * const MGLAPIClientHeaderFieldContentTypeValue = @"application/json";
static NSString * const MGLAPIClientHTTPMethodPost = @"POST";

@interface MGLAPIClient ()

@property (nonatomic, copy) NSURLSession *session;
@property (nonatomic, copy) NSString *baseURL;
@property (nonatomic, copy) NSData *digicertCert;
@property (nonatomic, copy) NSData *geoTrustCert;
@property (nonatomic, copy) NSData *testServerCert;
@property (nonatomic, copy) NSString *userAgent;
@property (nonatomic) NSMutableArray *dataTasks;
@property (nonatomic) BOOL usesTestServer;

@end

@implementation MGLAPIClient

- (instancetype)init {
    self = [super init];
    if (self) {
        _session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]
                                                 delegate:self delegateQueue:nil];
        _dataTasks = [NSMutableArray array];
        [self loadCertificates];
        [self setupBaseURL];
        [self setupUserAgent];
    }
    return self;
}

#pragma mark Public API

- (void)postEvents:(nonnull NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events completionHandler:(nullable void (^)(NSError * _Nullable error))completionHandler {
    __block NSURLSessionDataTask *dataTask = [self.session dataTaskWithRequest:[self requestForEvents:events]
                                                             completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
        NSError *statusError = nil;
        if (httpResponse.statusCode >= 400) {
            NSString *description = [NSString stringWithFormat:NSLocalizedString(@"The session data task failed. Original request was: %@", nil), dataTask.originalRequest];
            NSString *reason = [NSString stringWithFormat:NSLocalizedString(@"The status code was %ld", nil), (long)httpResponse.statusCode];
            NSDictionary *userInfo = @{NSLocalizedDescriptionKey: description,
                                       NSLocalizedFailureReasonErrorKey: reason};
            statusError = [NSError errorWithDomain:MGLErrorDomain code:1 userInfo:userInfo];
        }
        if (completionHandler) {
            error = error ? error : statusError;
            completionHandler(error);
        }
        [self.dataTasks removeObject:dataTask];
        dataTask = nil;
    }];
    [dataTask resume];
    [self.dataTasks addObject:dataTask];
}

- (void)postEvent:(nonnull MGLMapboxEventAttributes *)event completionHandler:(nullable void (^)(NSError * _Nullable error))completionHandler {
    [self postEvents:@[event] completionHandler:completionHandler];
}

- (void)cancelAll {
    [self.dataTasks makeObjectsPerformSelector:@selector(cancel)];
    [self.dataTasks removeAllObjects];
}

#pragma mark Utilities

- (NSURLRequest *)requestForEvents:(NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events {
    NSString *url = [NSString stringWithFormat:@"%@/%@?access_token=%@", self.baseURL, MGLAPIClientEventsPath, [MGLAccountManager accessToken]];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:url]];
    [request setValue:self.userAgent forHTTPHeaderField:MGLAPIClientHeaderFieldUserAgentKey];
    [request setValue:MGLAPIClientHeaderFieldContentTypeValue forHTTPHeaderField:MGLAPIClientHeaderFieldContentTypeKey];
    [request setHTTPMethod:MGLAPIClientHTTPMethodPost];
    NSData *jsonData = [self serializedDataForEvents:events];
    [request setHTTPBody:jsonData];
    return [request copy];
}

- (void)setupBaseURL {
    NSString *testServerURL = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLMetricsTestServerURL"];
    if (testServerURL) {
        _baseURL = testServerURL;
        _usesTestServer = YES;
    } else {
        _baseURL = MGLAPIClientBaseURL;
    }
}

- (void)loadCertificates {
    NSData *certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_com-geotrust"];
    self.geoTrustCert = certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_com-digicert"];
    self.digicertCert = certificate;
    [self loadCertificate:&certificate withResource:@"star_tilestream_net"];
    self.testServerCert = certificate;
}

- (void)loadCertificate:(NSData **)certificate withResource:(NSString *)resource {
    NSBundle *resourceBundle = [NSBundle mgl_frameworkBundle];
    NSString *cerPath = [resourceBundle pathForResource:resource ofType:@"der"];
    if (cerPath != nil) {
        *certificate = [NSData dataWithContentsOfFile:cerPath];
    }
}

- (void)setupUserAgent {
    NSString *appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
    NSString *appVersion = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
    NSString *appBuildNumber = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];
    NSString *semanticVersion = [NSBundle mgl_frameworkBundle].infoDictionary[@"MGLSemanticVersionString"];
    NSString *shortVersion = [NSBundle mgl_frameworkBundle].infoDictionary[@"CFBundleShortVersionString"];
    NSString *sdkVersion = semanticVersion ? semanticVersion : shortVersion;
    _userAgent = [NSString stringWithFormat:@"%@/%@/%@ %@/%@", appName, appVersion, appBuildNumber, MGLAPIClientUserAgentBase, sdkVersion];
}

#pragma mark - JSON Serialization

- (NSData *)serializedDataForEvents:(NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events {
    return [NSJSONSerialization dataWithJSONObject:events options:0 error:nil];
}

#pragma mark NSURLSessionDelegate

- (void)URLSession:(NSURLSession *)session didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^) (NSURLSessionAuthChallengeDisposition disposition, NSURLCredential *credential))completionHandler {
    if([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust]) {
        
        SecTrustRef serverTrust = [[challenge protectionSpace] serverTrust];
        SecTrustResultType trustResult;
        
        // Validate the certificate chain with the device's trust store anyway
        // This *might* give use revocation checking
        SecTrustEvaluate(serverTrust, &trustResult);
        if (trustResult == kSecTrustResultUnspecified)
        {
            // Look for a pinned certificate in the server's certificate chain
            long numKeys = SecTrustGetCertificateCount(serverTrust);
            
            BOOL found = NO;
            // Try GeoTrust Cert First
            for (int lc = 0; lc < numKeys; lc++) {
                SecCertificateRef certificate = SecTrustGetCertificateAtIndex(serverTrust, lc);
                NSData *remoteCertificateData = CFBridgingRelease(SecCertificateCopyData(certificate));
                
                // Compare Remote Key With Local Version
                if ([remoteCertificateData isEqualToData:_geoTrustCert]) {
                    // Found the certificate; continue connecting
                    completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                    found = YES;
                    break;
                }
            }
            
            if (!found) {
                // Fallback to Digicert Cert
                for (int lc = 0; lc < numKeys; lc++) {
                    SecCertificateRef certificate = SecTrustGetCertificateAtIndex(serverTrust, lc);
                    NSData *remoteCertificateData = CFBridgingRelease(SecCertificateCopyData(certificate));
                    
                    // Compare Remote Key With Local Version
                    if ([remoteCertificateData isEqualToData:_digicertCert]) {
                        // Found the certificate; continue connecting
                        completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                        found = YES;
                        break;
                    }
                }
                
                if (!found && _usesTestServer) {
                    // See if this is test server
                    for (int lc = 0; lc < numKeys; lc++) {
                        SecCertificateRef certificate = SecTrustGetCertificateAtIndex(serverTrust, lc);
                        NSData *remoteCertificateData = CFBridgingRelease(SecCertificateCopyData(certificate));
                        
                        // Compare Remote Key With Local Version
                        if ([remoteCertificateData isEqualToData:_testServerCert]) {
                            // Found the certificate; continue connecting
                            completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                            found = YES;
                            break;
                        }
                    }
                }
                
                if (!found) {
                    // The certificate wasn't found in GeoTrust nor Digicert. Cancel the connection.
                    completionHandler(NSURLSessionAuthChallengeCancelAuthenticationChallenge, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                }
            }
        }
        else
        {
            // Certificate chain validation failed; cancel the connection
            completionHandler(NSURLSessionAuthChallengeCancelAuthenticationChallenge, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
        }
    }
}

@end
