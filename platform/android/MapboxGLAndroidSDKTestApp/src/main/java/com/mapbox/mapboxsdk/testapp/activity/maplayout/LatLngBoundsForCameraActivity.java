package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.graphics.Color;
import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.FrameLayout;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing restricting user gestures to a bounds around Iceland, almost worldview and IDL.
 */
public class LatLngBoundsForCameraActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final LatLngBounds ICELAND_BOUNDS = new LatLngBounds.Builder()
    .include(new LatLng(66.852863, -25.985652))
    .include(new LatLng(62.985661, -12.626277))
    .build();

  private static final LatLngBounds ALMOST_WORLD_BOUNDS = new LatLngBounds.Builder()
    .include(new LatLng(20.0, 170.0))
    .include(new LatLng(-20, -170.0))
    .build();

  private static final LatLngBounds CROSS_IDL_BOUNDS = new LatLngBounds.Builder()
    .include(new LatLng(20.0, 170.0))
    .include(new LatLng(-20, 190.0))
    .build();

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_restricted_bounds);

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    mapboxMap.setStyle(Style.SATELLITE_STREETS);
    mapboxMap.setMinZoomPreference(2);
    mapboxMap.getUiSettings().setFlingVelocityAnimationEnabled(false);
    showCrosshair();
    setupBounds(ICELAND_BOUNDS);
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_bounds, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case R.id.menu_action_allmost_world_bounds:
        setupBounds(ALMOST_WORLD_BOUNDS);
        return true;
      case R.id.menu_action_cross_idl:
        setupBounds(CROSS_IDL_BOUNDS);
        return true;
      case R.id.menu_action_reset:
        setupBounds(null);
        return true;
    }
    return super.onOptionsItemSelected(item);
  }

  private void setupBounds(@Nullable LatLngBounds bounds) {
    mapboxMap.setLatLngBoundsForCameraTarget(bounds);
    showBoundsArea(bounds);
  }

  private void showBoundsArea(@Nullable LatLngBounds bounds) {
    mapboxMap.clear();
    if (bounds != null) {
      PolygonOptions boundsArea = new PolygonOptions()
        .add(bounds.getNorthWest())
        .add(bounds.getNorthEast())
        .add(bounds.getSouthEast())
        .add(bounds.getSouthWest());
      boundsArea.alpha(0.25f);
      boundsArea.fillColor(Color.RED);
      mapboxMap.addPolygon(boundsArea);
    }
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
