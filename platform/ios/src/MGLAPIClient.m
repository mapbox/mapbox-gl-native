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
@property (nonatomic, copy) NSData *digicertCert_2016;
@property (nonatomic, copy) NSData *geoTrustCert_2016;
@property (nonatomic, copy) NSData *digicertCert_2017;
@property (nonatomic, copy) NSData *geoTrustCert_2017;
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
    [self loadCertificate:&certificate withResource:@"api_mapbox_com-geotrust_2016"];
    self.geoTrustCert_2016 = certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_com-digicert_2016"];
    self.digicertCert_2016 = certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_com-geotrust_2017"];
    self.geoTrustCert_2017 = certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_com-digicert_2017"];
    self.digicertCert_2017 = certificate;
    [self loadCertificate:&certificate withResource:@"api_mapbox_staging"];
    self.testServerCert = certificate;
}

- (void)loadCertificate:(NSData **)certificate withResource:(NSString *)resource {
    NSBundle *frameworkBundle = [NSBundle mgl_frameworkBundle];
    NSString *cerPath = [frameworkBundle pathForResource:resource ofType:@"der"];
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

- (BOOL)evaluateCertificateWithCertificateData:(NSData *)certificateData keyCount:(CFIndex)keyCount serverTrust:(SecTrustRef)serverTrust challenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^) (NSURLSessionAuthChallengeDisposition disposition, NSURLCredential *credential))completionHandler {
    for (int lc = 0; lc < keyCount; lc++) {
        SecCertificateRef certificate = SecTrustGetCertificateAtIndex(serverTrust, lc);
        NSData *remoteCertificateData = CFBridgingRelease(SecCertificateCopyData(certificate));
        if ([remoteCertificateData isEqualToData:certificateData]) {
            completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
            return YES;
        }
    }
    return NO;
}

- (void)URLSession:(NSURLSession *)session didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^) (NSURLSessionAuthChallengeDisposition disposition, NSURLCredential *credential))completionHandler {
    
    if([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust]) {
        SecTrustRef serverTrust = [[challenge protectionSpace] serverTrust];
        SecTrustResultType trustResult;
        
        // Validate the certificate chain with the device's trust store anyway this *might* use revocation checking
        SecTrustEvaluate(serverTrust, &trustResult);
        
        BOOL found = NO; // For clarity; we start in a state where the challange has not been completed and no certificate has been found
        
        if (trustResult == kSecTrustResultUnspecified) {
            // Look for a pinned certificate in the server's certificate chain
            CFIndex numKeys = SecTrustGetCertificateCount(serverTrust);
            
            // Check certs in the following order: digicert 2016, digicert 2017, geotrust 2016, geotrust 2017
            found = [self evaluateCertificateWithCertificateData:self.digicertCert_2016 keyCount:numKeys serverTrust:serverTrust challenge:challenge completionHandler:completionHandler];
            if (!found) {
                found = [self evaluateCertificateWithCertificateData:self.digicertCert_2017 keyCount:numKeys serverTrust:serverTrust challenge:challenge completionHandler:completionHandler];
            }
            if (!found) {
                found = [self evaluateCertificateWithCertificateData:self.geoTrustCert_2016 keyCount:numKeys serverTrust:serverTrust challenge:challenge completionHandler:completionHandler];
            }
            if (!found) {
                found = [self evaluateCertificateWithCertificateData:self.geoTrustCert_2017 keyCount:numKeys serverTrust:serverTrust challenge:challenge completionHandler:completionHandler];
            }
            
            // If challenge can't be completed with any of the above certs, then try the test server if the app is configured to use the test server
            if (!found && _usesTestServer) {
                found = [self evaluateCertificateWithCertificateData:self.testServerCert keyCount:numKeys serverTrust:serverTrust challenge:challenge completionHandler:completionHandler];
            }
        }
        
        if (!found) {
            // No certificate was found so cancel the connection.
            completionHandler(NSURLSessionAuthChallengeCancelAuthenticationChallenge, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
        }
    }
}

@end
