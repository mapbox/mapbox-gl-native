package com.mapbox.mapboxsdk.testapp.activity.location;

import android.annotation.SuppressLint;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.android.core.permissions.PermissionsListener;
import com.mapbox.android.core.permissions.PermissionsManager;
import com.mapbox.mapboxsdk.location.LocationComponent;
import com.mapbox.mapboxsdk.location.LocationComponentActivationOptions;
import com.mapbox.mapboxsdk.location.LocationComponentOptions;
import com.mapbox.mapboxsdk.location.modes.CameraMode;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;

public class BasicLocationPulsingCircleActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final String SAVED_STATE_LOCATION = "saved_state_location";
  private static final String TAG = "Mbgl-BasicLocationPulsingCircleActivity";

  private Location lastLocation;
  private MapView mapView;
  private PermissionsManager permissionsManager;
  private LocationComponent locationComponent;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_location_layer_basic_pulsing_circle);

    mapView = findViewById(R.id.mapView);

    if (savedInstanceState != null) {
      lastLocation = savedInstanceState.getParcelable(SAVED_STATE_LOCATION);
    }
    mapView.onCreate(savedInstanceState);

    checkPermissions();
  }

  @SuppressLint("MissingPermission")
  @Override
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;

    mapboxMap.setStyle(Style.MAPBOX_STREETS, style -> {
      locationComponent = mapboxMap.getLocationComponent();

      LocationComponentOptions locationComponentOptions =
          LocationComponentOptions.builder(BasicLocationPulsingCircleActivity.this)
          .pulsingCircleEnabled(true)
          .build();

      LocationComponentActivationOptions locationComponentActivationOptions =
        buildLocationComponentActivationOptions(style,locationComponentOptions);

      locationComponent.activateLocationComponent(locationComponentActivationOptions);
      locationComponent.setLocationComponentEnabled(true);
      locationComponent.setCameraMode(CameraMode.TRACKING);
      locationComponent.forceLocationUpdate(lastLocation);
    });
  }

  private LocationComponentActivationOptions buildLocationComponentActivationOptions(@NonNull Style style,
                                                                                     @NonNull LocationComponentOptions
                                                                                       locationComponentOptions) {
    return LocationComponentActivationOptions
      .builder(this, style)
      .locationComponentOptions(locationComponentOptions)
      .useDefaultLocationEngine(true)
      .locationEngineRequest(new LocationEngineRequest.Builder(750)
        .setFastestInterval(750)
        .setPriority(LocationEngineRequest.PRIORITY_HIGH_ACCURACY)
        .build())
      .build();
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_pulsing_location_mode, menu);
    return true;
  }

  @SuppressLint("MissingPermission")
  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if (locationComponent == null) {
      return super.onOptionsItemSelected(item);
    }

    int id = item.getItemId();
    if (id == R.id.action_map_style_change) {
      toggleMapStyle();
      return true;
    } else if (id == R.id.action_component_disable) {
      locationComponent.setLocationComponentEnabled(false);
      return true;
    } else if (id == R.id.action_component_enabled) {
      locationComponent.setLocationComponentEnabled(true);
      return true;
    } else if (id == R.id.action_cancel_pulsing) {
      locationComponent.cancelPulsingLocationCircle();
      return true;
    }
    return super.onOptionsItemSelected(item);
  }

  private void toggleMapStyle() {
    if (locationComponent == null) {
      return;
    }

    mapboxMap.setStyle(Style.LIGHT);
  }

  private void checkPermissions() {
    if (PermissionsManager.areLocationPermissionsGranted(this)) {
      mapView.getMapAsync(this);
    } else {
      permissionsManager = new PermissionsManager(new PermissionsListener() {
        @Override
        public void onExplanationNeeded(List<String> permissionsToExplain) {
          Toast.makeText(BasicLocationPulsingCircleActivity.this, "You need to accept location permissions.",
            Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onPermissionResult(boolean granted) {
          if (granted) {
            mapView.getMapAsync(BasicLocationPulsingCircleActivity.this);
          } else {
            finish();
          }
        }
      });
      permissionsManager.requestLocationPermissions(this);
    }
  }



  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    permissionsManager.onRequestPermissionsResult(requestCode, permissions, grantResults);
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

  @SuppressLint("MissingPermission")
  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
    if (locationComponent != null) {
      outState.putParcelable(SAVED_STATE_LOCATION, locationComponent.getLastKnownLocation());
    }
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