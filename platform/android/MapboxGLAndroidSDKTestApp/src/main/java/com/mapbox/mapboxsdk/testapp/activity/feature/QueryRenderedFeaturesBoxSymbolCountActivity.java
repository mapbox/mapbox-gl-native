package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.RectF;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.BackgroundLayer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;

import java.io.IOException;
import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.expressions.Expression.rgb;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.backgroundColor;
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

      try {
        String testPoints = ResourceUtils.readRawResource(mapView.getContext(), R.raw.test_points_utrecht);
        Bitmap markerImage = BitmapFactory.decodeResource(getResources(), R.drawable.mapbox_marker_icon_default);

        mapboxMap.setStyle(new Style.Builder()
          .withLayer(
            new BackgroundLayer("bg")
              .withProperties(
                backgroundColor(rgb(120, 161, 226))
              )
          )
          .withLayer(
            new SymbolLayer("symbols-layer", "symbols-source")
              .withProperties(
                iconImage("test-icon")
              )
          )
          .withSource(
            new GeoJsonSource("symbols-source", testPoints)
          )
          .withImage("test-icon", markerImage)
        );
      } catch (IOException exception) {
        exception.printStackTrace();
      }

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
