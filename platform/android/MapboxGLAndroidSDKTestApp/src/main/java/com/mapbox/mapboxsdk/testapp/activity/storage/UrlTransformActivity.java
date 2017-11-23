package com.mapbox.mapboxsdk.testapp.activity.storage;

import android.app.Activity;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.storage.Resource;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test activity showcasing the url transform
 */
public class UrlTransformActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  /**
   * Be sure to use an isolated class so the activity is not leaked when
   * the activity goes out of scope (static class in this case).
   * <p>
   * Alternatively, unregister the callback in {@link Activity#onDestroy()}
   */
  private static final class Transform implements FileSource.ResourceTransformCallback {
    @Override
    public String onURL(@Resource.Kind int kind, String url) {
      Timber.i("[%s] Could be rewriting %s", Thread.currentThread().getName(), url);
      return url;
    }
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_data_driven_style);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);

    // Get a handle to the file source and set the resource transform
    FileSource.getInstance(UrlTransformActivity.this).setResourceTransform(new Transform());

    mapView.getMapAsync(map -> {
      Timber.i("Map loaded");
      mapboxMap = map;
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
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();

    // Example of how to reset the transform callback
    FileSource.getInstance(UrlTransformActivity.this).setResourceTransform(null);

    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
