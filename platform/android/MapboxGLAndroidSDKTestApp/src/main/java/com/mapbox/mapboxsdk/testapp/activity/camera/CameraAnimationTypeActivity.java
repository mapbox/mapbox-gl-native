package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test activity showcasing the Camera API and listen to camera updates by animating the camera above London.
 * <p>
 * Shows how to use animate, ease and move camera update factory methods.
 * </p>
 */
public class CameraAnimationTypeActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final LatLng LAT_LNG_LONDON_EYE = new LatLng(51.50325, -0.11968);
  private static final LatLng LAT_LNG_TOWER_BRIDGE = new LatLng(51.50550, -0.07520);

  private MapboxMap mapboxMap;
  private MapView mapView;
  private boolean cameraState;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_camera_animation_types);

    mapView = (MapView) findViewById(R.id.mapView);
    if (mapView != null) {
      mapView.onCreate(savedInstanceState);
      mapView.getMapAsync(this);
    }
  }

  @Override
  public void onMapReady(MapboxMap map) {
    mapboxMap = map;
    mapboxMap.getUiSettings().setAttributionEnabled(false);
    mapboxMap.getUiSettings().setLogoEnabled(false);
    mapboxMap.setOnCameraChangeListener(position -> Timber.w(position.toString()));

    // handle move button clicks
    View moveButton = findViewById(R.id.cameraMoveButton);
    if (moveButton != null) {
      moveButton.setOnClickListener(view -> {
        CameraPosition cameraPosition = new CameraPosition.Builder()
          .target(getNextLatLng())
          .zoom(14)
          .tilt(30)
          .tilt(0)
          .build();
        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
      });
    }

    // handle ease button clicks
    View easeButton = findViewById(R.id.cameraEaseButton);
    if (easeButton != null) {
      easeButton.setOnClickListener(view -> {
        CameraPosition cameraPosition = new CameraPosition.Builder()
          .target(getNextLatLng())
          .zoom(15)
          .bearing(180)
          .tilt(30)
          .build();

        MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
          @Override
          public void onCancel() {
            Timber.i("Duration onCancel Callback called.");
            Toast.makeText(
              CameraAnimationTypeActivity.this,
              "Ease onCancel Callback called.",
              Toast.LENGTH_LONG).show();
          }

          @Override
          public void onFinish() {
            Timber.i("Duration onFinish Callback called.");
            Toast.makeText(
              CameraAnimationTypeActivity.this,
              "Ease onFinish Callback called.",
              Toast.LENGTH_LONG).show();
          }
        };

        mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 7500, callback);
      });
    }

    // handle animate button clicks
    View animateButton = findViewById(R.id.cameraAnimateButton);
    if (animateButton != null) {
      animateButton.setOnClickListener(view -> {
        CameraPosition cameraPosition = new CameraPosition.Builder()
          .target(getNextLatLng())
          .bearing(270)
          .tilt(20)
          .build();

        MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
          @Override
          public void onCancel() {
            Timber.i("Duration onCancel Callback called.");
            Toast.makeText(
              CameraAnimationTypeActivity.this,
              "Duration onCancel Callback called.",
              Toast.LENGTH_LONG).show();
          }

          @Override
          public void onFinish() {
            Timber.i("Duration onFinish Callback called.");
            Toast.makeText(
              CameraAnimationTypeActivity.this,
              "Duration onFinish Callback called.",
              Toast.LENGTH_LONG).show();
          }
        };

        mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 7500, callback);
      });
    }
  }

  private LatLng getNextLatLng() {
    cameraState = !cameraState;
    return cameraState ? LAT_LNG_TOWER_BRIDGE : LAT_LNG_LONDON_EYE;
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
}
