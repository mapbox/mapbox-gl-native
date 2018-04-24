package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.Toast;

import com.google.gson.JsonObject;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;

import static com.mapbox.mapboxsdk.style.expressions.Expression.eq;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.neq;

/**
 * Test activity showcasing using the query source features API to query feature counts
 */
public class QuerySourceFeaturesActivity extends AppCompatActivity {

  public MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_query_source_features);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      QuerySourceFeaturesActivity.this.mapboxMap = mapboxMap;

      JsonObject properties = new JsonObject();
      properties.addProperty("key1", "value1");
      final GeoJsonSource source = new GeoJsonSource("test-source",
        FeatureCollection.fromFeatures(new Feature[] {
          Feature.fromGeometry(Point.fromLngLat(0, 0), properties)
        }));
      mapboxMap.addSource(source);

      mapboxMap.addLayer(new CircleLayer("test-layer", source.getId()).withFilter(neq(get("key1"), literal("value1"))));

      // Add a click listener
      mapboxMap.setOnMapClickListener(point -> {
        // Query
        List<Feature> features = source.querySourceFeatures(eq(get("key1"), literal("value1")));
        Toast.makeText(QuerySourceFeaturesActivity.this, String.format("Found %s features",
          features.size()), Toast.LENGTH_SHORT).show();
      });
    });

  }

  public MapboxMap getMapboxMap() {
    return mapboxMap;
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
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
