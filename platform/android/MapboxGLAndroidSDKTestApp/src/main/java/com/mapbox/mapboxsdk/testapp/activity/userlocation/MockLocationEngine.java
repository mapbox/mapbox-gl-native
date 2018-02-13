package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.animation.AnimatorListenerAdapter;
import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.location.Location;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineListener;

import timber.log.Timber;

/**
 * Sample LocationEngine that provides mocked LOCATIONS simulating GPS updates
 */
public class MockLocationEngine extends LocationEngine {
  private static MockLocationEngine INSTANCE;

  private final LocationAnimator locationAnimator;
  private boolean running;
  private static int counter;

  MockLocationEngine(Location start, Location end) {
    locationAnimator = new LocationAnimator(start, end, animation -> {
      for (LocationEngineListener listener : locationListeners) {
        listener.onLocationChanged((Location) animation.getAnimatedValue());
      }
    });
  }

  public static synchronized MockLocationEngine getInstance() {
    if (INSTANCE == null) {
      INSTANCE = new MockLocationEngine(
        MockLocationEngine.createLocation(40.416913, -3.703861),
        MockLocationEngine.createLocation(39.461643, -0.368041)
      );
    }
    return INSTANCE;
  }

  public static Location createLocation(double latitude, double longitude) {
    Location location = new Location(MockLocationEngine.class.getSimpleName());
    location.setLatitude(latitude);
    location.setLongitude(longitude);
    return location;
  }

  @Override
  public void activate() {
    // "Connection" is immediate here
    for (LocationEngineListener listener : locationListeners) {
      listener.onConnected();
    }
  }

  @Override
  public void deactivate() {
  }

  @Override
  public boolean isConnected() {
    return true; // Always connected
  }

  @Override
  public Location getLastLocation() {
    return null;
  }

  @Override
  public void requestLocationUpdates() {
    if (!running) {
      locationAnimator.start();
      running = true;
    }
  }

  @Override
  public void removeLocationUpdates() {
    if (running) {
      locationAnimator.stop();
      running = false;
      Timber.e("LOC %s", counter);
    }
  }

  @Override
  public Type obtainType() {
    return Type.MOCK;
  }

  private static class LocationAnimator extends AnimatorListenerAdapter {

    private static final long DURATION_ANIMATION = 10000;
    private final ValueAnimator locationAnimator;
    private long animationTime;

    LocationAnimator(Location start, Location end, ValueAnimator.AnimatorUpdateListener listener) {
      locationAnimator = ValueAnimator.ofObject(new LocationEvaluator(), start, end);
      locationAnimator.setDuration(DURATION_ANIMATION);
      locationAnimator.addUpdateListener(listener);
      locationAnimator.addListener(this);
    }

    void start() {
      locationAnimator.start();
      locationAnimator.setCurrentPlayTime(animationTime);
    }

    void stop() {
      animationTime = locationAnimator.getCurrentPlayTime();
      locationAnimator.cancel();
    }

    private static class LocationEvaluator implements TypeEvaluator<Location> {

      private Location location = new Location(MockLocationEngine.class.getSimpleName());

      @Override
      public Location evaluate(float fraction, Location startValue, Location endValue) {
        counter++;
        location.setLatitude(startValue.getLatitude()
          + ((endValue.getLatitude() - startValue.getLatitude()) * fraction));
        location.setLongitude(startValue.getLongitude()
          + ((endValue.getLongitude() - startValue.getLongitude()) * fraction));
        return location;
      }
    }
  }
}
