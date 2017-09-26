package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.os.Bundle;
import android.support.v4.view.animation.FastOutLinearInInterpolator;
import android.support.v4.view.animation.FastOutSlowInInterpolator;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.animation.AnticipateOvershootInterpolator;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using Android SDK animators to animate camera position changes.
 */
public class CameraAnimatorActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final double ANIMATION_DELAY_FACTOR = 1.5;

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_camera_animator);

    mapView = (MapView) findViewById(R.id.mapView);
    if (mapView != null) {
      mapView.onCreate(savedInstanceState);
      mapView.getMapAsync(this);
    }
  }

  @Override
  public void onMapReady(final MapboxMap map) {
    mapboxMap = map;
    findViewById(R.id.fab).setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        view.setVisibility(View.GONE);
        createAnimator(mapboxMap.getCameraPosition()).start();
      }
    });
  }

  private Animator createAnimator(CameraPosition currentPosition) {
    AnimatorSet animatorSet = new AnimatorSet();
    animatorSet.play(createLatLngAnimator(currentPosition.target));
    animatorSet.play(createZoomAnimator(currentPosition.zoom));
    animatorSet.play(createBearingAnimator(currentPosition.bearing));
    animatorSet.play(createTiltAnimator(currentPosition.tilt));
    return animatorSet;
  }

  private Animator createLatLngAnimator(LatLng currentPosition) {
    LatLng target = new LatLng(37.789992, -122.402214);
    ValueAnimator latLngAnimator = ValueAnimator.ofObject(new LatLngEvaluator(), currentPosition, target);
    latLngAnimator.setDuration((long) (1000 * ANIMATION_DELAY_FACTOR));
    latLngAnimator.setInterpolator(new FastOutSlowInInterpolator());
    latLngAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
      @Override
      public void onAnimationUpdate(ValueAnimator animation) {
        mapboxMap.setLatLng((LatLng) animation.getAnimatedValue());
      }
    });
    return latLngAnimator;
  }

  private Animator createZoomAnimator(double currentZoom) {
    ValueAnimator zoomAnimator = ValueAnimator.ofFloat((float) currentZoom, 14.5f);
    zoomAnimator.setDuration((long) (2200 * ANIMATION_DELAY_FACTOR));
    zoomAnimator.setStartDelay((long) (600 * ANIMATION_DELAY_FACTOR));
    zoomAnimator.setInterpolator(new AnticipateOvershootInterpolator());
    zoomAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
      @Override
      public void onAnimationUpdate(ValueAnimator animation) {
        mapboxMap.setZoom((Float) animation.getAnimatedValue());
      }
    });
    return zoomAnimator;
  }

  private Animator createBearingAnimator(double currentBearing) {
    ValueAnimator bearingAnimator = ValueAnimator.ofFloat((float) currentBearing, 135);
    bearingAnimator.setDuration((long) (1000 * ANIMATION_DELAY_FACTOR));
    bearingAnimator.setStartDelay((long) (1000 * ANIMATION_DELAY_FACTOR));
    bearingAnimator.setInterpolator(new FastOutLinearInInterpolator());
    bearingAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
      @Override
      public void onAnimationUpdate(ValueAnimator animation) {
        mapboxMap.setBearing((Float) animation.getAnimatedValue());
      }
    });
    return bearingAnimator;
  }

  private Animator createTiltAnimator(double currentTilt) {
    ValueAnimator tiltAnimator = ValueAnimator.ofFloat((float) currentTilt, 60);
    tiltAnimator.setDuration((long) (1000 * ANIMATION_DELAY_FACTOR));
    tiltAnimator.setStartDelay((long) (1500 * ANIMATION_DELAY_FACTOR));
    tiltAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
      @Override
      public void onAnimationUpdate(ValueAnimator animation) {
        mapboxMap.setTilt((Float) animation.getAnimatedValue());
      }
    });
    return tiltAnimator;
  }

  private static class LatLngEvaluator implements TypeEvaluator<LatLng> {

    private final LatLng latLng = new LatLng();

    @Override
    public LatLng evaluate(float fraction, LatLng startValue, LatLng endValue) {
      latLng.setLatitude(startValue.getLatitude()
        + ((endValue.getLatitude() - startValue.getLatitude()) * fraction));
      latLng.setLongitude(startValue.getLongitude()
        + ((endValue.getLongitude() - startValue.getLongitude()) * fraction));
      return latLng;
    }
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
