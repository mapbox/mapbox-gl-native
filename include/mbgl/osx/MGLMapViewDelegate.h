#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;
@class MGLAnnotationImage;
@class MGLPolygon;
@class MGLPolyline;
@class MGLShape;

@protocol MGLMapViewDelegate <NSObject>

@optional

- (void)mapView:(MGLMapView *)mapView regionWillChangeAnimated:(BOOL)animated;
- (void)mapViewRegionIsChanging:(MGLMapView *)mapView;
- (void)mapView:(MGLMapView *)mapView regionDidChangeAnimated:(BOOL)animated;

- (void)mapViewWillStartLoadingMap:(MGLMapView *)mapView;
- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView;

- (void)mapViewWillStartRenderingMap:(MGLMapView *)mapView;
- (void)mapViewDidFinishRenderingMap:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered;
- (void)mapViewWillStartRenderingFrame:(MGLMapView *)mapView;
- (void)mapViewDidFinishRenderingFrame:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered;

- (nullable MGLAnnotationImage *)mapView:(MGLMapView *)mapView imageForAnnotation:(id <MGLAnnotation>)annotation;
- (CGFloat)mapView:(MGLMapView *)mapView alphaForShapeAnnotation:(MGLShape *)annotation;
- (NSColor *)mapView:(MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation;
- (NSColor *)mapView:(MGLMapView *)mapView fillColorForPolygonAnnotation:(MGLPolygon *)annotation;
- (CGFloat)mapView:(MGLMapView *)mapView lineWidthForPolylineAnnotation:(MGLPolyline *)annotation;

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation;
- (nullable NSViewController *)mapView:(MGLMapView *)mapView calloutViewControllerForAnnotation:(id <MGLAnnotation>)annotation;
- (void)mapView:(MGLMapView *)mapView didSelectAnnotation:(id <MGLAnnotation>)annotation;
- (void)mapView:(MGLMapView *)mapView didDeselectAnnotation:(id <MGLAnnotation>)annotation;

@end

NS_ASSUME_NONNULL_END
