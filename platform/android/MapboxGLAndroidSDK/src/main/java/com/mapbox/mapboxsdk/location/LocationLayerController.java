package com.mapbox.mapboxsdk.location;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;

import java.util.ArrayList;
import java.util.List;

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
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_ACCURACY_ALPHA;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_ACCURACY_COLOR;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_ACCURACY_RADIUS;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_BACKGROUND_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_BACKGROUND_STALE_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_BEARING_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_COMPASS_BEARING;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_FOREGROUND_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_FOREGROUND_ICON_OFFSET;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_FOREGROUND_STALE_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_GPS_BEARING;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_LOCATION_STALE;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_SHADOW_ICON_OFFSET;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.SHADOW_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.SHADOW_LAYER;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.linear;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.zoom;
import static com.mapbox.mapboxsdk.style.layers.Property.NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.VISIBLE;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.colorToRgbaString;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;

final class LocationLayerController implements MapboxAnimator.OnLayerAnimationsValuesChangeListener {

  @RenderMode.Mode
  private int renderMode;

  private final MapboxMap mapboxMap;
  private final LayerSourceProvider layerSourceProvider;
  private final LayerBitmapProvider bitmapProvider;
  private LocationComponentOptions options;

  private final List<String> layerMap = new ArrayList<>();
  private Feature locationFeature;
  private GeoJsonSource locationSource;

  private boolean isHidden = true;

  LocationLayerController(MapboxMap mapboxMap, LayerSourceProvider layerSourceProvider,
                          LayerFeatureProvider featureProvider, LayerBitmapProvider bitmapProvider,
                          LocationComponentOptions options) {
    this.mapboxMap = mapboxMap;
    this.layerSourceProvider = layerSourceProvider;
    this.bitmapProvider = bitmapProvider;
    this.locationFeature = featureProvider.generateLocationFeature(locationFeature, options);
    initializeComponents(options);
    setRenderMode(RenderMode.NORMAL);
  }

  void initializeComponents(LocationComponentOptions options) {
    addLocationSource();
    addLayers(options.layerBelow());
    applyStyle(options);

    if (isHidden) {
      hide();
    } else {
      show();
    }
  }

  void applyStyle(@NonNull LocationComponentOptions options) {
    this.options = options;

    float elevation = options.elevation();
    // Only add icon elevation if the values greater than 0.
    if (elevation > 0) {
      styleShadow(options);
    }
    styleForeground(options);
    styleBackground(options);
    styleBearing(options);
    styleAccuracy(options.accuracyAlpha(), options.accuracyColor());
    styleScaling(options);
    determineIconsSource(options);
  }

  void setRenderMode(@RenderMode.Mode int renderMode) {
    this.renderMode = renderMode;

    if (!isHidden) {
      boolean isStale = locationFeature.getBooleanProperty(PROPERTY_LOCATION_STALE);
      switch (renderMode) {
        case RenderMode.NORMAL:
          styleForeground(options);
          setLayerVisibility(SHADOW_LAYER, true);
          setLayerVisibility(FOREGROUND_LAYER, true);
          setLayerVisibility(BACKGROUND_LAYER, true);
          setLayerVisibility(ACCURACY_LAYER, !isStale);
          setLayerVisibility(BEARING_LAYER, false);
          break;
        case RenderMode.COMPASS:
          styleForeground(options);
          setLayerVisibility(SHADOW_LAYER, true);
          setLayerVisibility(FOREGROUND_LAYER, true);
          setLayerVisibility(BACKGROUND_LAYER, true);
          setLayerVisibility(ACCURACY_LAYER, !isStale);
          setLayerVisibility(BEARING_LAYER, true);
          break;
        case RenderMode.GPS:
          styleForeground(options);
          setLayerVisibility(SHADOW_LAYER, false);
          setLayerVisibility(FOREGROUND_LAYER, true);
          setLayerVisibility(BACKGROUND_LAYER, true);
          setLayerVisibility(ACCURACY_LAYER, false);
          setLayerVisibility(BEARING_LAYER, false);
          break;
        default:
          break;
      }

      determineIconsSource(options);
    }
  }

  int getRenderMode() {
    return renderMode;
  }

  //
  // Layer action
  //

  void show() {
    isHidden = false;
    setRenderMode(renderMode);
  }

  void hide() {
    isHidden = true;
    for (String layerId : layerMap) {
      setLayerVisibility(layerId, false);
    }
  }

