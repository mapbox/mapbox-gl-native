package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;

import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing restricting user gestures to a bounds around Iceland.
 */
public class LatLngBoundsForCameraActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final LatLngBounds ICELAND_BOUNDS = new LatLngBounds.Builder()
    .include(new LatLng(66.852863, -25.985652))
    .include(new LatLng(62.985661, -12.626277))
    .build();

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_restricted_bounds);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    mapboxMap.setLatLngBoundsForCameraTarget(ICELAND_BOUNDS);
    mapboxMap.setMinZoomPreference(2);
    showBoundsArea();
    showCrosshair();
  }

  private void showBoundsArea() {
    PolygonOptions boundsArea = new PolygonOptions()
      .add(ICELAND_BOUNDS.getNorthWest())
      .add(ICELAND_BOUNDS.getNorthEast())
      .add(ICELAND_BOUNDS.getSouthEast())
      .add(ICELAND_BOUNDS.getSouthWest());
    boundsArea.alpha(0.25f);
    boundsArea.fillColor(Color.RED);
    mapboxMap.addPolygon(boundsArea);
  }

  private void showCrosshair() {
    View crosshair = new View(this);
    crosshair.setLayoutParams(new FrameLayout.LayoutParams(10, 10, Gravity.CENTER));
    crosshair.setBackgroundColor(Color.BLUE);
    mapView.addView(crosshair);
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
