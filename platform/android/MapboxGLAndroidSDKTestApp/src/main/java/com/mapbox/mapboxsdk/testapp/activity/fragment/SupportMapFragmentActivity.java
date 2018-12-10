package com.mapbox.mapboxsdk.testapp.activity.fragment;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapFragment;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.SupportMapFragment;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using the MapFragment API using Support Library Fragments.
 * <p>
 * Uses MapboxMapOptions to initialise the Fragment.
 * </p>
 */
public class SupportMapFragmentActivity extends AppCompatActivity implements MapFragment.OnMapViewReadyCallback,
  OnMapReadyCallback, MapView.OnDidFinishRenderingFrameListener {

  private MapboxMap mapboxMap;
  private MapView mapView;
  private boolean initialCameraAnimation = true;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_fragment);
    if (savedInstanceState == null) {
      SupportMapFragment mapFragment = SupportMapFragment.newInstance(createFragmentOptions());
      getSupportFragmentManager()
        .beginTransaction()
        .add(R.id.fragment_container, mapFragment, "com.mapbox.map")
        .commit();
      mapFragment.getMapAsync(this);
    }
  }

  private MapboxMapOptions createFragmentOptions() {
    MapboxMapOptions options = new MapboxMapOptions();
    options.styleUrl(Style.MAPBOX_STREETS);

    options.scrollGesturesEnabled(false);
    options.zoomGesturesEnabled(false);
    options.tiltGesturesEnabled(false);
    options.rotateGesturesEnabled(false);
    options.debugActive(false);

    LatLng dc = new LatLng(38.90252, -77.02291);

    options.minZoomPreference(9);
    options.maxZoomPreference(11);
    options.camera(new CameraPosition.Builder()
      .target(dc)
      .zoom(11)
      .build());
    return options;
  }

  @Override
  public void onMapViewReady(MapView map) {
    mapView = map;
    mapView.addOnDidFinishRenderingFrameListener(this);
  }

  @Override
  public void onMapReady(@NonNull MapboxMap map) {
    mapboxMap = map;
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.removeOnDidFinishRenderingFrameListener(this);
  }

  @Override
  public void onDidFinishRenderingFrame(boolean fully) {
    if (initialCameraAnimation && fully && mapboxMap != null) {
      mapboxMap.animateCamera(
        CameraUpdateFactory.newCameraPosition(new CameraPosition.Builder().tilt(45.0).build()), 5000);
      initialCameraAnimation = false;
    }
  }
}