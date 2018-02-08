package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.BitmapFactory;
import android.graphics.RectF;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;

import java.io.IOException;
import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;

/**
 * Test activity showcasing using the query rendered features API to count Symbols in a rectangle.
 */
public class QueryRenderedFeaturesBoxSymbolCountActivity extends AppCompatActivity {

  public MapView mapView;
  private MapboxMap mapboxMap;

  private Toast toast;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_query_features_box);

    final View selectionBox = findViewById(R.id.selection_box);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      QueryRenderedFeaturesBoxSymbolCountActivity.this.mapboxMap = mapboxMap;

      // Add a symbol layer (also works with annotations)
      try {
        mapboxMap.addSource(new GeoJsonSource("symbols-source", ResourceUtils.readRawResource(
          QueryRenderedFeaturesBoxSymbolCountActivity.this, R.raw.test_points_utrecht)));
      } catch (IOException ioException) {
        Timber.e(ioException, "Could not load geojson");
        return;
      }
      mapboxMap.addImage(
        "test-icon",
        BitmapFactory.decodeResource(getResources(),
          R.drawable.mapbox_marker_icon_default)
      );
      mapboxMap.addLayer(new SymbolLayer("symbols-layer", "symbols-source").withProperties(iconImage("test-icon")));

      selectionBox.setOnClickListener(view -> {
        // Query
        int top = selectionBox.getTop() - mapView.getTop();
        int left = selectionBox.getLeft() - mapView.getLeft();
        RectF box = new RectF(left, top, left + selectionBox.getWidth(), top + selectionBox.getHeight());
        Timber.i("Querying box %s", box);
        List<Feature> features = mapboxMap.queryRenderedFeatures(box, "symbols-layer");

        // Show count
        if (toast != null) {
          toast.cancel();
        }
        toast = Toast.makeText(
          QueryRenderedFeaturesBoxSymbolCountActivity.this,
          String.format("%s features in box", features.size()),
          Toast.LENGTH_SHORT);
        toast.show();
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
