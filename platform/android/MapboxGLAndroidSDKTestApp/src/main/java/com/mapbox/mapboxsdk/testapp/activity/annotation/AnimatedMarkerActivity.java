package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.os.Bundle;
import android.support.annotation.DrawableRes;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.animation.AccelerateDecelerateInterpolator;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.IconUtils;
import com.mapbox.services.api.utils.turf.TurfMeasurement;
import com.mapbox.services.commons.models.Position;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/**
 * Test activity showcasing animating MarkerViews.
 */
public class AnimatedMarkerActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  private LatLng dupontCircle = new LatLng(38.90962, -77.04341);

  private Marker passengerMarker = null;
  private MarkerView carMarker = null;

  private Runnable animationRunnable;

  private List<MarkerView> markerViews = new ArrayList<>();
  private boolean stopped;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_animated_marker);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      AnimatedMarkerActivity.this.mapboxMap = mapboxMap;
      setupMap();

      animationRunnable = () -> {
        for (int i = 0; i < 10; i++) {
          addRandomCar();
        }
        addPassenger();
        addMainCar();
      };
      mapView.post(animationRunnable);
    });
  }

  private void setupMap() {
    CameraPosition cameraPosition = new CameraPosition.Builder()
      .target(dupontCircle)
      .zoom(15)
      .build();
    mapboxMap.setCameraPosition(cameraPosition);
  }

  private void addPassenger() {
    if (isActivityStopped()) {
      return;
    }

    LatLng randomLatLng = getLatLngInBounds();

    if (passengerMarker == null) {
      Icon icon = IconUtils.drawableToIcon(this, R.drawable.ic_directions_run_black,
        ResourcesCompat.getColor(getResources(), R.color.blueAccent, getTheme()));
      passengerMarker = mapboxMap.addMarker(new MarkerViewOptions()
        .position(randomLatLng)
        .icon(icon));
    } else {
      passengerMarker.setPosition(randomLatLng);
    }
  }

  private void addMainCar() {
    if (isActivityStopped()) {
      return;
    }

    LatLng randomLatLng = getLatLngInBounds();

    if (carMarker == null) {
      carMarker = createCarMarker(randomLatLng, R.drawable.ic_taxi_top,
        markerView -> {
          // Make sure the car marker is selected so that it's always brought to the front (#5285)
          mapboxMap.selectMarker(carMarker);
          animateMoveToPassenger(carMarker);
        });
      markerViews.add(carMarker);
    } else {
      carMarker.setPosition(randomLatLng);
    }
  }

  private void animateMoveToPassenger(final MarkerView car) {
    if (isActivityStopped()) {
      return;
    }

    ValueAnimator animator = animateMoveMarker(car, passengerMarker.getPosition());
    animator.addListener(new AnimatorListenerAdapter() {
      @Override
      public void onAnimationEnd(Animator animation) {
        addPassenger();
        animateMoveToPassenger(car);
      }
    });
  }

  protected void addRandomCar() {
    markerViews.add(createCarMarker(getLatLngInBounds(), R.drawable.ic_car_top,
      markerView -> randomlyMoveMarker(markerView)));
  }

  private void randomlyMoveMarker(final MarkerView marker) {
    if (isActivityStopped()) {
      return;
    }

    ValueAnimator animator = animateMoveMarker(marker, getLatLngInBounds());

    // Add listener to restart animation on end
    animator.addListener(new AnimatorListenerAdapter() {
      @Override
      public void onAnimationEnd(Animator animation) {
        randomlyMoveMarker(marker);
      }
    });
  }

  private ValueAnimator animateMoveMarker(final MarkerView marker, LatLng to) {
    marker.setRotation((float) getBearing(marker.getPosition(), to));

    final ValueAnimator markerAnimator = ObjectAnimator.ofObject(
      marker, "position", new LatLngEvaluator(), marker.getPosition(), to);
    markerAnimator.setDuration((long) (10 * marker.getPosition().distanceTo(to)));
    markerAnimator.setInterpolator(new AccelerateDecelerateInterpolator());

    // Start
    markerAnimator.start();

    return markerAnimator;
  }

  private MarkerView createCarMarker(LatLng start, @DrawableRes int carResource,
                                     MarkerViewManager.OnMarkerViewAddedListener listener) {
    Icon icon = IconFactory.getInstance(AnimatedMarkerActivity.this)
      .fromResource(carResource);

    // View Markers
    return mapboxMap.addMarker(new MarkerViewOptions()
      .position(start)
      .icon(icon), listener);

    // GL Markers
//        return mapboxMap.addMarker(new MarkerOptions()
//                .position(start)
//                .icon(icon));

  }

  private LatLng getLatLngInBounds() {
    LatLngBounds bounds = mapboxMap.getProjection().getVisibleRegion().latLngBounds;
    Random generator = new Random();
    double randomLat = bounds.getLatSouth() + generator.nextDouble()
      * (bounds.getLatNorth() - bounds.getLatSouth());
    double randomLon = bounds.getLonWest() + generator.nextDouble()
      * (bounds.getLonEast() - bounds.getLonWest());
    return new LatLng(randomLat, randomLon);
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

    stopped = true;

    // Stop ongoing animations, prevent memory leaks
    if (mapboxMap != null) {
      MarkerViewManager markerViewManager = mapboxMap.getMarkerViewManager();
      for (MarkerView markerView : markerViews) {
        View view = markerViewManager.getView(markerView);
        if (view != null) {
          view.animate().cancel();
        }
      }
    }

    // onStop
    mapView.onStop();
    mapView.removeCallbacks(animationRunnable);
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

  /**
   * Evaluator for LatLng pairs
   */
  private static class LatLngEvaluator implements TypeEvaluator<LatLng> {

    private LatLng latLng = new LatLng();

    @Override
    public LatLng evaluate(float fraction, LatLng startValue, LatLng endValue) {
      latLng.setLatitude(startValue.getLatitude()
        + ((endValue.getLatitude() - startValue.getLatitude()) * fraction));
      latLng.setLongitude(startValue.getLongitude()
        + ((endValue.getLongitude() - startValue.getLongitude()) * fraction));
      return latLng;
    }
  }

  private double getBearing(LatLng from, LatLng to) {
    return TurfMeasurement.bearing(
      Position.fromCoordinates(from.getLongitude(), from.getLatitude()),
      Position.fromCoordinates(to.getLongitude(), to.getLatitude())
    );
  }

  private boolean isActivityStopped() {
    return stopped;
  }
}
