package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.os.Bundle;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.animation.AccelerateDecelerateInterpolator;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.geometry.VisibleRegion;
import com.mapbox.mapboxsdk.maps.Callback;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.api.utils.turf.TurfMeasurement;
import com.mapbox.services.commons.models.Position;

import java.util.Random;

public class AnimatedMarkerActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  private LatLng dupontCircle = new LatLng(38.90962, -77.04341);

  private Marker passengerMarker = null;
  private MarkerView carMarker = null;

  private Runnable animationRunnable;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_animated_marker);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {

      @Override
      public void onMapReady(@NonNull final MapboxMap mapboxMap) {
        AnimatedMarkerActivity.this.mapboxMap = mapboxMap;
        setupMap();

        animationRunnable = new Runnable() {
          @Override
          public void run() {
            for (int i = 0; i < 10; i++) {
              addRandomCar();
            }
            addPassenger();
            addMainCar();
          }
        };
        mapView.post(animationRunnable);
      }
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
    getLatLngInBounds(new Callback<LatLng>() {
      @Override
      public void onResult(LatLng randomLatLng) {
        if (passengerMarker == null) {
          Icon icon = IconFactory.getInstance(AnimatedMarkerActivity.this)
            .fromResource(R.drawable.ic_directions_run_black);
          mapboxMap.addMarker(new MarkerViewOptions()
              .position(randomLatLng)
              .icon(icon),
            new Callback<MarkerView>() {
              @Override
              public void onResult(MarkerView markerView) {
                passengerMarker = markerView;
              }
            });
        } else {
          passengerMarker.setPosition(randomLatLng);
        }
      }
    });
  }

  private void addMainCar() {
    getLatLngInBounds(new Callback<LatLng>() {
      @Override
      public void onResult(LatLng randomLatLng) {
        if (carMarker == null) {
          createCarMarker(randomLatLng, R.drawable.ic_taxi_top, new Callback<MarkerView>() {
            @Override
            public void onResult(MarkerView markerView) {
              carMarker = markerView;
              mapboxMap.selectMarker(carMarker);
              animateMoveToPassenger(carMarker);
            }
          });
        } else {
          carMarker.setPosition(randomLatLng);
        }
      }
    });
  }

  private void animateMoveToPassenger(final MarkerView car) {
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
    getLatLngInBounds(new Callback<LatLng>() {
      @Override
      public void onResult(LatLng latLng) {
        createCarMarker(latLng, R.drawable.ic_car_top, new Callback<MarkerView>() {
          @Override
          public void onResult(MarkerView markerView) {
            randomlyMoveMarker(markerView);
          }
        });
      }
    });
  }

  private void randomlyMoveMarker(final MarkerView marker) {
    getLatLngInBounds(new Callback<LatLng>() {
      @Override
      public void onResult(LatLng latLng) {
        ValueAnimator animator = animateMoveMarker(marker, latLng);
        // Add listener to restart animation on end
        animator.addListener(new AnimatorListenerAdapter() {
          @Override
          public void onAnimationEnd(Animator animation) {
            randomlyMoveMarker(marker);
          }
        });
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

  private void createCarMarker(LatLng start, @DrawableRes int carResource,
                               Callback<MarkerView> listener) {
    Icon icon = IconFactory.getInstance(AnimatedMarkerActivity.this)
      .fromResource(carResource);

    // View Markers
    mapboxMap.addMarker(new MarkerViewOptions()
      .position(start)
      .icon(icon), listener);

    // GL Markers
    //return mapboxMap.addMarker(new MarkerOptions()
    //  .position(start)
    //  .icon(icon));
  }

  private void getLatLngInBounds(final Callback<LatLng> listener) {
    mapboxMap.getProjection().getVisibleRegion(new Callback<VisibleRegion>() {
      @Override
      public void onResult(VisibleRegion visibleRegion) {
        LatLngBounds bounds = visibleRegion.latLngBounds;
        Random generator = new Random();
        double randomLat = bounds.getLatSouth() + generator.nextDouble()
          * (bounds.getLatNorth() - bounds.getLatSouth());
        double randomLon = bounds.getLonWest() + generator.nextDouble()
          * (bounds.getLonEast() - bounds.getLonWest());
        listener.onResult(new LatLng(randomLat, randomLon));
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
}