  void updateForegroundOffset(double tilt) {
    JsonArray foregroundJsonArray = new JsonArray();
    foregroundJsonArray.add(0f);
    foregroundJsonArray.add((float) (-0.05 * tilt));
    locationFeature.addProperty(PROPERTY_FOREGROUND_ICON_OFFSET, foregroundJsonArray);

    JsonArray backgroundJsonArray = new JsonArray();
    backgroundJsonArray.add(0f);
    backgroundJsonArray.add((float) (0.05 * tilt));
    locationFeature.addProperty(PROPERTY_SHADOW_ICON_OFFSET, backgroundJsonArray);

    refreshSource();
  }

  void updateForegroundBearing(float bearing) {
    if (renderMode != RenderMode.GPS) {
      setBearingProperty(PROPERTY_GPS_BEARING, bearing);
    }
  }

  boolean isHidden() {
    return isHidden;
  }

  private void setLayerVisibility(String layerId, boolean visible) {
    Layer layer = mapboxMap.getLayer(layerId);
    if (layer != null) {
      String targetVisibility = visible ? VISIBLE : NONE;
      if (!layer.getVisibility().value.equals(targetVisibility)) {
        layer.setProperties(visibility(visible ? VISIBLE : NONE));
      }
    }
  }

  private void addLayers(String idBelowLayer) {
    addSymbolLayer(BEARING_LAYER, idBelowLayer);
    addSymbolLayer(FOREGROUND_LAYER, BEARING_LAYER);
    addSymbolLayer(BACKGROUND_LAYER, FOREGROUND_LAYER);
    addSymbolLayer(SHADOW_LAYER, BACKGROUND_LAYER);
    addAccuracyLayer();
  }

  private void addSymbolLayer(String layerId, String beforeLayerId) {
    Layer layer = layerSourceProvider.generateLayer(layerId);
    addLayerToMap(layer, beforeLayerId);
  }

  private void addAccuracyLayer() {
    Layer accuracyLayer = layerSourceProvider.generateAccuracyLayer();
    addLayerToMap(accuracyLayer, BACKGROUND_LAYER);
  }

  private void addLayerToMap(Layer layer, @NonNull String idBelowLayer) {
    mapboxMap.addLayerBelow(layer, idBelowLayer);
    layerMap.add(layer.getId());
  }

  private void setBearingProperty(String propertyId, float bearing) {
    locationFeature.addNumberProperty(propertyId, bearing);
    refreshSource();
  }

  private void updateAccuracyRadius(float accuracy) {
    if (renderMode == RenderMode.COMPASS || renderMode == RenderMode.NORMAL) {
      locationFeature.addNumberProperty(PROPERTY_ACCURACY_RADIUS, accuracy);
      refreshSource();
    }
  }

  //
  // Source actions
  //

  private void addLocationSource() {
    locationSource = layerSourceProvider.generateSource(locationFeature);
    mapboxMap.addSource(locationSource);
  }

  private void refreshSource() {
    GeoJsonSource source = mapboxMap.getSourceAs(LOCATION_SOURCE);
    if (source != null) {
      locationSource.setGeoJson(locationFeature);
    }
  }

  private void setLocationPoint(Point locationPoint) {
    JsonObject properties = locationFeature.properties();
    if (properties != null) {
      locationFeature = Feature.fromGeometry(locationPoint, properties);
      refreshSource();
    }
  }

  //
  // Styling
  //

  private void styleBackground(LocationComponentOptions options) {
    Bitmap backgroundBitmap = bitmapProvider.generateBitmap(
      options.backgroundDrawable(), options.backgroundTintColor()
    );
    Bitmap backgroundStaleBitmap = bitmapProvider.generateBitmap(
      options.backgroundDrawableStale(), options.backgroundStaleTintColor()
    );
    mapboxMap.addImage(BACKGROUND_ICON, backgroundBitmap);
    mapboxMap.addImage(BACKGROUND_STALE_ICON, backgroundStaleBitmap);
  }

  private void styleShadow(LocationComponentOptions options) {
    mapboxMap.addImage(SHADOW_ICON, bitmapProvider.generateShadowBitmap(options));
  }

  private void styleBearing(LocationComponentOptions options) {
    Bitmap bearingBitmap = bitmapProvider.generateBitmap(options.bearingDrawable(), options.bearingTintColor());
    mapboxMap.addImage(BEARING_ICON, bearingBitmap);
  }

