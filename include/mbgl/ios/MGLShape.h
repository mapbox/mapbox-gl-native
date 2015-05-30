#import <Foundation/Foundation.h>

#import "MGLAnnotation.h"

@interface MGLShape : NSObject <MGLAnnotation>

@property (nonatomic, copy) NSString *title;
@property (nonatomic, copy) NSString *subtitle;

@end
