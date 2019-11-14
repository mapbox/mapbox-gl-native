package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test activity showcasing using maximum and minimum zoom levels to restrict camera movement.
 */
public class MaxMinZoomActivity extends AppCompatActivity implements OnMapReadyCallback {

  private MapView mapView;
  private MapboxMap mapboxMap;

  private MapboxMap.OnMapClickListener clickListener = new MapboxMap.OnMapClickListener() {
    @Override
    public boolean onMapClick(@NonNull LatLng point) {
      if (mapboxMap != null) {
        mapboxMap.setStyle(new Style.Builder().fromUri(Style.OUTDOORS));
      }
      return true;
    }
  };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_maxmin_zoom);

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);

    mapView.addOnDidFinishLoadingStyleListener(() -> Timber.d("Style Loaded"));
  }

  @Override
  public void onMapReady(@NonNull final MapboxMap map) {
    mapboxMap = map;
    mapboxMap.setStyle(Style.MAPBOX_STREETS);
    mapboxMap.setMinZoomPreference(3);
    mapboxMap.setMaxZoomPreference(5);
    mapboxMap.addOnMapClickListener(clickListener);
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
    if (mapboxMap != null) {
      mapboxMap.removeOnMapClickListener(clickListener);
    }
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
