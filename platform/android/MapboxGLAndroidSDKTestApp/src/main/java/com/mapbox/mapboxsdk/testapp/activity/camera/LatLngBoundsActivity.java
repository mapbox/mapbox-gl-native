package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using the LatLngBounds camera API.
 * <p>
 * This activity opens the map at zoom level 0 and animates into a bounds set by Los Angeles and New York
 * with some additional padding and an animation duration of 1500 ms.
 * </p>
 */
public class LatLngBoundsActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final LatLng LOS_ANGELES = new LatLng(34.053940, -118.242622);
  private static final LatLng NEW_YORK = new LatLng(40.712730, -74.005953);

  private final LatLng CHINA_BOTTOM_LEFT = new LatLng(15.68169, 73.499857);
  private final LatLng CHINA_TOP_RIGHT = new LatLng(53.560711, 134.77281);

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
  public void onMapReady(final MapboxMap map) {
    mapboxMap = map;
    moveToBounds(new LatLngBounds.Builder().include(NEW_YORK).include(LOS_ANGELES).build(), new int[] {0, 0, 0, 0});
    new Handler().postDelayed(new Runnable() {
      @Override
      public void run() {
        moveToBounds(new LatLngBounds.Builder().include(CHINA_BOTTOM_LEFT).include(CHINA_TOP_RIGHT).build(),
          new int[] {100, 100, 100, 100 });
      }
    }, 5000);
  }

  private void moveToBounds(LatLngBounds latLngBounds, int[] padding) {
    mapboxMap.clear();
    mapboxMap.addMarker(new MarkerOptions().position(latLngBounds.getNorthEast()));
    mapboxMap.addMarker(new MarkerOptions().position(latLngBounds.getSouthEast()));
    mapboxMap.addMarker(new MarkerOptions().position(latLngBounds.getSouthWest()));
    mapboxMap.addMarker(new MarkerOptions().position(latLngBounds.getNorthWest()));
    CameraUpdate update =
      CameraUpdateFactory.newLatLngBounds(latLngBounds,
        padding[0],
        padding[1],
        padding[2],
        padding[3]);
    mapboxMap.moveCamera(update);
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
