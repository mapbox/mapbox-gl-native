package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test activity showcasing toggling the user location on the map.
 */
public class MyLocationToggleActivity extends BaseLocationActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private FloatingActionButton locationToggleFab;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_my_location_toggle);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> mapboxMap = map);

    locationToggleFab = (FloatingActionButton) findViewById(R.id.fabLocationToggle);
    locationToggleFab.setOnClickListener(view -> {
      if (mapboxMap != null) {
        toggleGps(!mapboxMap.isMyLocationEnabled());
      }
    });
  }

  @Override
  protected void enableLocation(boolean enabled) {
    Timber.e("Enabling location: %s", enabled);
    mapboxMap.setMyLocationEnabled(enabled);
    if (enabled) {
      locationToggleFab.setImageResource(R.drawable.ic_location_disabled);
    } else {
      locationToggleFab.setImageResource(R.drawable.ic_my_location);
    }
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
