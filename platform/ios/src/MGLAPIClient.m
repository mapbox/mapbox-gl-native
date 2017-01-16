#import "MGLAPIClient.h"
#import "NSBundle+MGLAdditions.h"
#import "NSData+MGLAdditions.h"
#import "MGLAccountManager.h"

static NSString * const MGLAPIClientUserAgentBase = @"MapboxEventsiOS";
static NSString * const MGLAPIClientBaseURL = @"https://events.mapbox.com";
static NSString * const MGLAPIClientEventsPath = @"events/v2";

static NSString * const MGLAPIClientHeaderFieldUserAgentKey = @"User-Agent";
static NSString * const MGLAPIClientHeaderFieldContentTypeKey = @"Content-Type";
static NSString * const MGLAPIClientHeaderFieldContentTypeValue = @"application/json";
static NSString * const MGLAPIClientHeaderFieldContentEncodingKey = @"Content-Encoding";
static NSString * const MGLAPIClientHTTPMethodPost = @"POST";

@interface MGLAPIClient ()

@property (nonatomic, copy) NSURLSession *session;
@property (nonatomic, copy) NSURL *baseURL;
@property (nonatomic, copy) NSData *digicertCert;
@property (nonatomic, copy) NSData *geoTrustCert;
@property (nonatomic, copy) NSData *testServerCert;
@property (nonatomic, copy) NSString *userAgent;
@property (nonatomic) BOOL usesTestServer;

@end

@implementation MGLAPIClient

- (instancetype)init {
    self = [super init];
    if (self) {
        _session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]
                                                 delegate:self delegateQueue:nil];
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
            NSString *description = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"API_CLIENT_400_DESC", nil, nil, @"The session data task failed. Original request was: %@", nil), dataTask.originalRequest];
            NSString *reason = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"API_CLIENT_400_REASON", nil, nil, @"The status code was %ld", nil), (long)httpResponse.statusCode];
            NSDictionary *userInfo = @{NSLocalizedDescriptionKey: description,
                                       NSLocalizedFailureReasonErrorKey: reason};
            statusError = [NSError errorWithDomain:MGLErrorDomain code:1 userInfo:userInfo];
        }
        if (completionHandler) {
            error = error ?: statusError;
            completionHandler(error);
        }
        dataTask = nil;
    }];
    [dataTask resume];
}

- (void)postEvent:(nonnull MGLMapboxEventAttributes *)event completionHandler:(nullable void (^)(NSError * _Nullable error))completionHandler {
    [self postEvents:@[event] completionHandler:completionHandler];
}

#pragma mark Utilities

- (NSURLRequest *)requestForEvents:(NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events {
    NSString *path = [NSString stringWithFormat:@"%@?access_token=%@", MGLAPIClientEventsPath, [MGLAccountManager accessToken]];
    NSURL *url = [NSURL URLWithString:path relativeToURL:self.baseURL];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
    [request setValue:self.userAgent forHTTPHeaderField:MGLAPIClientHeaderFieldUserAgentKey];
    [request setValue:MGLAPIClientHeaderFieldContentTypeValue forHTTPHeaderField:MGLAPIClientHeaderFieldContentTypeKey];
    [request setHTTPMethod:MGLAPIClientHTTPMethodPost];
    
    NSData *jsonData = [self serializedDataForEvents:events];
    
    // Compressing less than 3 events can have a negative impact on the size.
    if (events.count > 2) {
        NSData *compressedData = [jsonData mgl_compressedData];
        [request setValue:@"deflate" forHTTPHeaderField:MGLAPIClientHeaderFieldContentEncodingKey];
        [request setHTTPBody:compressedData];
    }
    
    // Set JSON data if events.count were less than 3 or something went wrong with compressing HTTP body data.
    if (!request.HTTPBody) {
        [request setValue:nil forHTTPHeaderField:MGLAPIClientHeaderFieldContentEncodingKey];
        [request setHTTPBody:jsonData];
    }
    
    return [request copy];
}

- (void)setupBaseURL {
    NSString *testServerURLString = [[NSUserDefaults standardUserDefaults] stringForKey:@"MGLTelemetryTestServerURL"];
    NSURL *testServerURL = [NSURL URLWithString:testServerURLString];
    if (testServerURL && [testServerURL.scheme isEqualToString:@"https"]) {
        self.baseURL = testServerURL;
        self.usesTestServer = YES;
    } else {
        self.baseURL = [NSURL URLWithString:MGLAPIClientBaseURL];
    }
}

- (void)loadCertificates {
    NSData *certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_com-geotrust"];
    self.geoTrustCert = certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_com-digicert"];
    self.digicertCert = certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_staging"];
    self.testServerCert = certificate;
}

- (void)loadCertificate:(NSData **)certificate withResource:(NSString *)resource {
    NSBundle *frameworkBundle = [NSBundle mgl_frameworkBundle];
    NSString *cerPath = [frameworkBundle pathForResource:resource ofType:@"der" inDirectory:frameworkBundle.mgl_resourcesDirectory];
    if (cerPath != nil) {
        *certificate = [NSData dataWithContentsOfFile:cerPath];
    }
}

- (void)setupUserAgent {
    NSString *appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"];
    NSString *appVersion = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
    NSString *appBuildNumber = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];
    NSString *semanticVersion = [NSBundle mgl_frameworkInfoDictionary][@"MGLSemanticVersionString"];
    NSString *shortVersion = [NSBundle mgl_frameworkInfoDictionary][@"CFBundleShortVersionString"];
    NSString *sdkVersion = semanticVersion ?: shortVersion;
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
