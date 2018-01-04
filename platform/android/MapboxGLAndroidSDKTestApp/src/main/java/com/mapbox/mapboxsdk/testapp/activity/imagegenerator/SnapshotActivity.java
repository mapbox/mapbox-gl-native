package com.mapbox.mapboxsdk.testapp.activity.imagegenerator;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.Locale;

/**
 * Test activity showcasing the Snapshot API to create and display a bitmap of the current shown Map.
 */
public class SnapshotActivity extends AppCompatActivity implements OnMapReadyCallback, View.OnClickListener {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_snapshot);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(MapboxMap map) {
    mapboxMap = map;
    mapboxMap.setStyleUrl(Style.OUTDOORS);
    FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
    if (fab != null) {
      fab.setColorFilter(ContextCompat.getColor(SnapshotActivity.this, R.color.primary));
      fab.setOnClickListener(this);
    }
  }

  @Override
  public void onClick(View view) {
    final long startTime = System.nanoTime();
    mapboxMap.snapshot(snapshot -> {
      long endTime = System.nanoTime();
      long duration = (long) ((endTime - startTime) / 1e6);
      ImageView snapshotView = (ImageView) findViewById(R.id.imageView);
      snapshotView.setImageBitmap(snapshot);
      Toast.makeText(
        SnapshotActivity.this,
        String.format(Locale.getDefault(), "Snapshot taken in %d ms", duration),
        Toast.LENGTH_LONG).show();
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
  public void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  public void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }
}
