package com.mapbox.mapboxsdk.testapp.activity.location;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.ListPopupWindow;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Toast;

import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.android.core.permissions.PermissionsListener;
import com.mapbox.android.core.permissions.PermissionsManager;
import com.mapbox.mapboxsdk.location.LocationComponent;
import com.mapbox.mapboxsdk.location.LocationComponentActivationOptions;
import com.mapbox.mapboxsdk.location.LocationComponentOptions;
import com.mapbox.mapboxsdk.location.modes.CameraMode;
import com.mapbox.mapboxsdk.location.modes.PulseMode;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

/**
 * This activity shows how to customize the LocationComponent's pulsing circle.
 */
public class CustomizedLocationPulsingCircleActivity extends AppCompatActivity implements OnMapReadyCallback {

  //region

  // Adjust these variables to customize the example's pulsing circle UI
  private static final float DEFAULT_LOCATION_CIRCLE_PULSE_DURATION_MS = 2300;
  private static final float SECOND_LOCATION_CIRCLE_PULSE_DURATION_MS = 800;
  private static final float THIRD_LOCATION_CIRCLE_PULSE_DURATION_MS = 8000;
  private static final float DEFAULT_LOCATION_CIRCLE_PULSE_RADIUS = 35;
  private static final float DEFAULT_LOCATION_CIRCLE_PULSE_ALPHA = .55f;
  private static final String DEFAULT_LOCATION_CIRCLE_INTERPOLATOR_PULSE_MODE = PulseMode.DECELERATE;
  private static final boolean DEFAULT_LOCATION_CIRCLE_PULSE_FADE_MODE = true;
  //endregion

  //region
  private static int LOCATION_CIRCLE_PULSE_COLOR;
  private static float LOCATION_CIRCLE_PULSE_DURATION = DEFAULT_LOCATION_CIRCLE_PULSE_DURATION_MS;
  private static final String SAVED_STATE_LOCATION = "saved_state_location";
  private static final String SAVED_STATE_LOCATION_CIRCLE_PULSE_COLOR = "saved_state_color";
  private static final String SAVED_STATE_LOCATION_CIRCLE_PULSE_DURATION = "saved_state_duration";
  private static final String TAG = "Mbgl-CustomizedLocationPulsingCircleActivity";

