package com.mapbox.mapboxsdk.location;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.VisibleForTesting;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;

import java.util.HashSet;
import java.util.List;
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
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;
import static com.mapbox.mapboxsdk.utils.ColorUtils.colorToRgbaString;

final class LocationLayerController {

  @RenderMode.Mode
  private int renderMode;

  private final MapboxMap mapboxMap;
  private Style style;
  private final LayerSourceProvider layerSourceProvider;
  private final LayerBitmapProvider bitmapProvider;
  private LocationComponentOptions options;
  private final OnRenderModeChangedListener internalRenderModeChangedListener;

  @VisibleForTesting
  final Set<String> layerSet = new HashSet<>();
  private Feature locationFeature;
  private GeoJsonSource locationSource;

  private boolean isHidden = true;

  private LocationComponentPositionManager positionManager;

  LocationLayerController(MapboxMap mapboxMap, Style style, LayerSourceProvider layerSourceProvider,
                          LayerFeatureProvider featureProvider, LayerBitmapProvider bitmapProvider,
                          @NonNull LocationComponentOptions options,
                          @NonNull OnRenderModeChangedListener internalRenderModeChangedListener) {
    this.mapboxMap = mapboxMap;
    this.style = style;
    this.layerSourceProvider = layerSourceProvider;
    this.bitmapProvider = bitmapProvider;
    this.locationFeature = featureProvider.generateLocationFeature(locationFeature, options);
    this.internalRenderModeChangedListener = internalRenderModeChangedListener;
    initializeComponents(style, options);
  }

  void initializeComponents(Style style, LocationComponentOptions options) {
    this.style = style;
    this.positionManager = new LocationComponentPositionManager(style, options.layerAbove(), options.layerBelow());
    addLocationSource();
    addLayers();
    applyStyle(options);

    if (isHidden) {
      hide();
    } else {
      show();
    }
  }

  void applyStyle(@NonNull LocationComponentOptions options) {
    if (positionManager.update(options.layerAbove(), options.layerBelow())) {
      removeLayers();
      addLayers();
      if (isHidden) {
        hide();
      }
    }

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

    if (!isHidden) {
      show();
    }
  }

  void setRenderMode(@RenderMode.Mode int renderMode) {
    if (this.renderMode == renderMode) {
      return;
    }
    this.renderMode = renderMode;

    styleForeground(options);
    determineIconsSource(options);
    if (!isHidden) {
      show();
    }
    internalRenderModeChangedListener.onRenderModeChanged(renderMode);
  }

  int getRenderMode() {
    return renderMode;
  }

  //
  // Layer action
  //

  void show() {
    isHidden = false;
    boolean isStale = locationFeature.getBooleanProperty(PROPERTY_LOCATION_STALE);
    switch (renderMode) {
      case RenderMode.NORMAL:
        setLayerVisibility(SHADOW_LAYER, true);
        setLayerVisibility(FOREGROUND_LAYER, true);
        setLayerVisibility(BACKGROUND_LAYER, true);
        setLayerVisibility(ACCURACY_LAYER, !isStale);
        setLayerVisibility(BEARING_LAYER, false);
        break;
      case RenderMode.COMPASS:
        setLayerVisibility(SHADOW_LAYER, true);
        setLayerVisibility(FOREGROUND_LAYER, true);
        setLayerVisibility(BACKGROUND_LAYER, true);
        setLayerVisibility(ACCURACY_LAYER, !isStale);
        setLayerVisibility(BEARING_LAYER, true);
        break;
      case RenderMode.GPS:
        setLayerVisibility(SHADOW_LAYER, false);
        setLayerVisibility(FOREGROUND_LAYER, true);
        setLayerVisibility(BACKGROUND_LAYER, true);
        setLayerVisibility(ACCURACY_LAYER, false);
        setLayerVisibility(BEARING_LAYER, false);
        break;
      default:
        break;
    }
  }

