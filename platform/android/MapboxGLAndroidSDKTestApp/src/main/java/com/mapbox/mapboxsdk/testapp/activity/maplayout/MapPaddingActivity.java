package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using the map padding API.
 */
public class MapPaddingActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_padding);

    mapView = findViewById(R.id.mapView);
    mapView.setTag(true);
    mapView.onCreate(savedInstanceState);

    mapView.getMapAsync(mapboxMap -> {
      MapPaddingActivity.this.mapboxMap = mapboxMap;

      int paddingLeft = (int) getResources().getDimension(R.dimen.map_padding_left);
      int paddingBottom = (int) getResources().getDimension(R.dimen.map_padding_bottom);
      int paddingRight = (int) getResources().getDimension(R.dimen.map_padding_right);
      int paddingTop = (int) getResources().getDimension(R.dimen.map_padding_top);
      mapboxMap.setPadding(paddingLeft, paddingTop, paddingRight, paddingBottom);

      UiSettings settings = mapboxMap.getUiSettings();
      settings.setLogoMargins(paddingLeft, 0, 0, paddingBottom);
      settings.setCompassMargins(0, paddingTop, paddingRight, 0);
      settings.setAttributionEnabled(false);

      moveToBangalore();
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

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_padding, menu);
    return true;
  }

  private void moveToBangalore() {
    LatLng bangalore = new LatLng(12.9810816, 77.6368034);
    CameraPosition cameraPosition = new CameraPosition.Builder()
      .zoom(16)
      .target(bangalore)
      .bearing(40)
      .tilt(45)
      .build();

    mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
    mapboxMap.addMarker(new MarkerOptions().title("Center map").position(bangalore));
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {

      case R.id.action_bangalore:
        if (mapboxMap != null) {
          moveToBangalore();
        }
        return true;

      default:
        return super.onOptionsItemSelected(item);
    }
  }

}

