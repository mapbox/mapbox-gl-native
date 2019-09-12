#import "MGLTestAssertionHandler.h"

@implementation MGLTestAssertionHandler

- (instancetype)initWithTestCase:(XCTestCase *)testCase {
    if ((self = [super init])) {
        _testCase = testCase;
    }
    return self;
}

- (void)handleFailureInMethod:(SEL)selector
                       object:(id)object
                         file:(NSString *)fileName
                   lineNumber:(NSInteger)line
                  description:(NSString *)format, ...
{
    va_list args;
    va_start(args, format);
    NSString *description = [[NSString alloc] initWithFormat:format arguments:args];
    va_end(args);
    
    NSString *condition = [NSString stringWithFormat:
                           @"`[%@ %@]`",
                           object, NSStringFromSelector(selector)
                           ];

    if (self.testCase && self.shouldFail) {
        _XCTPreformattedFailureHandler(self.testCase,
                                       YES,
                                       fileName,
                                       line,
                                       condition,
                                       description
                                       );
    }
    else {
        NSLog(@"Assertion Failure: %@:%lu: %@ - %@",
              fileName,
              line,
              condition,
              description);
    }
}
 
- (void)handleFailureInFunction:(NSString *)functionName
                           file:(NSString *)fileName
                     lineNumber:(NSInteger)line
                    description:(NSString *)format, ...
{
    va_list args;
    va_start(args, format);
    NSString *description = [[NSString alloc] initWithFormat:format arguments:args];
    va_end(args);

    NSString *condition = [NSString stringWithFormat:
                           @"`%@`",
                           functionName];

    if (self.testCase && self.shouldFail) {
        _XCTPreformattedFailureHandler(self.testCase,
                                       YES,
                                       fileName,
                                       line,
                                       condition,
                                       description);
    }
    else {
        NSLog(@"Assertion Failure: %@:%lu: %@ - %@",
              fileName,
              line,
              condition,
              description);
    }
}
@end

