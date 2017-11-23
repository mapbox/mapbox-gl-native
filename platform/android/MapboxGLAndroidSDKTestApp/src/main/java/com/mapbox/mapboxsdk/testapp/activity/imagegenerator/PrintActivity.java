package com.mapbox.mapboxsdk.testapp.activity.imagegenerator;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.v4.print.PrintHelper;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using the Snaphot API to print a Map.
 */
public class PrintActivity extends AppCompatActivity implements MapboxMap.SnapshotReadyCallback {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_print);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> PrintActivity.this.mapboxMap = mapboxMap);

    final View fab = findViewById(R.id.fab);
    if (fab != null) {
      fab.setOnClickListener(view -> {
        if (mapboxMap != null) {
          mapboxMap.snapshot(PrintActivity.this);
        }
      });
    }
  }

  @Override
  public void onSnapshotReady(Bitmap snapshot) {
    PrintHelper photoPrinter = new PrintHelper(this);
    photoPrinter.setScaleMode(PrintHelper.SCALE_MODE_FIT);
    photoPrinter.printBitmap("map.jpg - mapbox print job", snapshot);
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
