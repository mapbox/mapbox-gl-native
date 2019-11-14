package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.Color;
import android.graphics.RectF;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.lt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toNumber;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;

/**
 * Demo's query rendered features
 */
public class QueryRenderedFeaturesBoxHighlightActivity extends AppCompatActivity {

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
      QueryRenderedFeaturesBoxHighlightActivity.this.mapboxMap = mapboxMap;

      // Add layer / source
      final GeoJsonSource source = new GeoJsonSource("highlighted-shapes-source");
      final Layer layer = new FillLayer("highlighted-shapes-layer", "highlighted-shapes-source")
        .withProperties(fillColor(Color.RED));

      selectionBox.setOnClickListener(view -> {
        // Query
        int top = selectionBox.getTop() - mapView.getTop();
        int left = selectionBox.getLeft() - mapView.getLeft();
        RectF box = new RectF(left, top, left + selectionBox.getWidth(), top + selectionBox.getHeight());
        Timber.i("Querying box %s for buildings", box);

        Expression filter = lt(toNumber(get("height")), literal(10));
        List<Feature> features = mapboxMap.queryRenderedFeatures(box, filter, "building");

        // Show count
        Toast.makeText(
          QueryRenderedFeaturesBoxHighlightActivity.this,
          String.format("%s features in box", features.size()),
          Toast.LENGTH_SHORT).show();

        // Update source data
        source.setGeoJson(FeatureCollection.fromFeatures(features));
      });

      mapboxMap.setStyle(new Style.Builder()
        .fromUri(Style.MAPBOX_STREETS)
        .withSource(source)
        .withLayer(layer)
      );
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
