package com.mapbox.mapboxsdk.testapp.activity.textureview;

import android.animation.ObjectAnimator;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.Locale;

/**
 * Test animating a {@link android.view.TextureView} backed map.
 */
public class TextureViewAnimationActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private Handler handler;
  private Runnable delayed;

  private static LatLng[] PLACES = {
    new LatLng(37.7749, -122.4194), // SF
    new LatLng(38.9072, -77.0369), // DC
    new LatLng(52.3702, 4.8952), // AMS
    new LatLng(60.1699, 24.9384), // HEL
    new LatLng(-13.1639, -74.2236), // AYA
    new LatLng(52.5200, 13.4050), // BER
    new LatLng(12.9716, 77.5946), // BAN
    new LatLng(31.2304, 121.4737) // SHA
  };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_textureview_animate);
    handler = new Handler(getMainLooper());
    setupToolbar();
    setupMapView(savedInstanceState);
  }

  private void setupToolbar() {
    ActionBar actionBar = getSupportActionBar();
    if (actionBar != null) {
      getSupportActionBar().setDisplayHomeAsUpEnabled(true);
      getSupportActionBar().setHomeButtonEnabled(true);
    }
  }

  private void setupMapView(Bundle savedInstanceState) {
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.getMapAsync(mapboxMap -> {
      TextureViewAnimationActivity.this.mapboxMap = mapboxMap;

      setFpsView(mapboxMap);

      // Animate the map view
      ObjectAnimator animation = ObjectAnimator.ofFloat(mapView, "rotationY", 0.0f, 360f);
      animation.setDuration(3600);
      animation.setRepeatCount(ObjectAnimator.INFINITE);
      animation.start();

      // Start an animation on the map as well
      flyTo(mapboxMap, 0, 14);
    });
  }

  private void flyTo(final MapboxMap mapboxMap, final int place, final double zoom) {
    mapboxMap.animateCamera(
      CameraUpdateFactory.newLatLngZoom(PLACES[place], zoom),
      10000,
      new MapboxMap.CancelableCallback() {
        @Override
        public void onCancel() {
          delayed = () -> {
            delayed = null;
            flyTo(mapboxMap, place, zoom);
          };
          handler.postDelayed(delayed, 2000);
        }

        @Override
        public void onFinish() {
          flyTo(mapboxMap, place == (PLACES.length - 1) ? 0 : place + 1, zoom);
        }
      });
  }

  private void setFpsView(MapboxMap mapboxMap) {
    final TextView fpsView = (TextView) findViewById(R.id.fpsView);
    mapboxMap.setOnFpsChangedListener(fps -> fpsView.setText(String.format(Locale.US, "FPS: %4.2f", fps)));
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
    if (handler != null && delayed != null) {
      handler.removeCallbacks(delayed);
    }
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
