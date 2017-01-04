package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

public class AddRemoveMarkerActivity extends AppCompatActivity {

  public static final double THRESHOLD = 5.0;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private double lastZoom;
  private boolean isShowingHighThresholdMarker;
  private boolean isShowingLowThresholdMarker;

  private MarkerOptions lowThresholdMarker;
  private MarkerOptions highThresholdMarker;
  private Marker activeMarker;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_add_remove_marker);

    final Icon icon1 = IconFactory.getInstance(this).fromResource(R.drawable.ic_arsenal);
    final Icon icon2 = IconFactory.getInstance(this).fromResource(R.drawable.ic_chelsea);

    lowThresholdMarker = new MarkerOptions()
      .icon(icon1)
      .position(new LatLng(-0.1, 0));

    highThresholdMarker = new MarkerOptions()
      .icon(icon2)
      .position(new LatLng(0.1, 0));

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap mapboxMap) {
        AddRemoveMarkerActivity.this.mapboxMap = mapboxMap;
        updateZoom(mapboxMap.getCameraPosition().zoom);
        mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.9f));
        mapboxMap.setOnCameraChangeListener(new MapboxMap.OnCameraChangeListener() {
          @Override
          public void onCameraChange(CameraPosition position) {
            updateZoom(position.zoom);
          }
        });
      }
    });
  }

  private void updateZoom(double zoom) {
    if (lastZoom == zoom) {
      return;
    }

    lastZoom = zoom;
    if (zoom > THRESHOLD) {
      showHighThresholdMarker();
    } else {
      showLowThresholdMarker();
    }
  }

  private void showLowThresholdMarker() {
    if (isShowingLowThresholdMarker) {
      return;
    }

    isShowingLowThresholdMarker = true;
    isShowingHighThresholdMarker = false;

    if (activeMarker != null) {
      Timber.d("Remove marker with " + activeMarker.getId());
      mapboxMap.removeMarker(activeMarker);
    } else {
      Timber.e("active marker is null");
    }

    activeMarker = mapboxMap.addMarker(lowThresholdMarker);
    Timber.d("showLowThresholdMarker() " + activeMarker.getId());
  }

  private void showHighThresholdMarker() {
    if (isShowingHighThresholdMarker) {
      return;
    }

    isShowingLowThresholdMarker = false;
    isShowingHighThresholdMarker = true;

    if (activeMarker != null) {
      Timber.d("Remove marker with " + activeMarker.getId());
      mapboxMap.removeMarker(activeMarker);
    } else {
      Timber.e("active marker is null");
    }

    activeMarker = mapboxMap.addMarker(highThresholdMarker);
    Timber.d("showHighThresholdMarker() " + activeMarker.getId());
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
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }
}