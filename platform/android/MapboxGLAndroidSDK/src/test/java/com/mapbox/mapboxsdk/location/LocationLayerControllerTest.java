package com.mapbox.mapboxsdk.location;

import android.graphics.Bitmap;

import com.google.gson.JsonElement;
import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;

import org.junit.Test;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.BACKGROUND_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.BACKGROUND_LAYER;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.BACKGROUND_STALE_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.BEARING_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.BEARING_LAYER;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.FOREGROUND_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.FOREGROUND_LAYER;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.FOREGROUND_STALE_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.LOCATION_SOURCE;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_ACCURACY_RADIUS;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_COMPASS_BEARING;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_FOREGROUND_ICON_OFFSET;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_GPS_BEARING;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_SHADOW_ICON_OFFSET;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.SHADOW_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.SHADOW_LAYER;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class LocationLayerControllerTest {

  @Test
  public void onInitialization_locationSourceIsAdded() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addSource(locationSource);
  }

  @Test
  public void onInitialization_shadowLayerIsAdded() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer shadowLayer = mock(Layer.class);
    when(sourceProvider.generateLayer(SHADOW_LAYER)).thenReturn(shadowLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addLayerBelow(shadowLayer, BACKGROUND_LAYER);
  }

  @Test
  public void onInitialization_backgroundLayerIsAdded() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer backgroundLayer = mock(Layer.class);
    when(sourceProvider.generateLayer(BACKGROUND_LAYER)).thenReturn(backgroundLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addLayerBelow(backgroundLayer, FOREGROUND_LAYER);
  }

  @Test
  public void onInitialization_foregroundLayerIsAdded() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer foregroundLayer = mock(Layer.class);
    when(sourceProvider.generateLayer(FOREGROUND_LAYER)).thenReturn(foregroundLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addLayerBelow(foregroundLayer, BEARING_LAYER);
  }

  @Test
  public void onInitialization_bearingLayerIsAdded() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer bearingLayer = mock(Layer.class);
    when(sourceProvider.generateLayer(BEARING_LAYER)).thenReturn(bearingLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    String layerBelow = "layer-below";
    when(options.layerBelow()).thenReturn(layerBelow);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addLayerBelow(bearingLayer, layerBelow);
  }

  @Test
  public void onInitialization_accuracyLayerIsAdded() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer accuracyLayer = mock(Layer.class);
    when(sourceProvider.generateAccuracyLayer()).thenReturn(accuracyLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addLayerBelow(accuracyLayer, BACKGROUND_LAYER);
  }

  @Test
  public void applyStyle_styleShadowWithValidElevation() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateShadowBitmap(any(LocationComponentOptions.class))).thenReturn(bitmap);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    when(options.elevation()).thenReturn(2f);

    // Style is applied on initialization
    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addImage(SHADOW_ICON, bitmap);
  }

  @Test
  public void applyStyle_ignoreStyleShadowWithInvalidElevation() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateShadowBitmap(any(LocationComponentOptions.class))).thenReturn(bitmap);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    when(options.elevation()).thenReturn(0f);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap, times(0)).addImage(SHADOW_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleForegroundFromOptions() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    int drawableResId = 123;
    int tintColor = 456;
    when(options.foregroundDrawable()).thenReturn(drawableResId);
    when(options.foregroundTintColor()).thenReturn(tintColor);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateBitmap(drawableResId, tintColor)).thenReturn(bitmap);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addImage(FOREGROUND_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleForegroundStaleFromOptions() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    int drawableResId = 123;
    int tintColor = 456;
    when(options.foregroundDrawableStale()).thenReturn(drawableResId);
    when(options.foregroundStaleTintColor()).thenReturn(tintColor);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateBitmap(drawableResId, tintColor)).thenReturn(bitmap);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addImage(FOREGROUND_STALE_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleBackgroundFromOptions() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    int drawableResId = 123;
    int tintColor = 456;
    when(options.backgroundDrawable()).thenReturn(drawableResId);
    when(options.backgroundTintColor()).thenReturn(tintColor);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateBitmap(drawableResId, tintColor)).thenReturn(bitmap);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addImage(BACKGROUND_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleBackgroundStaleFromOptions() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    int drawableResId = 123;
    int tintColor = 456;
    when(options.backgroundDrawableStale()).thenReturn(drawableResId);
    when(options.backgroundStaleTintColor()).thenReturn(tintColor);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateBitmap(drawableResId, tintColor)).thenReturn(bitmap);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addImage(BACKGROUND_STALE_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleBearingFromOptions() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    int drawableResId = 123;
    int tintColor = 456;
    when(options.bearingDrawable()).thenReturn(drawableResId);
    when(options.bearingTintColor()).thenReturn(tintColor);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateBitmap(drawableResId, tintColor)).thenReturn(bitmap);

    new LocationLayerController(mapboxMap, sourceProvider, buildFeatureProvider(options), bitmapProvider, options);

    verify(mapboxMap).addImage(BEARING_ICON, bitmap);
  }

  @Test
  public void updateForegroundOffset_foregroundIconPropertyIsUpdated() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );

    layer.updateForegroundOffset(2d);

    verify(locationFeature).addProperty(eq(PROPERTY_FOREGROUND_ICON_OFFSET), any(JsonElement.class));
  }

  @Test
  public void updateForegroundOffset_shadowPropertyIsUpdated() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );

    layer.updateForegroundOffset(2d);

    verify(locationFeature).addProperty(eq(PROPERTY_SHADOW_ICON_OFFSET), any(JsonElement.class));
  }

  @Test
  public void onNewLatLngValue_locationFeatureIsUpdated() {
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );

    layer.onNewLatLngValue(new LatLng());

    // wanted twice (once for initialization)
    verify(locationSource, times(2)).setGeoJson(locationFeature);
  }

  @Test
  public void onNewGpsBearingValue_locationFeatureIsUpdated() {
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );
    layer.setRenderMode(RenderMode.GPS);
    float gpsBearing = 2f;

    layer.onNewGpsBearingValue(gpsBearing);

    verify(locationFeature).addNumberProperty(PROPERTY_GPS_BEARING, gpsBearing);
  }

  @Test
  public void onNewGpsBearingValue_updateIgnoredWithInvalidRenderMode() {
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );
    layer.setRenderMode(RenderMode.COMPASS);
    float gpsBearing = 2f;

    layer.onNewGpsBearingValue(gpsBearing);

    verify(locationFeature, times(0)).addNumberProperty(PROPERTY_GPS_BEARING, gpsBearing);
  }

  @Test
  public void onNewCompassBearingValue_locationFeatureIsUpdated() {
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );
    layer.setRenderMode(RenderMode.COMPASS);
    float compassBearing = 2f;

    layer.onNewCompassBearingValue(compassBearing);

    verify(locationFeature).addNumberProperty(PROPERTY_COMPASS_BEARING, compassBearing);
  }

  @Test
  public void onNewCompassBearingValue_updateIgnoredWithInvalidRenderMode() {
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );
    layer.setRenderMode(RenderMode.GPS);
    float compassBearing = 2f;

    layer.onNewCompassBearingValue(compassBearing);

    verify(locationFeature, times(0)).addNumberProperty(PROPERTY_COMPASS_BEARING, compassBearing);
  }

  @Test
  public void onNewAccuracyRadiusValue_locationFeatureIsUpdated() {
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );
    float accuracyRadiusValue = 2f;

    layer.onNewAccuracyRadiusValue(accuracyRadiusValue);

    verify(locationFeature).addNumberProperty(PROPERTY_ACCURACY_RADIUS, accuracyRadiusValue);
  }

  @Test
  public void onNewAccuracyRadiusValue_updateIgnoredWithInvalidRenderMode() {
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, sourceProvider, buildFeatureProvider(locationFeature, options), bitmapProvider, options
    );
    layer.setRenderMode(RenderMode.GPS);
    float accuracyRadiusValue = 2f;

    layer.onNewAccuracyRadiusValue(accuracyRadiusValue);

    verify(locationFeature, times(0)).addNumberProperty(PROPERTY_ACCURACY_RADIUS, accuracyRadiusValue);
  }

  private LayerFeatureProvider buildFeatureProvider(LocationComponentOptions options) {
    LayerFeatureProvider provider = mock(LayerFeatureProvider.class);
    when(provider.generateLocationFeature(null, options)).thenReturn(mock(Feature.class));
    return provider;
  }

  private LayerFeatureProvider buildFeatureProvider(Feature feature, LocationComponentOptions options) {
    LayerFeatureProvider provider = mock(LayerFeatureProvider.class);
    when(provider.generateLocationFeature(null, options)).thenReturn(feature);
    return provider;
  }

  private LayerSourceProvider buildLayerProvider() {
    LayerSourceProvider layerSourceProvider = mock(LayerSourceProvider.class);
    when(layerSourceProvider.generateLayer(SHADOW_LAYER)).thenReturn(mock(Layer.class));
    when(layerSourceProvider.generateLayer(BACKGROUND_LAYER)).thenReturn(mock(Layer.class));
    when(layerSourceProvider.generateLayer(FOREGROUND_LAYER)).thenReturn(mock(Layer.class));
    when(layerSourceProvider.generateLayer(BEARING_LAYER)).thenReturn(mock(Layer.class));
    when(layerSourceProvider.generateAccuracyLayer()).thenReturn(mock(Layer.class));
    return layerSourceProvider;
  }
}