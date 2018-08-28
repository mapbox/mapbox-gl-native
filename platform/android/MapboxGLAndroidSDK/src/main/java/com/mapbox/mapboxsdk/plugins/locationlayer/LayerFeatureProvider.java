package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Point;

import static com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerConstants.PROPERTY_COMPASS_BEARING;
import static com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerConstants.PROPERTY_GPS_BEARING;
import static com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerConstants.PROPERTY_LOCATION_STALE;

class LayerFeatureProvider {

  @NonNull
  Feature generateLocationFeature(@Nullable Feature locationFeature, LocationLayerOptions options) {
    if (locationFeature != null) {
      return locationFeature;
    }
    locationFeature = Feature.fromGeometry(Point.fromLngLat(0.0, 0.0));
    locationFeature.addNumberProperty(PROPERTY_GPS_BEARING, 0f);
    locationFeature.addNumberProperty(PROPERTY_COMPASS_BEARING, 0f);
    locationFeature.addBooleanProperty(PROPERTY_LOCATION_STALE, options.enableStaleState());
    return locationFeature;
  }
}
