package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.location.Location;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationSource;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.location.LocationEngineListener;

public class MyLocationToggleActivity extends BaseLocationActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private FloatingActionButton locationToggleFab;

  private LocationEngine locationServices;
  private LocationEngineListener locationListener;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_my_location_toggle);

    locationServices = LocationSource.getLocationEngine(this);

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
    // Ensure no memory leak occurs if we register the location listener but the call hasn't
    // been made yet.
    if (locationListener != null) {
      locationServices.removeLocationEngineListener(locationListener);
    }
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void enableLocation(boolean enabled) {
    if (enabled) {
      // To move the camera instantly, we attempt to get the last known location and either
      // ease or animate the camera to that position depending on the zoom level.
      Location lastLocation = LocationSource.getLocationEngine(this).getLastLocation();

      if (lastLocation != null) {
        if (mapboxMap.getCameraPosition().zoom > 15.99) {
          mapboxMap.easeCamera(CameraUpdateFactory.newLatLng(new LatLng(lastLocation)), 1000);
        } else {
          mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(lastLocation), 16), 1000);
        }
      } else {
        locationListener = new LocationEngineListener() {
          @Override
          public void onConnected() {
            // Nothing
          }

          @Override
          public void onLocationChanged(Location location) {
            if (location != null) {
              mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), 16));
              locationServices.removeLocationEngineListener(this);
            }
          }
        };
        locationServices.addLocationEngineListener(locationListener);
      }
      locationToggleFab.setImageResource(R.drawable.ic_location_disabled);
    } else {
      locationToggleFab.setImageResource(R.drawable.ic_my_location);
    }
    mapboxMap.setMyLocationEnabled(enabled);
  }
}
