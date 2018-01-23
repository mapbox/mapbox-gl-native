package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.Toast;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.android.core.location.LocationEngineListener;

import timber.log.Timber;

/**
 * Test activity showcasing the different tracking modes the SDK exposes.
 * <p>
 * This includes MyLocationTracking/MyLocationBearingTracking and how the components can be configured to be dismissed
 * using gesture configurations.
 * </p>
 */
public class MyLocationTrackingModeActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener,
  OnMapReadyCallback, LocationEngineListener {

  public static final int TRACKING_NONE_INDEX = 0;
  public static final int TRACKING_FOLLOW_INDEX = 1;
  public static final int BEARING_NONE_INDEX = 0;
  public static final int BEARING_GPS_INDEX = 1;
  public static final int BEARING_COMPASS_INDEX = 2;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private Spinner locationSpinner;
  private Spinner bearingSpinner;
  private boolean firstRun = true;

  private MenuItem dismissLocationTrackingOnGestureItem;
  private MenuItem dismissBearingTrackingOnGestureItem;
  private MenuItem enableRotateGesturesItem;
  private MenuItem enableScrollGesturesItem;

  @Override
  protected void onCreate(final Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_my_location_tracking);
    setupToolbar();

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    MyLocationTrackingModeActivity.this.mapboxMap = mapboxMap;

    mapboxMap.setMyLocationEnabled(true);
    Mapbox.getLocationEngine().addLocationEngineListener(this);
    Mapbox.getLocationEngine().requestLocationUpdates();
  }

  @Override
  public void onConnected() {
    // Nothing
  }

  @Override
  public void onLocationChanged(Location location) {
    Timber.e("Location changed %s", location);
    if (firstRun) {
      setInitialLocation(location, 16);
    }
  }

  private void setInitialLocation(Location location, double zoom) {
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), zoom));
    mapboxMap.setMyLocationEnabled(true);
    setupSpinners(mapboxMap);
    firstRun = false;
  }

  private void setupToolbar() {
    Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
    setSupportActionBar(toolbar);

    final ActionBar actionBar = getSupportActionBar();
    if (actionBar != null) {
      actionBar.setDisplayShowTitleEnabled(false);
      actionBar.setDisplayHomeAsUpEnabled(true);
      actionBar.setDisplayShowHomeEnabled(true);

      locationSpinner = (Spinner) findViewById(R.id.spinner_location);
      ArrayAdapter<CharSequence> locationTrackingAdapter = ArrayAdapter.createFromResource(
        actionBar.getThemedContext(), R.array.user_tracking_mode, android.R.layout.simple_spinner_item);
      locationTrackingAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
      locationSpinner.setAdapter(locationTrackingAdapter);

      bearingSpinner = (Spinner) findViewById(R.id.spinner_bearing);
      ArrayAdapter<CharSequence> bearingTrackingAdapter = ArrayAdapter.createFromResource(
        actionBar.getThemedContext(), R.array.user_bearing_mode, android.R.layout.simple_spinner_item);
      bearingTrackingAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
      bearingSpinner.setAdapter(bearingTrackingAdapter);
    }
  }

  private void setupSpinners(@NonNull MapboxMap mapboxMap) {
    locationSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);
    bearingSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);
    setCheckBoxes();

    mapboxMap.setOnMyLocationTrackingModeChangeListener(myLocationTrackingMode -> {
      locationSpinner.setOnItemSelectedListener(null);
      switch (myLocationTrackingMode) {
        case MyLocationTracking.TRACKING_NONE:
          locationSpinner.setSelection(TRACKING_NONE_INDEX);
          break;
        case MyLocationTracking.TRACKING_FOLLOW:
          locationSpinner.setSelection(TRACKING_FOLLOW_INDEX);
          break;
      }
      locationSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);
    });

    mapboxMap.setOnMyBearingTrackingModeChangeListener(myBearingTrackingMode -> {
      bearingSpinner.setOnItemSelectedListener(null);
      switch (myBearingTrackingMode) {
        case MyBearingTracking.NONE:
          bearingSpinner.setSelection(BEARING_NONE_INDEX);
          break;

        case MyBearingTracking.GPS:
          bearingSpinner.setSelection(BEARING_GPS_INDEX);
          break;

        case MyBearingTracking.COMPASS:
          bearingSpinner.setSelection(BEARING_COMPASS_INDEX);
          break;
      }
      bearingSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);
    });
  }

  @Override
  public void onItemSelected(AdapterView<?> parent, View view, int position, long id) throws SecurityException {
    TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
    if (parent.getId() == R.id.spinner_location) {
      switch (position) {
        case TRACKING_NONE_INDEX:
          trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
          break;

        case TRACKING_FOLLOW_INDEX:
          trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
          break;
      }
    } else if (parent.getId() == R.id.spinner_bearing) {
      switch (position) {
        case BEARING_NONE_INDEX:
          trackingSettings.setMyBearingTrackingMode(MyBearingTracking.NONE);
          break;

        case BEARING_GPS_INDEX:
          trackingSettings.setMyBearingTrackingMode(MyBearingTracking.GPS);
          break;

        case BEARING_COMPASS_INDEX:
          trackingSettings.setMyBearingTrackingMode(MyBearingTracking.COMPASS);
          break;
      }
    }
  }

  @Override
  public void onNothingSelected(AdapterView<?> parent) {

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
    Mapbox.getLocationEngine().removeLocationEngineListener(this);
    Mapbox.getLocationEngine().removeLocationUpdates();
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
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_tracking, menu);
    dismissLocationTrackingOnGestureItem = menu.findItem(R.id.action_toggle_dismissible_location);
    dismissBearingTrackingOnGestureItem = menu.findItem(R.id.action_toggle_dismissible_bearing);
    enableRotateGesturesItem = menu.findItem(R.id.action_toggle_rotate_gesture_enabled);
    enableScrollGesturesItem = menu.findItem(R.id.action_toggle_scroll_gesture_enabled);
    setCheckBoxes();
    return true;
  }

  private void setCheckBoxes() {
    if (mapboxMap != null && dismissBearingTrackingOnGestureItem != null) {
      TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
      UiSettings uiSettings = mapboxMap.getUiSettings();
      dismissBearingTrackingOnGestureItem.setChecked(trackingSettings.isDismissBearingTrackingOnGesture());
      dismissLocationTrackingOnGestureItem.setChecked(trackingSettings.isDismissLocationTrackingOnGesture());
      enableRotateGesturesItem.setChecked(uiSettings.isRotateGesturesEnabled());
      enableScrollGesturesItem.setChecked(uiSettings.isScrollGesturesEnabled());
    }
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    boolean state;
    switch (item.getItemId()) {
      case android.R.id.home:
        onBackPressed();
        return true;
      case R.id.action_toggle_dismissible_location:
        state = !item.isChecked();
        mapboxMap.getTrackingSettings().setDismissLocationTrackingOnGesture(state);
        Toast.makeText(this, "Dismiss tracking mode on gesture = " + state, Toast.LENGTH_SHORT).show();
        item.setChecked(state);
        return true;
      case R.id.action_toggle_dismissible_bearing:
        state = !item.isChecked();
        mapboxMap.getTrackingSettings().setDismissBearingTrackingOnGesture(state);
        Toast.makeText(this, "Dismiss bearing mode on gesture = " + state, Toast.LENGTH_SHORT).show();
        item.setChecked(state);
        return true;
      case R.id.action_toggle_rotate_gesture_enabled:
        state = !item.isChecked();
        mapboxMap.getUiSettings().setRotateGesturesEnabled(state);
        Toast.makeText(this, "Rotate gesture enabled = " + state, Toast.LENGTH_SHORT).show();
        item.setChecked(state);
        return true;
      case R.id.action_toggle_scroll_gesture_enabled:
        state = !item.isChecked();
        mapboxMap.getUiSettings().setScrollGesturesEnabled(state);
        Toast.makeText(this, "Scroll gesture enabled = " + state, Toast.LENGTH_SHORT).show();
        item.setChecked(state);
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }
}
