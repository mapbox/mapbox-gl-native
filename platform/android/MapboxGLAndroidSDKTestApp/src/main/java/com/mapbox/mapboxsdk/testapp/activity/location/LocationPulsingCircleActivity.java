package com.mapbox.mapboxsdk.testapp.activity.location;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.location.Location;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
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

public class LocationPulsingCircleActivity extends AppCompatActivity implements OnMapReadyCallback {

  private MapView mapView;
  private Button pulsingCircleFrequencyButton;
  private Button pulsingCircleColorButton;

  private PermissionsManager permissionsManager;

  private LocationComponent locationComponent;
  private MapboxMap mapboxMap;
  private boolean defaultStyle = false;
  private float currentPulseFrequency;

  private static final String SAVED_STATE_LOCATION = "saved_state_location";

  // Make sure that the frequency value is equal to or larger than the duration value.
  private static final float DEFAULT_LOCATION_CIRCLE_PULSE_FREQUENCY = 1700;
  private static final float DEFAULT_LOCATION_CIRCLE_PULSE_DURATION = 1700;

  private static final float DEFAULT_LOCATION_CIRCLE_PULSE_ALPHA = .4f;
  private static final String DEFAULT_LOCATION_CIRCLE_INTERPOLATOR_PULSE_MODE = PulseMode.DECELERATE;
  private static final boolean DEFAULT_LOCATION_CIRCLE_PULSE_FADE_MODE = false;

  private static int DEFAULT_LOCATION_CIRCLE_PULSE_COLOR;

