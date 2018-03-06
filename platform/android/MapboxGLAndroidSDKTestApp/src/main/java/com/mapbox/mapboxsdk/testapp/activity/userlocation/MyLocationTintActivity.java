package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.app.Activity;
import android.graphics.Color;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.IdRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.android.core.location.LocationEngineListener;

/**
 * Test activity showcasing how to tint the MyLocationView.
 */
public class MyLocationTintActivity extends BaseLocationActivity implements LocationEngineListener {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private boolean firstRun;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_my_location_dot_color);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;

      // enable location updates
      toggleGps(true);

      // add some padding
      final MyLocationViewSettings myLocationViewSettings = mapboxMap.getMyLocationViewSettings();
      myLocationViewSettings.setPadding(0, 500, 0, 0);

      // enable tracking
      TrackingSettings settings = mapboxMap.getTrackingSettings();
      settings.setDismissLocationTrackingOnGesture(false);
      settings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);

      // handle default button clicks
      ViewUtils.attachClickListener(
        MyLocationTintActivity.this,
        R.id.default_user_dot_coloring_button,
        view -> {
          myLocationViewSettings.setAccuracyTintColor(ContextCompat.getColor(
            MyLocationTintActivity.this, R.color.mapbox_blue));
          myLocationViewSettings.setForegroundTintColor(ContextCompat.getColor(
            MyLocationTintActivity.this, R.color.mapbox_blue));
          myLocationViewSettings.setBackgroundTintColor(Color.WHITE);
        });

      // handle tint user dot button clicks
      ViewUtils.attachClickListener(
        MyLocationTintActivity.this,
        R.id.tint_user_dot_button,
        view -> {
          myLocationViewSettings.setAccuracyTintColor(
            ContextCompat.getColor(MyLocationTintActivity.this, R.color.mapboxGreen));
          myLocationViewSettings.setForegroundTintColor(
            ContextCompat.getColor(MyLocationTintActivity.this, R.color.mapboxGreen));
          myLocationViewSettings.setBackgroundTintColor(Color.WHITE);
        });

      // handle tint accuracy ring button clicks
      ViewUtils.attachClickListener(
        MyLocationTintActivity.this,
        R.id.user_accuracy_ring_tint_button,
        view -> {
          myLocationViewSettings.setAccuracyTintColor(
            ContextCompat.getColor(MyLocationTintActivity.this, R.color.accent));
          myLocationViewSettings.setForegroundTintColor(
            ContextCompat.getColor(MyLocationTintActivity.this, R.color.mapbox_blue));
          myLocationViewSettings.setBackgroundTintColor(Color.WHITE);
        });

      ViewUtils.attachClickListener(
        MyLocationTintActivity.this,
        R.id.user_dot_transparent_button,
        view -> {
          myLocationViewSettings.setForegroundTintColor(Color.TRANSPARENT);
          myLocationViewSettings.setBackgroundTintColor(Color.TRANSPARENT);
        }
      );
    });

  }

  @Override
  public void onConnected() {
    // Nothing
  }

  @Override
  public void onLocationChanged(Location location) {
    if (mapboxMap != null && firstRun) {
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), 15));
      firstRun = false;
    }
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  public void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  public void onPause() {
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

  @Override
  protected void enableLocation(boolean enabled) {
    if (enabled) {
      mapboxMap.setMyLocationEnabled(true);
      if (mapboxMap.getMyLocation() != null) {
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(
          new LatLng(mapboxMap.getMyLocation().getLatitude(),
            mapboxMap.getMyLocation().getLongitude()), 15));
      }
    } else {
      mapboxMap.setMyLocationEnabled(false);
    }
  }

  private static class ViewUtils {

    public static void attachClickListener(
      @NonNull Activity activity, @IdRes int buttonId, @Nullable View.OnClickListener clickListener) {
      View view = activity.findViewById(buttonId);
      if (view != null) {
        view.setOnClickListener(clickListener);
      }
    }
  }
}
