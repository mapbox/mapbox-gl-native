#import "MGLOpenGLLayer.h"

#import "MGLMapView_Private.h"

@implementation MGLOpenGLLayer {
    NSOpenGLContext *_context;
}

- (MGLMapView *)mapView {
    return (MGLMapView *)super.view;
}

// - (BOOL)isAsynchronous {
//    return YES;
// }

- (BOOL)needsDisplayOnBoundsChange {
    return YES;
}

- (CGRect)frame {
    return self.view.bounds;
}

- (NSOpenGLContext *)openGLContextForPixelFormat:(NSOpenGLPixelFormat *)pixelFormat {
    if (!_context) {
        _context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    }
    return _context;
}

- (NSOpenGLPixelFormat *)openGLPixelFormatForDisplayMask:(uint32_t)mask {
    NSOpenGLPixelFormatAttribute pfas[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFAClosestPolicy,
        NSOpenGLPFAAccumSize, 32,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADepthSize, 16,
        NSOpenGLPFAStencilSize, 8,
        NSOpenGLPFAScreenMask, mask,
        NSOpenGLPFAAllowOfflineRenderers, // Allows using the integrated GPU
        0
    };
    return [[NSOpenGLPixelFormat alloc] initWithAttributes:pfas];
}

- (BOOL)canDrawInOpenGLContext:(__unused NSOpenGLContext *)context pixelFormat:(__unused NSOpenGLPixelFormat *)pixelFormat forLayerTime:(__unused CFTimeInterval)t displayTime:(__unused const CVTimeStamp *)ts {
    return !self.mapView.dormant;
}

- (void)drawInOpenGLContext:(NSOpenGLContext *)context pixelFormat:(NSOpenGLPixelFormat *)pixelFormat forLayerTime:(CFTimeInterval)t displayTime:(const CVTimeStamp *)ts {
    [self.mapView renderSync];
    [super drawInOpenGLContext:context pixelFormat:pixelFormat forLayerTime:t displayTime:ts];
}

@end
