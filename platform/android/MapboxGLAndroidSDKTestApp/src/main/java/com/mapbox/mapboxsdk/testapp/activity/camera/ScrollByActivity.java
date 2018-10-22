package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.StringRes;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.widget.SeekBar;
import android.widget.TextView;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using the scrollBy Camera API by moving x,y pixels above Grenada, Spain.
 */
public class ScrollByActivity extends AppCompatActivity implements OnMapReadyCallback {

  public static final int MULTIPLIER_PER_PIXEL = 50;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private SeekBar seekBarX;
  private SeekBar seekBarY;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_scroll_by);

    Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
    setSupportActionBar(toolbar);

    ActionBar actionBar = getSupportActionBar();
    if (actionBar != null) {
      actionBar.setDisplayHomeAsUpEnabled(true);
      actionBar.setDisplayShowHomeEnabled(true);
    }

    seekBarX = (SeekBar) findViewById(R.id.seekbar_move_x);
    TextView textViewX = (TextView) findViewById(R.id.textview_x);
    seekBarX.setOnSeekBarChangeListener(new PixelBarChangeListener(textViewX, R.string.scrollby_x_value));

    seekBarY = (SeekBar) findViewById(R.id.seekbar_move_y);
    TextView textViewY = (TextView) findViewById(R.id.textview_y);
    seekBarY.setOnSeekBarChangeListener(new PixelBarChangeListener(textViewY, R.string.scrollby_y_value));

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.setTag(true);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(MapboxMap map) {
    mapboxMap = map;
    UiSettings uiSettings = mapboxMap.getUiSettings();
    uiSettings.setLogoEnabled(false);
    uiSettings.setAttributionEnabled(false);

    FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
    fab.setColorFilter(ContextCompat.getColor(ScrollByActivity.this, R.color.primary));
    fab.setOnClickListener(view -> mapboxMap.easeCamera(CameraUpdateFactory.scrollBy(
      seekBarX.getProgress() * MULTIPLIER_PER_PIXEL,
      seekBarY.getProgress() * MULTIPLIER_PER_PIXEL)
    ));
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

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case android.R.id.home:
        onBackPressed();
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }

  private static class PixelBarChangeListener implements SeekBar.OnSeekBarChangeListener {

    @StringRes
    private int prefixTextResource;
    private TextView valueView;

    public PixelBarChangeListener(@NonNull TextView textView, @StringRes int textRes) {
      valueView = textView;
      prefixTextResource = textRes;
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
      int value = progress * ScrollByActivity.MULTIPLIER_PER_PIXEL;
      valueView.setText(String.format(seekBar.getResources().getString(prefixTextResource), value));
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }
  }
}
