package com.mapbox.mapboxsdk.testapp.activity.textureview;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;

import java.io.IOException;

import timber.log.Timber;

/**
 * Example showcasing how to create a TextureView with a transparent background.
 */
public class TextureViewTransparentBackgroundActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_textureview_transparent);
    setupBackground();
    setupMapView(savedInstanceState);
  }

  private void setupBackground() {
    ImageView imageView = findViewById(R.id.imageView);
    imageView.setImageResource(R.drawable.water);
    imageView.setScaleType(ImageView.ScaleType.FIT_XY);
  }

  private void setupMapView(Bundle savedInstanceState) {
    try {
      MapboxMapOptions mapboxMapOptions = new MapboxMapOptions();
      mapboxMapOptions.styleJson(ResourceUtils.readRawResource(this, R.raw.no_bg_style));
      mapboxMapOptions.translucentTextureSurface(true);
      mapboxMapOptions.textureMode(true);
      mapboxMapOptions.camera(new CameraPosition.Builder()
        .zoom(2)
        .target(new LatLng(48.507879, 8.363795))
        .build()
      );

      mapView = new MapView(this, mapboxMapOptions);
      mapView.onCreate(savedInstanceState);
      mapView.getMapAsync(map -> mapboxMap = map);
      ((ViewGroup) findViewById(R.id.coordinator_layout)).addView(mapView);
    } catch (IOException exception) {
      Timber.e(exception);
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