package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test activity showcasing how to listen to map change events.
 */
public class MapChangeActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);

    mapView = findViewById(R.id.mapView);
    mapView.addOnCameraIsChangingListener(() -> Timber.v("OnCameraIsChanging"));
    mapView.addOnCameraDidChangeListener(animated -> Timber.v("OnCamaraDidChange: animated: %s", animated));
    mapView.addOnCameraWillChangeListener(animated -> Timber.v("OnCameraWilChange: animated: %s", animated));
    mapView.addOnDidFailLoadingMapListener(errorMessage -> Timber.v("OnDidFailLoadingMap: %s", errorMessage));
    mapView.addOnDidFinishLoadingMapListener(() -> Timber.v("OnDidFinishLoadingMap"));
    mapView.addOnDidFinishLoadingStyleListener(() -> Timber.v("OnDidFinishLoadingStyle"));
    mapView.addOnDidFinishRenderingFrameListener(fully -> Timber.v("OnDidFinishRenderingFrame: fully: %s", fully));
    mapView.addOnDidFinishRenderingMapListener(fully -> Timber.v("OnDidFinishRenderingMap: fully: %s", fully));
    mapView.addOnDidEnterIdleListener(() -> Timber.v("OnDidEnterIdle"));
    mapView.addOnSourceChangedListener(sourceId -> Timber.v("OnSourceChangedListener: source with id: %s", sourceId));
    mapView.addOnWillStartLoadingMapListener(() -> Timber.v("OnWillStartLoadingMap"));
    mapView.addOnWillStartRenderingFrameListener(() -> Timber.v("OnWillStartRenderingFrame"));
    mapView.addOnWillStartRenderingMapListener(() -> Timber.v("OnWillStartRenderingMap"));
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(
        new LatLng(55.754020, 37.620948), 12), 9000);
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
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }
}