  private Location lastLocation;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_location_layer_pulsing_circle);

    DEFAULT_LOCATION_CIRCLE_PULSE_COLOR = ContextCompat.getColor(this, R.color.mapbox_location_layer_blue);

    mapView = findViewById(R.id.mapView);

    pulsingCircleFrequencyButton = findViewById(R.id.button_location_circle_frequency);
    pulsingCircleFrequencyButton.setText(String.format("%sms",
        String.valueOf(DEFAULT_LOCATION_CIRCLE_PULSE_FREQUENCY)));
    pulsingCircleFrequencyButton.setOnClickListener(v -> {
      if (locationComponent == null) {
        return;
      }
      showFrequencyListDialog();
    });

    pulsingCircleColorButton = findViewById(R.id.button_location_circle_color);
    pulsingCircleColorButton.setOnClickListener(v -> {
      if (locationComponent == null) {
        return;
      }
      showColorListDialog();
    });


    if (savedInstanceState != null) {
      lastLocation = savedInstanceState.getParcelable(SAVED_STATE_LOCATION);
    }

    mapView.onCreate(savedInstanceState);

    if (PermissionsManager.areLocationPermissionsGranted(this)) {
      mapView.getMapAsync(this);
    } else {
      permissionsManager = new PermissionsManager(new PermissionsListener() {
        @Override
        public void onExplanationNeeded(List<String> permissionsToExplain) {
          Toast.makeText(LocationPulsingCircleActivity.this, "You need to accept location permissions.",
              Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onPermissionResult(boolean granted) {
          if (granted) {
            mapView.getMapAsync(LocationPulsingCircleActivity.this);
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
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;

    mapboxMap.setStyle(Style.MAPBOX_STREETS, style -> {

      locationComponent = mapboxMap.getLocationComponent();
      locationComponent.activateLocationComponent(this, style, true,
          new LocationEngineRequest.Builder(750)
              .setFastestInterval(750)
              .setPriority(LocationEngineRequest.PRIORITY_HIGH_ACCURACY)
              .build(),
          buildLocationComponentOptions(
              "waterway-label",
              DEFAULT_LOCATION_CIRCLE_PULSE_COLOR,
              DEFAULT_LOCATION_CIRCLE_PULSE_ALPHA,
              DEFAULT_LOCATION_CIRCLE_PULSE_DURATION,
              DEFAULT_LOCATION_CIRCLE_PULSE_FREQUENCY));

      locationComponent.setLocationComponentEnabled(true);
      locationComponent.setCameraMode(CameraMode.TRACKING);
      locationComponent.forceLocationUpdate(lastLocation);
    });
  }

  private LocationComponentOptions buildLocationComponentOptions(@Nullable String idOfLayerBelow,
                                                                 @Nullable int pulsingCircleColor,
                                                                 @Nullable float pulsingCircleAlpha,
                                                                 @Nullable float pulsingCircleDuration,
                                                                 @Nullable float pulsingCircleFrequency) {
    currentPulseFrequency = pulsingCircleFrequency;
    return LocationComponentOptions.builder(this)
        .layerBelow(idOfLayerBelow)
        .pulsingCircleEnabled(true)
        .pulsingCircleFadeEnabled(DEFAULT_LOCATION_CIRCLE_PULSE_FADE_MODE)
        .pulsingCircleInterpolator(DEFAULT_LOCATION_CIRCLE_INTERPOLATOR_PULSE_MODE)
        .pulsingCircleColor(pulsingCircleColor)
        .pulsingCircleAlpha(pulsingCircleAlpha)
        .pulsingCircleDuration(pulsingCircleDuration)
        .pulsingCircleFrequency(pulsingCircleFrequency)
        .build();
  }

  @SuppressLint("MissingPermission")
  private void setNewLocationComponentOptions(@Nullable float newPulsingFrequency,
                                              @Nullable int newPulsingColor) {
    if (mapboxMap.getStyle() != null) {
      locationComponent.activateLocationComponent(this, mapboxMap.getStyle(),
          true,
          new LocationEngineRequest.Builder(750)
              .setFastestInterval(750)
              .setPriority(LocationEngineRequest.PRIORITY_HIGH_ACCURACY)
              .build(), buildLocationComponentOptions("waterway-label",
              newPulsingColor, DEFAULT_LOCATION_CIRCLE_PULSE_ALPHA,
              DEFAULT_LOCATION_CIRCLE_PULSE_DURATION, newPulsingFrequency));
    }
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_location_mode, menu);
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
    } else if (id == R.id.action_component_throttling_enabled) {
      locationComponent.setMaxAnimationFps(5);
    } else if (id == R.id.action_component_throttling_disabled) {
      locationComponent.setMaxAnimationFps(Integer.MAX_VALUE);
    }

    return super.onOptionsItemSelected(item);
  }

  private void toggleMapStyle() {
    if (locationComponent == null) {
      return;
    }
    String styleUrl = Style.DARK.equals(mapboxMap.getStyle().getUrl()) ? Style.LIGHT : Style.DARK;
    mapboxMap.setStyle(new Style.Builder().fromUrl(styleUrl));
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

  private void showFrequencyListDialog() {
    List<String> modes = new ArrayList<>();
    modes.add("400ms");
    modes.add("1000ms");
    modes.add("2000ms");
    ArrayAdapter<String> profileAdapter = new ArrayAdapter<>(this,
        android.R.layout.simple_list_item_1, modes);
    ListPopupWindow listPopup = new ListPopupWindow(this);
    listPopup.setAdapter(profileAdapter);
    listPopup.setAnchorView(pulsingCircleFrequencyButton);
    listPopup.setOnItemClickListener((parent, itemView, position, id) -> {
      String selectedMode = modes.get(position);
      pulsingCircleFrequencyButton.setText(selectedMode);
      if (selectedMode.contentEquals(String.format("%sms",
          String.valueOf(DEFAULT_LOCATION_CIRCLE_PULSE_FREQUENCY)))) {
        setNewLocationComponentOptions(DEFAULT_LOCATION_CIRCLE_PULSE_FREQUENCY, Color.BLUE);
      } else if (selectedMode.contentEquals("1000ms")) {
        setNewLocationComponentOptions(1000, Color.BLUE);
      } else if (selectedMode.contentEquals("3000ms")) {
        setNewLocationComponentOptions(3000, Color.BLUE);
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
    trackingTypes.add("Yellow");
    ArrayAdapter<String> profileAdapter = new ArrayAdapter<>(this,
        android.R.layout.simple_list_item_1, trackingTypes);
    ListPopupWindow listPopup = new ListPopupWindow(this);
    listPopup.setAdapter(profileAdapter);
    listPopup.setAnchorView(pulsingCircleColorButton);
    listPopup.setOnItemClickListener((parent, itemView, position, id) -> {
      String selectedTrackingType = trackingTypes.get(position);
      pulsingCircleColorButton.setText(selectedTrackingType);
      if (selectedTrackingType.contentEquals("Blue")) {
        setNewLocationComponentOptions(currentPulseFrequency, Color.BLUE);
      } else if (selectedTrackingType.contentEquals("Red")) {
        setNewLocationComponentOptions(currentPulseFrequency, Color.RED);
      } else if (selectedTrackingType.contentEquals("Green")) {
        setNewLocationComponentOptions(currentPulseFrequency, Color.GREEN);
      } else if (selectedTrackingType.contentEquals("Yellow")) {
        setNewLocationComponentOptions(currentPulseFrequency, Color.YELLOW);
      }
      listPopup.dismiss();
    });
    listPopup.show();
  }
}