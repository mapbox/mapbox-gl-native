#import <Foundation/Foundation.h>
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, MGLTileSetScheme) {
    MGLTileSetSchemeXYZ = 0,
    MGLTileSetSchemeTMS
};

@interface MGLTileSet : NSObject

@property (nonatomic, copy) NS_ARRAY_OF(NSString *) *tileURLTemplates;
@property (nonatomic) NSUInteger minimumZoomLevel;
@property (nonatomic) NSUInteger maximumZoomLevel;
@property (nonatomic, copy) NSString *attribution;
@property (nonatomic) MGLTileSetScheme scheme;

- (instancetype)initWithTileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates;
- (instancetype)initWithTileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel;

@end

NS_ASSUME_NONNULL_END