  private Location lastLocation;
  private MapView mapView;
  private Button pulsingCircleDurationButton;
  private Button pulsingCircleColorButton;
  private PermissionsManager permissionsManager;
  private LocationComponent locationComponent;
  private MapboxMap mapboxMap;
  private float currentPulseDuration;
  //endregion

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_location_layer_customized_pulsing_circle);

    LOCATION_CIRCLE_PULSE_COLOR = Color.BLUE;

    mapView = findViewById(R.id.mapView);

    if (savedInstanceState != null) {
      lastLocation = savedInstanceState.getParcelable(SAVED_STATE_LOCATION);
      LOCATION_CIRCLE_PULSE_COLOR = savedInstanceState.getInt(SAVED_STATE_LOCATION_CIRCLE_PULSE_COLOR);
      LOCATION_CIRCLE_PULSE_DURATION = savedInstanceState.getFloat(SAVED_STATE_LOCATION_CIRCLE_PULSE_DURATION);
    }

    pulsingCircleDurationButton = findViewById(R.id.button_location_circle_duration);
    pulsingCircleDurationButton.setText(String.format("%sms",
      String.valueOf(LOCATION_CIRCLE_PULSE_DURATION)));
    pulsingCircleDurationButton.setOnClickListener(v -> {
      if (locationComponent == null) {
        return;
      }
      showDurationListDialog();
    });

    pulsingCircleColorButton = findViewById(R.id.button_location_circle_color);
    pulsingCircleColorButton.setOnClickListener(v -> {
      if (locationComponent == null) {
        return;
      }
      showColorListDialog();
    });

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
        buildLocationComponentOptions(
          "waterway-label",
          LOCATION_CIRCLE_PULSE_COLOR,
          DEFAULT_LOCATION_CIRCLE_PULSE_ALPHA,
          LOCATION_CIRCLE_PULSE_DURATION,
          DEFAULT_LOCATION_CIRCLE_PULSE_RADIUS);

      LocationComponentActivationOptions locationComponentActivationOptions =
        buildLocationComponentActivationOptions(style,locationComponentOptions);

      locationComponent.activateLocationComponent(locationComponentActivationOptions);
      locationComponent.setLocationComponentEnabled(true);
      locationComponent.setCameraMode(CameraMode.TRACKING);
      locationComponent.forceLocationUpdate(lastLocation);
    });
  }

  private LocationComponentOptions buildLocationComponentOptions(@Nullable String idOfLayerBelow,
                                                                 @Nullable int pulsingCircleColor,
                                                                 @Nullable float pulsingCircleAlpha,
                                                                 @Nullable float pulsingCircleDuration,
                                                                 @Nullable float pulsingCircleMaxRadius
  ) {
    currentPulseDuration = pulsingCircleDuration;
    return LocationComponentOptions.builder(this)
      .layerBelow(idOfLayerBelow)
      .pulsingCircleEnabled(true)
      .pulsingCircleFadeEnabled(DEFAULT_LOCATION_CIRCLE_PULSE_FADE_MODE)
      .pulsingCircleInterpolator(DEFAULT_LOCATION_CIRCLE_INTERPOLATOR_PULSE_MODE)
      .pulsingCircleColor(pulsingCircleColor)
      .pulsingCircleAlpha(pulsingCircleAlpha)
      .pulsingCircleDuration(pulsingCircleDuration)
      .pulsingCircleMaxRadius(pulsingCircleMaxRadius)
      .build();
  }

  @SuppressLint("MissingPermission")
  private void setNewLocationComponentOptions(@Nullable float newPulsingDuration,
                                              @Nullable int newPulsingColor) {
    mapboxMap.getStyle(new Style.OnStyleLoaded() {
      @Override
      public void onStyleLoaded(@NonNull Style style) {
        locationComponent.applyStyle(
            buildLocationComponentOptions(
                "waterway-label",
                newPulsingColor, DEFAULT_LOCATION_CIRCLE_PULSE_ALPHA,
                newPulsingDuration,
                DEFAULT_LOCATION_CIRCLE_PULSE_RADIUS));
      }
    });
  }

  private LocationComponentActivationOptions buildLocationComponentActivationOptions(
      @NonNull Style style,
      @NonNull LocationComponentOptions locationComponentOptions) {
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
          Toast.makeText(CustomizedLocationPulsingCircleActivity.this, "You need to accept location permissions.",
            Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onPermissionResult(boolean granted) {
          if (granted) {
            mapView.getMapAsync(CustomizedLocationPulsingCircleActivity.this);
          } else {
            finish();
          }
        }
      });
      permissionsManager.requestLocationPermissions(this);
    }
  }

  private void showDurationListDialog() {
    List<String> modes = new ArrayList<>();
    modes.add(String.format("%sms", String.valueOf(DEFAULT_LOCATION_CIRCLE_PULSE_DURATION_MS)));
    modes.add(String.format("%sms", String.valueOf(SECOND_LOCATION_CIRCLE_PULSE_DURATION_MS)));
    modes.add(String.format("%sms", String.valueOf(THIRD_LOCATION_CIRCLE_PULSE_DURATION_MS)));
    ArrayAdapter<String> profileAdapter = new ArrayAdapter<>(this,
      android.R.layout.simple_list_item_1, modes);
    ListPopupWindow listPopup = new ListPopupWindow(this);
    listPopup.setAdapter(profileAdapter);
    listPopup.setAnchorView(pulsingCircleDurationButton);
    listPopup.setOnItemClickListener((parent, itemView, position, id) -> {
      String selectedMode = modes.get(position);
      pulsingCircleDurationButton.setText(selectedMode);
      if (selectedMode.contentEquals(String.format("%sms",
        String.valueOf(DEFAULT_LOCATION_CIRCLE_PULSE_DURATION_MS)))) {
        LOCATION_CIRCLE_PULSE_DURATION = DEFAULT_LOCATION_CIRCLE_PULSE_DURATION_MS;
        setNewLocationComponentOptions(DEFAULT_LOCATION_CIRCLE_PULSE_DURATION_MS, LOCATION_CIRCLE_PULSE_COLOR);
      } else if (selectedMode.contentEquals(String.format("%sms",
        String.valueOf(SECOND_LOCATION_CIRCLE_PULSE_DURATION_MS)))) {
        LOCATION_CIRCLE_PULSE_DURATION = SECOND_LOCATION_CIRCLE_PULSE_DURATION_MS;
        setNewLocationComponentOptions(SECOND_LOCATION_CIRCLE_PULSE_DURATION_MS, LOCATION_CIRCLE_PULSE_COLOR);
      } else if (selectedMode.contentEquals(String.format("%sms",
        String.valueOf(THIRD_LOCATION_CIRCLE_PULSE_DURATION_MS)))) {
        LOCATION_CIRCLE_PULSE_DURATION = THIRD_LOCATION_CIRCLE_PULSE_DURATION_MS;
        setNewLocationComponentOptions(THIRD_LOCATION_CIRCLE_PULSE_DURATION_MS, LOCATION_CIRCLE_PULSE_COLOR);
      }
      listPopup.dismiss();
    });
    listPopup.show();
  }

  private void showColorListDialog() {
    List<String> trackingTypes = new ArrayList<>();
    trackingTypes.add("Blue");
    trackingTypes.add("Red");
    trackingTypes.add("Green");
    trackingTypes.add("Gray");
    ArrayAdapter<String> profileAdapter = new ArrayAdapter<>(this,
      android.R.layout.simple_list_item_1, trackingTypes);
    ListPopupWindow listPopup = new ListPopupWindow(this);
    listPopup.setAdapter(profileAdapter);
    listPopup.setAnchorView(pulsingCircleColorButton);
    listPopup.setOnItemClickListener((parent, itemView, position, id) -> {
      String selectedTrackingType = trackingTypes.get(position);
      pulsingCircleColorButton.setText(selectedTrackingType);
      if (selectedTrackingType.contentEquals("Blue")) {
        LOCATION_CIRCLE_PULSE_COLOR = Color.BLUE;
        setNewLocationComponentOptions(currentPulseDuration, Color.BLUE);
      } else if (selectedTrackingType.contentEquals("Red")) {
        LOCATION_CIRCLE_PULSE_COLOR = Color.RED;
        setNewLocationComponentOptions(currentPulseDuration, Color.RED);
      } else if (selectedTrackingType.contentEquals("Green")) {
        LOCATION_CIRCLE_PULSE_COLOR = Color.GREEN;
        setNewLocationComponentOptions(currentPulseDuration, Color.GREEN);
      } else if (selectedTrackingType.contentEquals("Gray")) {
        LOCATION_CIRCLE_PULSE_COLOR = Color.parseColor("#4a4a4a");
        setNewLocationComponentOptions(currentPulseDuration, Color.parseColor("#4a4a4a"));
      }
      listPopup.dismiss();
    });
    listPopup.show();
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
      outState.putInt(SAVED_STATE_LOCATION_CIRCLE_PULSE_COLOR, LOCATION_CIRCLE_PULSE_COLOR);
      outState.putFloat(SAVED_STATE_LOCATION_CIRCLE_PULSE_DURATION, LOCATION_CIRCLE_PULSE_DURATION);
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