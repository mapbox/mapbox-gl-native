package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.Manifest;
import android.content.pm.PackageManager;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class MyLocationToggleActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private FloatingActionButton locationToggleFab;
  private LocationServices locationServices;
  private LocationListener locationListener;

  private static final int PERMISSIONS_LOCATION = 0;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_my_location_toggle);

    Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
    setSupportActionBar(toolbar);

    locationServices = LocationServices.getLocationServices(MyLocationToggleActivity.this);

    final ActionBar actionBar = getSupportActionBar();
    if (actionBar != null) {
      actionBar.setDisplayShowTitleEnabled(true);
      actionBar.setDisplayHomeAsUpEnabled(true);
      actionBar.setDisplayShowHomeEnabled(true);
    }

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap map) {
        mapboxMap = map;
      }
    });

    locationToggleFab = (FloatingActionButton) findViewById(R.id.fabLocationToggle);
    locationToggleFab.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        if (mapboxMap != null) {
          toggleGps(!mapboxMap.isMyLocationEnabled());
        }
      }
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
    mapView.onDestroy();
    if (locationListener != null) {
      locationServices.removeLocationListener(locationListener);
    }
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
        return false;
    }
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
      Location lastLocation = locationServices.getLastLocation();
      if (lastLocation != null) {
        if (mapboxMap.getCameraPosition().zoom < 16) {
          mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(lastLocation), 16), 1000);
        } else {
          mapboxMap.easeCamera(CameraUpdateFactory.newLatLng(new LatLng(lastLocation)), 1000);
        }
      }

      locationListener = new LocationListener() {
        @Override
        public void onLocationChanged(Location location) {
          if (location != null) {
            if (mapboxMap.getCameraPosition().zoom < 16) {
              mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), 16), 1000);
            } else {
              mapboxMap.easeCamera(CameraUpdateFactory.newLatLng(new LatLng(location)), 1000);
            }
            locationServices.removeLocationListener(this);
          }
        }
      };
      locationServices.addLocationListener(locationListener);
      locationToggleFab.setImageResource(R.drawable.ic_location_disabled_24dp);
    } else {
      locationToggleFab.setImageResource(R.drawable.ic_my_location_24dp);
    }

    mapboxMap.setMyLocationEnabled(enabled);
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    if (requestCode == PERMISSIONS_LOCATION) {
      if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
        enableLocation(true);
      }
    }
  }

}
