package com.mapbox.mapboxsdk.testapp.activity.fragment;

import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapFragment;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using the MapFragment API using SDK Fragments.
 * <p>
 * Uses MapboxMapOptions to initialise the Fragment.
 * </p>
 */
public class MapFragmentActivity extends AppCompatActivity implements MapFragment.OnMapViewReadyCallback,
  OnMapReadyCallback, MapView.OnDidFinishRenderingFrameListener {

  private static final String TAG = "com.mapbox.map";
  private MapboxMap mapboxMap;
  private MapView mapView;
  private boolean initialCameraAnimation = true;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_fragment);

    MapFragment mapFragment;
    if (savedInstanceState == null) {
      mapFragment = MapFragment.newInstance(createFragmentOptions());
      getFragmentManager()
        .beginTransaction()
        .add(R.id.fragment_container, mapFragment, TAG)
        .commit();
    } else {
      mapFragment = (MapFragment) getFragmentManager().findFragmentByTag(TAG);
    }
    mapFragment.getMapAsync(this);
  }

  private MapboxMapOptions createFragmentOptions() {
    MapboxMapOptions options = MapboxMapOptions.createFromAttributes(this, null);

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
    mapboxMap.setStyle(Style.OUTDOORS);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    if (mapView != null) {
      mapView.removeOnDidFinishRenderingFrameListener(this);
    }
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
