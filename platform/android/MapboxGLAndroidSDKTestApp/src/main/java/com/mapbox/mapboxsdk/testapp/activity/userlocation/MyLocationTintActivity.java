package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.IdRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class MyLocationTintActivity extends AppCompatActivity implements LocationListener {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private boolean firstRun;

  private static final int PERMISSIONS_LOCATION = 0;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_my_location_dot_color);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap map) {
        mapboxMap = map;

        // enable location updates
        toggleGps(!mapboxMap.isMyLocationEnabled());

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
          new View.OnClickListener() {
            @Override
            public void onClick(View view) {
              myLocationViewSettings.setAccuracyTintColor(ContextCompat.getColor(
                MyLocationTintActivity.this, R.color.mapbox_blue));
              myLocationViewSettings.setForegroundTintColor(ContextCompat.getColor(
                MyLocationTintActivity.this, R.color.mapbox_blue));
              myLocationViewSettings.setBackgroundTintColor(Color.WHITE);
            }
          });

        // handle tint user dot button clicks
        ViewUtils.attachClickListener(
          MyLocationTintActivity.this,
          R.id.tint_user_dot_button,
          new View.OnClickListener() {
            @Override
            public void onClick(View view) {
              myLocationViewSettings.setAccuracyTintColor(
                ContextCompat.getColor(MyLocationTintActivity.this, R.color.mapboxGreen));
              myLocationViewSettings.setForegroundTintColor(
                ContextCompat.getColor(MyLocationTintActivity.this, R.color.mapboxGreen));
              myLocationViewSettings.setBackgroundTintColor(Color.WHITE);
            }
          });

        // handle tint accuracy ring button clicks
        ViewUtils.attachClickListener(
          MyLocationTintActivity.this,
          R.id.user_accuracy_ring_tint_button,
          new View.OnClickListener() {
            @Override
            public void onClick(View view) {
              myLocationViewSettings.setAccuracyTintColor(
                ContextCompat.getColor(MyLocationTintActivity.this, R.color.accent));
              myLocationViewSettings.setForegroundTintColor(
                ContextCompat.getColor(MyLocationTintActivity.this, R.color.mapbox_blue));
              myLocationViewSettings.setBackgroundTintColor(Color.WHITE);
            }
          });

        ViewUtils.attachClickListener(
          MyLocationTintActivity.this,
          R.id.user_dot_transparent_button,
          new View.OnClickListener() {
            @Override
            public void onClick(View view) {
              myLocationViewSettings.setForegroundTintColor(Color.TRANSPARENT);
              myLocationViewSettings.setBackgroundTintColor(Color.TRANSPARENT);
            }
          }
        );
      }
    });

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
    LocationServices.getLocationServices(this).addLocationListener(this);
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
    LocationServices.getLocationServices(this).removeLocationListener(this);
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

  @UiThread
  public void toggleGps(boolean enableGps) {
    if (enableGps) {
      if ((ContextCompat.checkSelfPermission(this,
        Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED)
        || (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
        != PackageManager.PERMISSION_GRANTED)) {
        ActivityCompat.requestPermissions(this, new String[] {
          Manifest.permission.ACCESS_COARSE_LOCATION,
          Manifest.permission.ACCESS_FINE_LOCATION}, PERMISSIONS_LOCATION);
      } else {
        enableLocation(true);
      }
    } else {
      enableLocation(false);
    }
  }

  private void enableLocation(boolean enabled) {
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

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    if (requestCode == PERMISSIONS_LOCATION) {
      if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
        enableLocation(true);
      }
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
