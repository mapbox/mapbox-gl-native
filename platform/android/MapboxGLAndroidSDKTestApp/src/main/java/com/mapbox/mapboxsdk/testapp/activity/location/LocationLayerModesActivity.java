package com.mapbox.mapboxsdk.testapp.activity.location;

import android.annotation.SuppressLint;
import android.content.res.Configuration;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.ListPopupWindow;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Toast;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineListener;
import com.mapbox.android.core.location.LocationEnginePriority;
import com.mapbox.android.core.location.LocationEngineProvider;
import com.mapbox.android.core.permissions.PermissionsListener;
import com.mapbox.android.core.permissions.PermissionsManager;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerOptions;
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerPlugin;
import com.mapbox.mapboxsdk.plugins.locationlayer.OnCameraTrackingChangedListener;
import com.mapbox.mapboxsdk.plugins.locationlayer.OnLocationLayerClickListener;
import com.mapbox.mapboxsdk.plugins.locationlayer.modes.CameraMode;
import com.mapbox.mapboxsdk.plugins.locationlayer.modes.RenderMode;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

public class LocationLayerModesActivity extends AppCompatActivity implements OnMapReadyCallback,
  LocationEngineListener, OnLocationLayerClickListener, OnCameraTrackingChangedListener {

  private MapView mapView;
  private Button locationModeBtn;
  private Button locationTrackingBtn;

  private PermissionsManager permissionsManager;

  private LocationLayerPlugin locationLayerPlugin;
  private LocationEngine locationEngine;
  private MapboxMap mapboxMap;
  private boolean customStyle;

  private static final String SAVED_STATE_CAMERA = "saved_state_camera";
  private static final String SAVED_STATE_RENDER = "saved_state_render";

  @CameraMode.Mode
  private int cameraMode = CameraMode.TRACKING;

  @RenderMode.Mode
  private int renderMode = RenderMode.NORMAL;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_location_layer_mode);

    mapView = findViewById(R.id.mapView);

    locationModeBtn = findViewById(R.id.button_location_mode);
    locationModeBtn.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        if (locationLayerPlugin == null) {
          return;
        }
        showModeListDialog();
      }
    });

    locationTrackingBtn = findViewById(R.id.button_location_tracking);
    locationTrackingBtn.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        if (locationLayerPlugin == null) {
          return;
        }
        showTrackingListDialog();
      }
    });


    if (savedInstanceState != null) {
      cameraMode = savedInstanceState.getInt(SAVED_STATE_CAMERA);
      renderMode = savedInstanceState.getInt(SAVED_STATE_RENDER);
    }

    mapView.onCreate(savedInstanceState);

    if (PermissionsManager.areLocationPermissionsGranted(this)) {
      mapView.getMapAsync(this);
    } else {
      permissionsManager = new PermissionsManager(new PermissionsListener() {
        @Override
        public void onExplanationNeeded(List<String> permissionsToExplain) {
          Toast.makeText(LocationLayerModesActivity.this, "You need to accept location permissions.",
            Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onPermissionResult(boolean granted) {
          if (granted) {
            mapView.getMapAsync(LocationLayerModesActivity.this);
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

  @SuppressLint("MissingPermission")
  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;

    locationEngine = new LocationEngineProvider(this).obtainBestLocationEngineAvailable();
    locationEngine.setPriority(LocationEnginePriority.HIGH_ACCURACY);
    locationEngine.setFastestInterval(1000);
    locationEngine.addLocationEngineListener(this);
    locationEngine.activate();

    locationLayerPlugin = mapboxMap.getLocationLayerPlugin();
    locationLayerPlugin.addOnLocationClickListener(this);
    locationLayerPlugin.addOnCameraTrackingChangedListener(this);
    locationLayerPlugin.setCameraMode(cameraMode);
    setRendererMode(renderMode);
    activateLocationLayer();
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_location_mode, menu);
    return true;
  }

  @SuppressLint("MissingPermission")
  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if (locationLayerPlugin == null) {
      return super.onOptionsItemSelected(item);
    }

    int id = item.getItemId();
    if (id == R.id.action_style_change) {
      toggleStyle();
      return true;
    } else if (id == R.id.action_map_style_change) {
      toggleMapStyle();
      return true;
    } else if (id == R.id.action_plugin_disable) {
      locationLayerPlugin.deactivateLocationLayerPlugin();
      return true;
    } else if (id == R.id.action_plugin_enabled) {
      activateLocationLayer();
      return true;
    }

    return super.onOptionsItemSelected(item);
  }

  private void activateLocationLayer() {
    if (locationLayerPlugin != null) {
      int[] padding;
      if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
        padding = new int[] {0, 750, 0, 0};
      } else {
        padding = new int[] {0, 250, 0, 0};
      }

      LocationLayerOptions options = LocationLayerOptions.builder(this)
        .padding(padding)
        .layerBelow("waterway-label")
        .build();

      locationLayerPlugin.activateLocationLayerPlugin(locationEngine, options);
    }
  }

  public void toggleStyle() {
    customStyle = !customStyle;
    locationLayerPlugin.applyStyle(
      this,
      customStyle ? R.style.CustomLocationLayer : R.style.mapbox_LocationLayer);
  }

  public void toggleMapStyle() {
    String styleUrl = mapboxMap.getStyleUrl().contentEquals(Style.DARK) ? Style.LIGHT : Style.DARK;
    mapboxMap.setStyle(styleUrl);
  }

  public LocationLayerPlugin getLocationLayerPlugin() {
    return locationLayerPlugin;
  }

  @Override
  @SuppressWarnings( {"MissingPermission"})
  protected void onStart() {
    super.onStart();
    mapView.onStart();
    if (locationEngine != null) {
      locationEngine.addLocationEngineListener(this);
      if (locationEngine.isConnected()) {
        locationEngine.requestLocationUpdates();
      } else {
        locationEngine.activate();
      }
    }
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
    if (locationEngine != null) {
      locationEngine.removeLocationEngineListener(this);
      locationEngine.removeLocationUpdates();
    }
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
    outState.putInt(SAVED_STATE_CAMERA, cameraMode);
    outState.putInt(SAVED_STATE_RENDER, renderMode);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
    if (locationEngine != null) {
      locationEngine.deactivate();
    }
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  @SuppressWarnings( {"MissingPermission"})
  public void onConnected() {
    locationEngine.requestLocationUpdates();
  }

  @Override
  public void onLocationChanged(Location location) {
    // no impl
  }

  @Override
  public void onLocationLayerClick() {
    Toast.makeText(this, "OnLocationLayerClick", Toast.LENGTH_LONG).show();
  }

  private void showModeListDialog() {
    List<String> modes = new ArrayList<>();
    modes.add("Normal");
    modes.add("Compass");
    modes.add("GPS");
    ArrayAdapter<String> profileAdapter = new ArrayAdapter<>(this,
      android.R.layout.simple_list_item_1, modes);
    ListPopupWindow listPopup = new ListPopupWindow(this);
    listPopup.setAdapter(profileAdapter);
    listPopup.setAnchorView(locationModeBtn);
    listPopup.setOnItemClickListener((parent, itemView, position, id) -> {
      String selectedMode = modes.get(position);
      locationModeBtn.setText(selectedMode);
      if (selectedMode.contentEquals("Normal")) {
        setRendererMode(RenderMode.NORMAL);
      } else if (selectedMode.contentEquals("Compass")) {
        setRendererMode(RenderMode.COMPASS);
      } else if (selectedMode.contentEquals("GPS")) {
        setRendererMode(RenderMode.GPS);
      }
      listPopup.dismiss();
    });
    listPopup.show();
  }

  private void setRendererMode(@RenderMode.Mode int mode) {
    renderMode = mode;
    locationLayerPlugin.setRenderMode(mode);
    if (mode == RenderMode.NORMAL) {
      locationModeBtn.setText("Normal");
    } else if (mode == RenderMode.COMPASS) {
      locationModeBtn.setText("Compass");
    } else if (mode == RenderMode.GPS) {
      locationModeBtn.setText("Gps");
    }
  }

  private void showTrackingListDialog() {
    List<String> trackingTypes = new ArrayList<>();
    trackingTypes.add("None");
    trackingTypes.add("Tracking");
    trackingTypes.add("Tracking Compass");
    trackingTypes.add("Tracking GPS");
    trackingTypes.add("Tracking GPS North");
    ArrayAdapter<String> profileAdapter = new ArrayAdapter<>(this,
      android.R.layout.simple_list_item_1, trackingTypes);
    ListPopupWindow listPopup = new ListPopupWindow(this);
    listPopup.setAdapter(profileAdapter);
    listPopup.setAnchorView(locationTrackingBtn);
    listPopup.setOnItemClickListener((parent, itemView, position, id) -> {
      String selectedTrackingType = trackingTypes.get(position);
      locationTrackingBtn.setText(selectedTrackingType);
      if (selectedTrackingType.contentEquals("None")) {
        locationLayerPlugin.setCameraMode(CameraMode.NONE);
      } else if (selectedTrackingType.contentEquals("Tracking")) {
        locationLayerPlugin.setCameraMode(CameraMode.TRACKING);
      } else if (selectedTrackingType.contentEquals("Tracking Compass")) {
        locationLayerPlugin.setCameraMode(CameraMode.TRACKING_COMPASS);
      } else if (selectedTrackingType.contentEquals("Tracking GPS")) {
        locationLayerPlugin.setCameraMode(CameraMode.TRACKING_GPS);
      } else if (selectedTrackingType.contentEquals("Tracking GPS North")) {
        locationLayerPlugin.setCameraMode(CameraMode.TRACKING_GPS_NORTH);
      }
      listPopup.dismiss();

      if (locationLayerPlugin.getCameraMode() != CameraMode.NONE) {
        locationLayerPlugin.zoomWhileTracking(15, 750, new MapboxMap.CancelableCallback() {
          @Override
          public void onCancel() {
            // No impl
          }

          @Override
          public void onFinish() {
            locationLayerPlugin.tiltWhileTracking(45);
          }
        });
      } else {
        mapboxMap.easeCamera(CameraUpdateFactory.tiltTo(0));
      }
    });
    listPopup.show();
  }

  @Override
  public void onCameraTrackingDismissed() {
    locationTrackingBtn.setText("None");
  }

  @Override
  public void onCameraTrackingChanged(int currentMode) {
    this.cameraMode = currentMode;

    if (cameraMode == CameraMode.NONE) {
      locationTrackingBtn.setText("None");
    } else if (cameraMode == CameraMode.TRACKING) {
      locationTrackingBtn.setText("Tracking");
    } else if (cameraMode == CameraMode.TRACKING_COMPASS) {
      locationTrackingBtn.setText("Tracking Compass");
    } else if (cameraMode == CameraMode.TRACKING_GPS) {
      locationTrackingBtn.setText("Tracking GPS");
    } else if (cameraMode == CameraMode.TRACKING_GPS_NORTH) {
      locationTrackingBtn.setText("Tracking GPS North");
    }
  }
}