  private void styleAccuracy(float accuracyAlpha, @ColorInt int accuracyColor) {
    locationFeature.addNumberProperty(PROPERTY_ACCURACY_ALPHA, accuracyAlpha);
    locationFeature.addStringProperty(PROPERTY_ACCURACY_COLOR, colorToRgbaString(accuracyColor));
    refreshSource();
  }

  private void styleForeground(LocationComponentOptions options) {
    Bitmap foregroundBitmap = bitmapProvider.generateBitmap(
      options.foregroundDrawable(), options.foregroundTintColor()
    );
    Bitmap foregroundBitmapStale = bitmapProvider.generateBitmap(
      options.foregroundDrawableStale(), options.foregroundStaleTintColor()
    );
    if (renderMode == RenderMode.GPS) {
      foregroundBitmap = bitmapProvider.generateBitmap(
        options.gpsDrawable(), options.foregroundTintColor()
      );
      foregroundBitmapStale = bitmapProvider.generateBitmap(
        options.gpsDrawable(), options.foregroundStaleTintColor()
      );
    }
    mapboxMap.addImage(FOREGROUND_ICON, foregroundBitmap);
    mapboxMap.addImage(FOREGROUND_STALE_ICON, foregroundBitmapStale);
  }

  private void styleScaling(LocationComponentOptions options) {
    for (String layerId : layerMap) {
      Layer layer = mapboxMap.getLayer(layerId);
      if (layer != null && layer instanceof SymbolLayer) {
        layer.setProperties(
          iconSize(
            interpolate(linear(), zoom(),
              stop(options.minZoom(), options.minZoomIconScale()),
              stop(options.maxZoom(), options.maxZoomIconScale())
            )
          )
        );
      }
    }
  }

  private void determineIconsSource(LocationComponentOptions options) {
    String foregroundIconString = buildIconString(
      renderMode == RenderMode.GPS ? options.gpsName() : options.foregroundName(), FOREGROUND_ICON);
    String foregroundStaleIconString = buildIconString(options.foregroundStaleName(), FOREGROUND_STALE_ICON);
    String backgroundIconString = buildIconString(options.backgroundName(), BACKGROUND_ICON);
    String backgroundStaleIconString = buildIconString(options.backgroundStaleName(), BACKGROUND_STALE_ICON);
    String bearingIconString = buildIconString(options.bearingName(), BEARING_ICON);

    locationFeature.addStringProperty(PROPERTY_FOREGROUND_ICON, foregroundIconString);
    locationFeature.addStringProperty(PROPERTY_BACKGROUND_ICON, backgroundIconString);
    locationFeature.addStringProperty(PROPERTY_FOREGROUND_STALE_ICON, foregroundStaleIconString);
    locationFeature.addStringProperty(PROPERTY_BACKGROUND_STALE_ICON, backgroundStaleIconString);
    locationFeature.addStringProperty(PROPERTY_BEARING_ICON, bearingIconString);
    refreshSource();
  }

  private String buildIconString(@Nullable String bitmapName, @NonNull String drawableName) {
    if (bitmapName != null) {
      return bitmapName;
    }
    return drawableName;
  }

  void setLocationsStale(boolean isStale) {
    locationFeature.addBooleanProperty(PROPERTY_LOCATION_STALE, isStale);
    refreshSource();
    if (renderMode != RenderMode.GPS) {
      setLayerVisibility(ACCURACY_LAYER, !isStale);
    }
  }

  //
  // Map click event
  //

  boolean onMapClick(LatLng point) {
    PointF screenLoc = mapboxMap.getProjection().toScreenLocation(point);
    List<Feature> features = mapboxMap.queryRenderedFeatures(screenLoc,
      BACKGROUND_LAYER,
      FOREGROUND_LAYER,
      BEARING_LAYER
    );
    return !features.isEmpty();
  }

  @Override
  public void onNewLatLngValue(LatLng latLng) {
    Point point = Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude());
    setLocationPoint(point);
  }

  @Override
  public void onNewGpsBearingValue(float gpsBearing) {
    if (renderMode == RenderMode.GPS) {
      setBearingProperty(PROPERTY_GPS_BEARING, gpsBearing);
    }
  }

  @Override
  public void onNewCompassBearingValue(float compassBearing) {
    if (renderMode == RenderMode.COMPASS) {
      setBearingProperty(PROPERTY_COMPASS_BEARING, compassBearing);
    }
  }

  @Override
  public void onNewAccuracyRadiusValue(float accuracyRadiusValue) {
    updateAccuracyRadius(accuracyRadiusValue);
  }
}