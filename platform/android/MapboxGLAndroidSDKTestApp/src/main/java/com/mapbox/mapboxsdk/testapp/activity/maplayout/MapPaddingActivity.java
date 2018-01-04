package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using the map padding API.
 * <p>
 * This activity tests for correct padding around a marker (Bangalore) and correct padding around MyLocationView.
 * </p>
 */
public class MapPaddingActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_padding);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.setTag(true);
    mapView.onCreate(savedInstanceState);

    mapView.getMapAsync(mapboxMap -> {
      MapPaddingActivity.this.mapboxMap = mapboxMap;

      int paddingLeft = (int) getResources().getDimension(R.dimen.map_padding_left);
      int paddingBottom = (int) getResources().getDimension(R.dimen.map_padding_bottom);
      int paddingRight = (int) getResources().getDimension(R.dimen.map_padding_right);
      int paddingTop = (int) getResources().getDimension(R.dimen.map_padding_top);
      mapboxMap.setPadding(paddingLeft, paddingTop, paddingRight, paddingBottom);

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

  private void toggleGps(boolean enable) {
    try {
      // Enable user location
      mapboxMap.setMyLocationEnabled(enable);

      TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
      trackingSettings.setDismissLocationTrackingOnGesture(false);
      trackingSettings.setMyLocationTrackingMode(
        enable ? MyLocationTracking.TRACKING_FOLLOW : MyLocationTracking.TRACKING_NONE);
    } catch (SecurityException securityException) {
      // permission not granted is handled in FeatureOverviewActivity
      finish();
    }
  }

  private void moveToBangalore() {
    toggleGps(false);

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
      case R.id.action_user_tracking:
        if (mapboxMap != null) {
          toggleGps(true);
        }
        return true;

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