  void hide() {
    isHidden = true;
    for (String layerId : layerSet) {
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

  boolean isConsumingCompass() {
    return renderMode == RenderMode.COMPASS;
  }

  private void setLayerVisibility(@NonNull String layerId, boolean visible) {
    Layer layer = style.getLayer(layerId);
    if (layer != null) {
      String targetVisibility = visible ? VISIBLE : NONE;
      if (!layer.getVisibility().value.equals(targetVisibility)) {
        layer.setProperties(visibility(visible ? VISIBLE : NONE));
      }
    }
  }

  private void addLayers() {
    // positions the top-most reference layer
    Layer layer = layerSourceProvider.generateLayer(BEARING_LAYER);
    positionManager.addLayerToMap(layer);
    layerSet.add(layer.getId());

    // adds remaining layers while keeping the order
    addSymbolLayer(FOREGROUND_LAYER, BEARING_LAYER);
    addSymbolLayer(BACKGROUND_LAYER, FOREGROUND_LAYER);
    addSymbolLayer(SHADOW_LAYER, BACKGROUND_LAYER);
    addAccuracyLayer();
  }

  private void addSymbolLayer(@NonNull String layerId, @NonNull String beforeLayerId) {
    Layer layer = layerSourceProvider.generateLayer(layerId);
    addLayerToMap(layer, beforeLayerId);
  }

  private void addAccuracyLayer() {
    Layer accuracyLayer = layerSourceProvider.generateAccuracyLayer();
    addLayerToMap(accuracyLayer, BACKGROUND_LAYER);
  }

  private void addLayerToMap(Layer layer, @NonNull String idBelowLayer) {
    style.addLayerBelow(layer, idBelowLayer);
    layerSet.add(layer.getId());
  }

  private void removeLayers() {
    for (String layerId : layerSet) {
      style.removeLayer(layerId);
    }
    layerSet.clear();
  }

  private void setBearingProperty(@NonNull String propertyId, float bearing) {
    locationFeature.addNumberProperty(propertyId, bearing);
    refreshSource();
  }

  private void updateAccuracyRadius(float accuracy) {
    locationFeature.addNumberProperty(PROPERTY_ACCURACY_RADIUS, accuracy);
    refreshSource();
  }

  //
  // Source actions
  //

  private void addLocationSource() {
    locationSource = layerSourceProvider.generateSource(locationFeature);
    style.addSource(locationSource);
  }

  private void refreshSource() {
    GeoJsonSource source = style.getSourceAs(LOCATION_SOURCE);
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
    style.addImage(BACKGROUND_ICON, backgroundBitmap);
    style.addImage(BACKGROUND_STALE_ICON, backgroundStaleBitmap);
  }

  private void styleShadow(@NonNull LocationComponentOptions options) {
    style.addImage(SHADOW_ICON, bitmapProvider.generateShadowBitmap(options));
  }

  private void styleBearing(LocationComponentOptions options) {
    Bitmap bearingBitmap = bitmapProvider.generateBitmap(options.bearingDrawable(), options.bearingTintColor());
    style.addImage(BEARING_ICON, bearingBitmap);
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
    style.addImage(FOREGROUND_ICON, foregroundBitmap);
    style.addImage(FOREGROUND_STALE_ICON, foregroundBitmapStale);
  }

  private void styleScaling(@NonNull LocationComponentOptions options) {
    for (String layerId : layerSet) {
      Layer layer = style.getLayer(layerId);
      if (layer instanceof SymbolLayer) {
        layer.setProperties(
          iconSize(
            interpolate(linear(), zoom(),
              stop(mapboxMap.getMinZoomLevel(), options.minZoomIconScale()),
              stop(mapboxMap.getMaxZoomLevel(), options.maxZoomIconScale())
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

  @NonNull
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

  boolean onMapClick(@NonNull LatLng point) {
    PointF screenLoc = mapboxMap.getProjection().toScreenLocation(point);
    List<Feature> features = mapboxMap.queryRenderedFeatures(screenLoc,
      BACKGROUND_LAYER,
      FOREGROUND_LAYER,
      BEARING_LAYER
    );
    return !features.isEmpty();
  }

  private final MapboxAnimator.AnimationsValueChangeListener<LatLng> latLngValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<LatLng>() {
      @Override
      public void onNewAnimationValue(LatLng value) {
        Point point = Point.fromLngLat(value.getLongitude(), value.getLatitude());
        setLocationPoint(point);
      }
    };

  private final MapboxAnimator.AnimationsValueChangeListener<Float> gpsBearingValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<Float>() {
      @Override
      public void onNewAnimationValue(Float value) {
        setBearingProperty(PROPERTY_GPS_BEARING, value);
      }
    };

  private final MapboxAnimator.AnimationsValueChangeListener<Float> compassBearingValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<Float>() {
      @Override
      public void onNewAnimationValue(Float value) {
        setBearingProperty(PROPERTY_COMPASS_BEARING, value);
      }
    };

  private final MapboxAnimator.AnimationsValueChangeListener<Float> accuracyValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<Float>() {
      @Override
      public void onNewAnimationValue(Float value) {
        updateAccuracyRadius(value);
      }
    };

  Set<AnimatorListenerHolder> getAnimationListeners() {
    Set<AnimatorListenerHolder> holders = new HashSet<>();
    holders.add(new AnimatorListenerHolder(MapboxAnimator.ANIMATOR_LAYER_LATLNG, latLngValueListener));

    if (renderMode == RenderMode.GPS) {
      holders.add(new AnimatorListenerHolder(MapboxAnimator.ANIMATOR_LAYER_GPS_BEARING, gpsBearingValueListener));
    } else if (renderMode == RenderMode.COMPASS) {
      holders.add(
        new AnimatorListenerHolder(MapboxAnimator.ANIMATOR_LAYER_COMPASS_BEARING, compassBearingValueListener));
    }

    if (renderMode == RenderMode.COMPASS || renderMode == RenderMode.NORMAL) {
      holders.add(new AnimatorListenerHolder(MapboxAnimator.ANIMATOR_LAYER_ACCURACY, accuracyValueListener));
    }

    return holders;
  }
}