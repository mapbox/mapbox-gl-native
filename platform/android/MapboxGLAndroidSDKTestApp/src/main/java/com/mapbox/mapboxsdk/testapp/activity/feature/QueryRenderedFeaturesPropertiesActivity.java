package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import com.google.gson.JsonElement;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import timber.log.Timber;

import java.util.List;
import java.util.Map;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;

/**
 * Test activity showcasing using the query rendered features API to query feature properties on Map click.
 */
public class QueryRenderedFeaturesPropertiesActivity extends AppCompatActivity {

  private static final String SYMBOL_SOURCE_ID = "symbol-source-id";
  private static final String SYMBOL_LAYER_ID = "symbol-source-id";

  private MapView mapView;
  private MapboxMap mapboxMap;
  private GeoJsonSource geoJsonSource;
  private SymbolLayer symbolLayer;

  private MapboxMap.OnMapClickListener mapClickListener = new MapboxMap.OnMapClickListener() {
    @Override
    public boolean onMapClick(@NonNull LatLng point) {
      final float density = getResources().getDisplayMetrics().density;
      final PointF pixel = mapboxMap.getProjection().toScreenLocation(point);
      Timber.i(
        "Requesting features for %sx%s (%sx%s adjusted for density)",
        pixel.x, pixel.y, pixel.x / density, pixel.y / density
      );
      List<Feature> features = mapboxMap.queryRenderedFeatures(pixel);

      // Debug output
      debugOutput(features);

      geoJsonSource.setGeoJson(Feature.fromGeometry(Point.fromLngLat(point.getLongitude(), point.getLatitude())));

      StringBuilder stringBuilder = new StringBuilder();
      if (!features.isEmpty()) {
        stringBuilder.append(String.format("Found %s features\n", features.size()));
        Feature feature = features.get(0);
        for (Map.Entry<String, JsonElement> prop : feature.properties().entrySet()) {
          stringBuilder.append(String.format("%s: %s\n", prop.getKey(), prop.getValue()));
        }
      } else {
        stringBuilder.append("No features here");
      }
      symbolLayer.setProperties(textField(stringBuilder.toString()));
      return true;
    }
  };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_query_features_point);

    // Initialize map as normal
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      mapboxMap.setStyle(Style.MAPBOX_STREETS, style -> {
        QueryRenderedFeaturesPropertiesActivity.this.mapboxMap = mapboxMap;
        style.addSource(geoJsonSource = new GeoJsonSource(SYMBOL_SOURCE_ID));
        style.addLayer(symbolLayer = new SymbolLayer(SYMBOL_LAYER_ID, SYMBOL_SOURCE_ID));
        // Add a click listener
        mapboxMap.addOnMapClickListener(mapClickListener);
      });
    });
  }

  private void debugOutput(List<Feature> features) {
    Timber.i("Got %s features", features.size());
    for (Feature feature : features) {
      if (feature != null) {
        Timber.i("Got feature %s with %s properties and Geometry %s",
          feature.id(),
          feature.properties() != null ? feature.properties().entrySet().size() : "<null>",
          feature.geometry() != null ? feature.geometry().getClass().getSimpleName() : "<null>"
        );
        if (feature.properties() != null) {
          for (Map.Entry<String, JsonElement> entry : feature.properties().entrySet()) {
            Timber.i("Prop %s - %s", entry.getKey(), entry.getValue());
          }
        }
      } else {
        Timber.i("Got NULL feature");
      }
    }
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  protected void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  protected void onPause() {
    super.onPause();
    mapView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    if (mapboxMap != null) {
      mapboxMap.removeOnMapClickListener(mapClickListener);
    }
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
