package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

import timber.log.Timber;

public class LatLngBoundsActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final LatLng LOS_ANGELES = new LatLng(34.053940, -118.242622);
  private static final LatLng NEW_YORK = new LatLng(40.712730, -74.005953);

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_visible_bounds);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.setStyleUrl(Style.DARK);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(MapboxMap map) {
    mapboxMap = map;
    UiSettings uiSettings = mapboxMap.getUiSettings();
    uiSettings.setAllGesturesEnabled(false);

    mapboxMap.addMarker(new MarkerOptions()
      .title("Los Angeles")
      .snippet("City Hall")
      .position(LOS_ANGELES));

    mapboxMap.addMarker(new MarkerOptions()
      .title("New York")
      .snippet("City Hall")
      .position(NEW_YORK));

    List<LatLng> points = new ArrayList<>();
    points.add(NEW_YORK);
    points.add(LOS_ANGELES);

    // Create Bounds
    final LatLngBounds bounds = new LatLngBounds.Builder()
      .includes(points)
      .build();

    // Add map padding
    int mapPadding = (int) getResources().getDimension(R.dimen.fab_margin);
    mapboxMap.setPadding(mapPadding, mapPadding, mapPadding, mapPadding);

    // Move camera to the bounds with added padding
    int padding = (int) getResources().getDimension(R.dimen.coordinatebounds_margin);
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLngBounds(bounds, padding));

    // Log data
    Timber.e("Move to bounds: " + bounds.toString());
    Timber.e("Resulting bounds:" + mapboxMap.getProjection().getVisibleRegion().latLngBounds.toString());
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
