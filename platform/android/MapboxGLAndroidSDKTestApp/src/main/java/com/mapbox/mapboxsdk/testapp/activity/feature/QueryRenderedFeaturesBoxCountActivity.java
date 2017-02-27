package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.RectF;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Feature;

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
    mapView.getMapAsync(new OnMapReadyCallback() {
      @SuppressWarnings("ConstantConditions")
      @Override
      public void onMapReady(final MapboxMap mapboxMap) {
        QueryRenderedFeaturesBoxCountActivity.this.mapboxMap = mapboxMap;
        selectionBox.setOnClickListener(new View.OnClickListener() {
          @Override
          public void onClick(View view) {
            // Query
            int top = selectionBox.getTop() - mapView.getTop();
            int left = selectionBox.getLeft() - mapView.getLeft();
            RectF box = new RectF(left, top, left + selectionBox.getWidth(), top + selectionBox.getHeight());
            Timber.i(String.format("Querying box %s", box));
            List<Feature> features = mapboxMap.queryRenderedFeatures(box);

            // Show count
            Toast.makeText(
              QueryRenderedFeaturesBoxCountActivity.this,
              String.format("%s features in box", features.size()),
              Toast.LENGTH_SHORT).show();

            // Debug output
            debugOutput(features);
          }
        });
      }
    });
  }

  private void debugOutput(List<Feature> features) {
    Timber.i(String.format("Got %s features", features.size()));
    for (Feature feature : features) {
      if (feature != null) {
        Timber.i(String.format("Got feature %s with %s properties and Geometry %s",
          feature.getId(),
          feature.getProperties() != null ? feature.getProperties().entrySet().size() : "<null>",
          feature.getGeometry() != null ? feature.getGeometry().getClass().getSimpleName() : "<null>")
        );
        if (feature.getProperties() != null) {
          for (Map.Entry<String, JsonElement> entry : feature.getProperties().entrySet()) {
            Timber.i(String.format("Prop %s - %s", entry.getKey(), entry.getValue()));
          }
        }
      } else {
        // TODO Question: Why not formatting here??
        Timber.i("Got NULL feature %s");
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
