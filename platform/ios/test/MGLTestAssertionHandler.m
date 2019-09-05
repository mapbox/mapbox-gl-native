#import "MGLTestAssertionHandler.h"

@implementation MGLTestAssertionHandler

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
    
    NSLog(@"NSAssert failure: Method `%@` (%@:%lu) for %@: %@",
          NSStringFromSelector(selector),
          fileName,
          line,
          object,
          description);
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

    NSLog(@"NSCAssert failure: Function `%@` (%@:%lu): %@",
          functionName,
          fileName,
          line,
          description);
}
@end

