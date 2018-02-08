package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.RectF;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.google.gson.JsonElement;
import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;
import java.util.Map;

import timber.log.Timber;

/**
 * Test activity showcasing using the query rendered features API to count features in a rectangle.
 */
public class QueryRenderedFeaturesBoxCountActivity extends AppCompatActivity {

  public MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_query_features_box);

    final View selectionBox = findViewById(R.id.selection_box);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      QueryRenderedFeaturesBoxCountActivity.this.mapboxMap = mapboxMap;
      selectionBox.setOnClickListener(view -> {
        // Query
        int top = selectionBox.getTop() - mapView.getTop();
        int left = selectionBox.getLeft() - mapView.getLeft();
        RectF box = new RectF(left, top, left + selectionBox.getWidth(), top + selectionBox.getHeight());
        Timber.i("Querying box %s", box);
        List<Feature> features = mapboxMap.queryRenderedFeatures(box);

        // Show count
        Toast.makeText(
          QueryRenderedFeaturesBoxCountActivity.this,
          String.format("%s features in box", features.size()),
          Toast.LENGTH_SHORT).show();

        // Debug output
        debugOutput(features);
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
        Timber.i("Got 0 features");
      }
    }
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
