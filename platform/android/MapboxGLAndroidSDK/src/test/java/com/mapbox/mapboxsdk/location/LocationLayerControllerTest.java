package com.mapbox.mapboxsdk.location;

import android.graphics.Bitmap;
import android.support.annotation.NonNull;

import com.google.gson.JsonElement;
import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mockito;

import java.util.Set;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.ACCURACY_LAYER;
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
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_LAYER_ACCURACY;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_LAYER_COMPASS_BEARING;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_LAYER_GPS_BEARING;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_LAYER_LATLNG;
import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertNull;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class LocationLayerControllerTest {

  private MapboxMap mapboxMap = mock(MapboxMap.class);
  private Style style = mock(Style.class);

  @Before
  public void before() {
    when(mapboxMap.getStyle()).thenReturn(style);
  }

  @Test
  public void onInitialization_locationSourceIsAdded() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addSource(locationSource);
  }

  @Test
  public void onInitialization_shadowLayerIsAdded() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer shadowLayer = mock(Layer.class);
    when(sourceProvider.generateLayer(SHADOW_LAYER)).thenReturn(shadowLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addLayerBelow(shadowLayer, BACKGROUND_LAYER);
  }

  @Test
  public void onInitialization_backgroundLayerIsAdded() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer backgroundLayer = mock(Layer.class);
    when(sourceProvider.generateLayer(BACKGROUND_LAYER)).thenReturn(backgroundLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addLayerBelow(backgroundLayer, FOREGROUND_LAYER);
  }

  @Test
  public void onInitialization_foregroundLayerIsAdded() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer foregroundLayer = mock(Layer.class);
    when(sourceProvider.generateLayer(FOREGROUND_LAYER)).thenReturn(foregroundLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addLayerBelow(foregroundLayer, BEARING_LAYER);
  }

  @Test
  public void onInitialization_bearingLayerIsAdded() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer bearingLayer = mock(Layer.class);
    when(sourceProvider.generateLayer(BEARING_LAYER)).thenReturn(bearingLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    String layerBelow = "layer-below";
    when(options.layerBelow()).thenReturn(layerBelow);

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addLayerBelow(bearingLayer, layerBelow);
  }

  @Test
  public void onInitialization_accuracyLayerIsAdded() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    Layer accuracyLayer = mock(Layer.class);
    when(sourceProvider.generateAccuracyLayer()).thenReturn(accuracyLayer);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addLayerBelow(accuracyLayer, BACKGROUND_LAYER);
  }

  @Test
  public void onInitialization_numberOfCachedLayerIdsIsConstant() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    LocationLayerController controller =
      new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
        bitmapProvider, options, internalRenderModeChangedListener);

    controller.initializeComponents(mapboxMap.getStyle(), options);

    assertEquals(5, controller.layerSet.size());
  }

  @Test
  public void applyStyle_styleShadowWithValidElevation() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateShadowBitmap(any(LocationComponentOptions.class))).thenReturn(bitmap);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    when(options.elevation()).thenReturn(2f);

    // Style is applied on initialization
    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addImage(SHADOW_ICON, bitmap);
  }

  @Test
  public void applyStyle_ignoreStyleShadowWithInvalidElevation() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateShadowBitmap(any(LocationComponentOptions.class))).thenReturn(bitmap);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    when(options.elevation()).thenReturn(0f);

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style, times(0)).addImage(SHADOW_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleForegroundFromOptions() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
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

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addImage(FOREGROUND_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleForegroundStaleFromOptions() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
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

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addImage(FOREGROUND_STALE_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleBackgroundFromOptions() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
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

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addImage(BACKGROUND_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleBackgroundStaleFromOptions() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
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

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addImage(BACKGROUND_STALE_ICON, bitmap);
  }

  @Test
  public void applyStyle_styleBearingFromOptions() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
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

    new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addImage(BEARING_ICON, bitmap);
  }

  @Test
  public void applyStyle_layerBelowChanged() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));

    LocationComponentOptions options = mock(LocationComponentOptions.class);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateShadowBitmap(any(LocationComponentOptions.class))).thenReturn(bitmap);

    LocationLayerController layerController =
      new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
        bitmapProvider, options, internalRenderModeChangedListener);

    Layer bearingLayer2 = mock(Layer.class);
    when(sourceProvider.generateLayer(BEARING_LAYER)).thenReturn(bearingLayer2);
    Layer foregroundLayer2 = mock(Layer.class);
    when(sourceProvider.generateLayer(FOREGROUND_LAYER)).thenReturn(foregroundLayer2);
    Layer backgroundLayer2 = mock(Layer.class);
    when(sourceProvider.generateLayer(BACKGROUND_LAYER)).thenReturn(backgroundLayer2);
    Layer shadowLayer2 = mock(Layer.class);
    when(sourceProvider.generateLayer(SHADOW_LAYER)).thenReturn(shadowLayer2);
    Layer accuracyLayer2 = mock(Layer.class);
    when(sourceProvider.generateAccuracyLayer()).thenReturn(accuracyLayer2);
    String layerBelow = "layer-below";
    when(options.layerBelow()).thenReturn(layerBelow);

    layerController.applyStyle(options);

    verify(style).removeLayer(BEARING_LAYER);
    verify(style).removeLayer(FOREGROUND_LAYER);
    verify(style).removeLayer(BACKGROUND_LAYER);
    verify(style).removeLayer(SHADOW_LAYER);
    verify(style).removeLayer(ACCURACY_LAYER);

    verify(style).addLayerBelow(bearingLayer2, layerBelow);
    verify(style).addLayerBelow(foregroundLayer2, BEARING_LAYER);
    verify(style).addLayerBelow(backgroundLayer2, FOREGROUND_LAYER);
    verify(style).addLayerBelow(shadowLayer2, BACKGROUND_LAYER);
    verify(style).addLayerBelow(accuracyLayer2, BACKGROUND_LAYER);
  }

  @Test
  public void applyStyle_layerBelowNotChanged() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateShadowBitmap(any(LocationComponentOptions.class))).thenReturn(bitmap);

    String layerBelow = "layer-below";
    when(options.layerBelow()).thenReturn(layerBelow);

    LocationLayerController layerController =
      new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
        bitmapProvider, options, internalRenderModeChangedListener);

    options = mock(LocationComponentOptions.class);
    layerBelow = "layer-below";
    when(options.layerBelow()).thenReturn(layerBelow);
    layerController.applyStyle(options);

    verify(style, times(0)).removeLayer(any(String.class));
    verify(style, times(5)).addLayerBelow(any(Layer.class), any(String.class));
  }

  @Test
  public void applyStyle_layerBelowNotChangedNull() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(mock(GeoJsonSource.class));
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    Bitmap bitmap = mock(Bitmap.class);
    when(bitmapProvider.generateShadowBitmap(any(LocationComponentOptions.class))).thenReturn(bitmap);

    LocationLayerController layerController =
      new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
        bitmapProvider, options, internalRenderModeChangedListener);

    options = mock(LocationComponentOptions.class);
    layerController.applyStyle(options);

    verify(style, times(0)).removeLayer(any(String.class));
    verify(style, times(1)).addLayer(any(Layer.class));
    verify(style, times(4)).addLayerBelow(any(Layer.class), Mockito.<String>any());
  }

  @Test
  public void updateForegroundOffset_foregroundIconPropertyIsUpdated() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );

    layer.updateForegroundOffset(2d);

    verify(locationFeature).addProperty(eq(PROPERTY_FOREGROUND_ICON_OFFSET), any(JsonElement.class));
  }

  @Test
  public void updateForegroundOffset_shadowPropertyIsUpdated() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );
    layer.updateForegroundOffset(2d);

    verify(locationFeature).addProperty(eq(PROPERTY_SHADOW_ICON_OFFSET), any(JsonElement.class));
  }

  @Test
  public void onNewLatLngValue_locationFeatureIsUpdated() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(style.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );

    getAnimationListener(ANIMATOR_LAYER_LATLNG, layer.getAnimationListeners()).onNewAnimationValue(new LatLng());

    // wanted twice (once for initialization)
    verify(locationSource, times(2)).setGeoJson(locationFeature);
  }

  @Test
  public void onNewGpsBearingValue_locationFeatureIsUpdated() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(style.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );
    layer.setRenderMode(RenderMode.GPS);
    float gpsBearing = 2f;

    getAnimationListener(ANIMATOR_LAYER_GPS_BEARING, layer.getAnimationListeners()).onNewAnimationValue(gpsBearing);

    verify(locationFeature).addNumberProperty(PROPERTY_GPS_BEARING, gpsBearing);
  }

  @Test
  public void onNewGpsBearingValue_updateIgnoredWithInvalidRenderMode() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(style.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );
    layer.setRenderMode(RenderMode.COMPASS);
    float gpsBearing = 2f;

    assertNull(getAnimationListener(ANIMATOR_LAYER_GPS_BEARING, layer.getAnimationListeners()));
    verify(locationFeature, times(0)).addNumberProperty(PROPERTY_GPS_BEARING, gpsBearing);
  }

  @Test
  public void onNewCompassBearingValue_locationFeatureIsUpdated() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(style.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );
    layer.setRenderMode(RenderMode.COMPASS);
    float compassBearing = 2f;

    getAnimationListener(ANIMATOR_LAYER_COMPASS_BEARING, layer.getAnimationListeners())
      .onNewAnimationValue(compassBearing);

    verify(locationFeature).addNumberProperty(PROPERTY_COMPASS_BEARING, compassBearing);
  }

  @Test
  public void onNewCompassBearingValue_updateIgnoredWithInvalidRenderMode() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(style.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );
    layer.setRenderMode(RenderMode.GPS);
    float compassBearing = 2f;

    assertNull(getAnimationListener(ANIMATOR_LAYER_COMPASS_BEARING, layer.getAnimationListeners()));
    verify(locationFeature, times(0)).addNumberProperty(PROPERTY_COMPASS_BEARING, compassBearing);
  }

  @Test
  public void onNewAccuracyRadiusValue_locationFeatureIsUpdated() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(style.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );
    layer.setRenderMode(RenderMode.NORMAL);
    float accuracyRadiusValue = 2f;

    getAnimationListener(ANIMATOR_LAYER_ACCURACY, layer.getAnimationListeners())
      .onNewAnimationValue(accuracyRadiusValue);

    verify(locationFeature).addNumberProperty(PROPERTY_ACCURACY_RADIUS, accuracyRadiusValue);
  }

  @Test
  public void onNewAccuracyRadiusValue_updateIgnoredWithInvalidRenderMode() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(style.getSourceAs(LOCATION_SOURCE)).thenReturn(locationSource);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    Feature locationFeature = mock(Feature.class);
    LocationLayerController layer = new LocationLayerController(
      mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(locationFeature, options),
      bitmapProvider, options, internalRenderModeChangedListener
    );
    layer.setRenderMode(RenderMode.GPS);
    float accuracyRadiusValue = 2f;

    assertNull(getAnimationListener(ANIMATOR_LAYER_ACCURACY, layer.getAnimationListeners()));
    verify(locationFeature, times(0))
      .addNumberProperty(PROPERTY_ACCURACY_RADIUS, accuracyRadiusValue);
  }

  @Test
  public void renderModeChanged_doNotNotifyAboutDuplicates_NORMAL() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    LocationLayerController controller =
      new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    controller.setRenderMode(RenderMode.NORMAL);
    controller.setRenderMode(RenderMode.NORMAL);

    verify(internalRenderModeChangedListener, times(1)).onRenderModeChanged(RenderMode.NORMAL);
  }

  @Test
  public void renderModeChanged_doNotNotifyAboutDuplicates_GPS() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
    LayerSourceProvider sourceProvider = buildLayerProvider();
    GeoJsonSource locationSource = mock(GeoJsonSource.class);
    when(sourceProvider.generateSource(any(Feature.class))).thenReturn(locationSource);
    LayerBitmapProvider bitmapProvider = mock(LayerBitmapProvider.class);
    LocationComponentOptions options = mock(LocationComponentOptions.class);

    LocationLayerController controller =
      new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    controller.setRenderMode(RenderMode.GPS);
    controller.setRenderMode(RenderMode.GPS);

    verify(internalRenderModeChangedListener, times(1)).onRenderModeChanged(RenderMode.GPS);
  }

  @Test
  public void layerHidden_renderModeChanged_layerShown_foregroundIconUpdated() {
    OnRenderModeChangedListener internalRenderModeChangedListener = mock(OnRenderModeChangedListener.class);
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

    LocationLayerController controller =
      new LocationLayerController(mapboxMap, mapboxMap.getStyle(), sourceProvider, buildFeatureProvider(options),
      bitmapProvider, options, internalRenderModeChangedListener);

    verify(style).addImage(FOREGROUND_ICON, bitmap);

    int drawableGpsResId = 789;
    when(options.gpsDrawable()).thenReturn(drawableGpsResId);

    Bitmap bitmapGps = mock(Bitmap.class);
    when(bitmapProvider.generateBitmap(drawableGpsResId, tintColor)).thenReturn(bitmapGps);

    controller.hide();

    controller.setRenderMode(RenderMode.GPS);

    controller.show();

    verify(style).addImage(FOREGROUND_ICON, bitmapGps);
  }

  private LayerFeatureProvider buildFeatureProvider(@NonNull LocationComponentOptions options) {
    LayerFeatureProvider provider = mock(LayerFeatureProvider.class);
    when(provider.generateLocationFeature(null, options)).thenReturn(mock(Feature.class));
    return provider;
  }

  private LayerFeatureProvider buildFeatureProvider(Feature feature, @NonNull LocationComponentOptions options) {
    LayerFeatureProvider provider = mock(LayerFeatureProvider.class);
    when(provider.generateLocationFeature(null, options)).thenReturn(feature);
    return provider;
  }

  private LayerSourceProvider buildLayerProvider() {
    LayerSourceProvider layerSourceProvider = mock(LayerSourceProvider.class);

    Layer shadowLayer = mock(Layer.class);
    when(shadowLayer.getId()).thenReturn(SHADOW_LAYER);
    when(layerSourceProvider.generateLayer(SHADOW_LAYER)).thenReturn(shadowLayer);

    Layer backgroundLayer = mock(Layer.class);
    when(backgroundLayer.getId()).thenReturn(BACKGROUND_LAYER);
    when(layerSourceProvider.generateLayer(BACKGROUND_LAYER)).thenReturn(backgroundLayer);

    Layer foregroundLayer = mock(Layer.class);
    when(foregroundLayer.getId()).thenReturn(FOREGROUND_LAYER);
    when(layerSourceProvider.generateLayer(FOREGROUND_LAYER)).thenReturn(foregroundLayer);

    Layer bearingLayer = mock(Layer.class);
    when(bearingLayer.getId()).thenReturn(BEARING_LAYER);
    when(layerSourceProvider.generateLayer(BEARING_LAYER)).thenReturn(bearingLayer);

    Layer accuracyLayer = mock(Layer.class);
    when(accuracyLayer.getId()).thenReturn(ACCURACY_LAYER);
    when(layerSourceProvider.generateAccuracyLayer()).thenReturn(accuracyLayer);
    return layerSourceProvider;
  }

  private <T> MapboxAnimator.AnimationsValueChangeListener<T> getAnimationListener(
    @MapboxAnimator.Type int animatorType,
    Set<AnimatorListenerHolder> holders) {
    for (AnimatorListenerHolder holder : holders) {
      @MapboxAnimator.Type int type = holder.getAnimatorType();
      if (type == animatorType) {
        return holder.getListener();
      }
    }
    return null;
  }
}