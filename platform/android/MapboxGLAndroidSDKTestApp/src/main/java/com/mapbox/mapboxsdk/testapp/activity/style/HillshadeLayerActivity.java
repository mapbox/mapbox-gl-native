package com.mapbox.mapboxsdk.testapp.activity.style;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.HillshadeLayer;
import com.mapbox.mapboxsdk.style.sources.RasterDemSource;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using HillshadeLayer.
 */
public class HillshadeLayerActivity extends AppCompatActivity {

  private static final String LAYER_ID = "hillshade-layer";
  private static final String LAYER_BELOW_ID = "waterway-river-canal";
  private static final String SOURCE_ID = "hillshade-source";
  private static final String SOURCE_URL = "mapbox://mapbox.terrain-rgb";

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_fill_extrusion_layer);

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;

      RasterDemSource rasterDemSource = new RasterDemSource(SOURCE_ID, SOURCE_URL);
      HillshadeLayer hillshadeLayer = new HillshadeLayer(LAYER_ID, SOURCE_ID);
      mapboxMap.setStyle(new Style.Builder()
        .fromUri(Style.MAPBOX_STREETS)
        .withLayerBelow(hillshadeLayer, LAYER_BELOW_ID)
        .withSource(rasterDemSource)
      );
    });
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
  public void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  public void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }
}