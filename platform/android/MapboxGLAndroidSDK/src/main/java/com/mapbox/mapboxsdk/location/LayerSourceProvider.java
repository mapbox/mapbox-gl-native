package com.mapbox.mapboxsdk.location;

import android.support.annotation.NonNull;
import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonOptions;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.ACCURACY_LAYER;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.BACKGROUND_LAYER;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.BEARING_LAYER;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.FOREGROUND_LAYER;
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
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_PULSING_CIRCLE_LAYER;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.SHADOW_ICON;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.SHADOW_LAYER;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.switchCase;
import static com.mapbox.mapboxsdk.style.layers.Property.ICON_ROTATION_ALIGNMENT_MAP;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circlePitchAlignment;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleStrokeColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconOffset;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconRotate;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconRotationAlignment;

class LayerSourceProvider {

  private static final String EMPTY_STRING = "";

  @NonNull
  GeoJsonSource generateSource(Feature locationFeature) {
    return new GeoJsonSource(
      LOCATION_SOURCE,
      locationFeature,
      new GeoJsonOptions().withMaxZoom(16)
    );
  }

  @NonNull
  Layer generateLayer(@NonNull String layerId) {
    SymbolLayer layer = new SymbolLayer(layerId, LOCATION_SOURCE);
    layer.setProperties(
      iconAllowOverlap(true),
      iconIgnorePlacement(true),
      iconRotationAlignment(ICON_ROTATION_ALIGNMENT_MAP),
      iconRotate(
        match(literal(layerId), literal(0f),
          stop(FOREGROUND_LAYER, get(PROPERTY_GPS_BEARING)),
          stop(BACKGROUND_LAYER, get(PROPERTY_GPS_BEARING)),
          stop(SHADOW_LAYER, get(PROPERTY_GPS_BEARING)),
          stop(BEARING_LAYER, get(PROPERTY_COMPASS_BEARING))
        )
      ),
      iconImage(
        match(literal(layerId), literal(EMPTY_STRING),
          stop(FOREGROUND_LAYER, switchCase(
            get(PROPERTY_LOCATION_STALE), get(PROPERTY_FOREGROUND_STALE_ICON),
            get(PROPERTY_FOREGROUND_ICON))),
          stop(BACKGROUND_LAYER, switchCase(
            get(PROPERTY_LOCATION_STALE), get(PROPERTY_BACKGROUND_STALE_ICON),
            get(PROPERTY_BACKGROUND_ICON))),
          stop(SHADOW_LAYER, literal(SHADOW_ICON)),
          stop(BEARING_LAYER, get(PROPERTY_BEARING_ICON))
        )
      ),
      iconOffset(
        match(literal(layerId), literal(new Float[] {0f, 0f}),
          stop(literal(FOREGROUND_LAYER), get(PROPERTY_FOREGROUND_ICON_OFFSET)),
          stop(literal(SHADOW_LAYER), get(PROPERTY_SHADOW_ICON_OFFSET))
        )
      )
    );
    return layer;
  }

  @NonNull
  Layer generateAccuracyLayer() {
    return new CircleLayer(ACCURACY_LAYER, LOCATION_SOURCE)
      .withProperties(
        circleRadius(get(PROPERTY_ACCURACY_RADIUS)),
        circleColor(get(PROPERTY_ACCURACY_COLOR)),
        circleOpacity(get(PROPERTY_ACCURACY_ALPHA)),
        circleStrokeColor(get(PROPERTY_ACCURACY_COLOR)),
        circlePitchAlignment(Property.CIRCLE_PITCH_ALIGNMENT_MAP)
      );
  }

  @NonNull
  Layer generatePulsingCircleLayer() {
    return new CircleLayer(PROPERTY_PULSING_CIRCLE_LAYER, LOCATION_SOURCE)
      .withProperties(
        circlePitchAlignment(Property.CIRCLE_PITCH_ALIGNMENT_MAP)
      );
  }
}
