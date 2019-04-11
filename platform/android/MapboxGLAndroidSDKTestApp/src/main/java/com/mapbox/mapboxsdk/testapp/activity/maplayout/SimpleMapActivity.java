package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing a simple MapView without any MapboxMap interaction.
 */
public class SimpleMapActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.setStyle(
        new Style.Builder().fromUrl(Style.MAPBOX_STREETS)
      );
    });
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
    if (mapboxMap != null) {
      // Regression test for #14394
      mapboxMap.queryRenderedFeatures(new PointF(0,0));
    }
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
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }
}
