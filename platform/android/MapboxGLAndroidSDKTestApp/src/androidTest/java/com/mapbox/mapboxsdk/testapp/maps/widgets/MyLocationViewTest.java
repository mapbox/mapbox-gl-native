package com.mapbox.mapboxsdk.testapp.maps.widgets;

import android.annotation.SuppressLint;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;

import org.hamcrest.Description;
import org.hamcrest.Matcher;
import org.hamcrest.TypeSafeMatcher;
import org.junit.Ignore;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.hamcrest.Matchers.not;

/**
 * Experimental MyLocationView tests,
 * requires application to be granted with runtime location permissions.
 * <p>
 * Tests for enabling and disabling the {@link MyLocationView}.
 * Tests for enabling tracking modes and if the correct default images are shown when toggling
 * {@link com.mapbox.mapboxsdk.maps.TrackingSettings#setMyLocationTrackingMode(int)} &
 * {@link com.mapbox.mapboxsdk.maps.TrackingSettings#setMyBearingTrackingMode(int)}.
 * </p>
 */
public class MyLocationViewTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  @Ignore // requires runtime permissions, disabled for CI
  public void testEnabled() {
    validateTestSetup();
    onView(withId(R.id.userLocationView)).check(matches(not(isDisplayed())));
    onView(withId(R.id.mapView)).perform(new ToggleLocationAction(mapboxMap, true));
    onView(withId(R.id.userLocationView)).check(matches(isDisplayed()));
    onView(withId(R.id.mapView)).perform(new ToggleLocationAction(mapboxMap, false));
    onView(withId(R.id.userLocationView)).check(matches(not(isDisplayed())));
  }

  @Test
  @Ignore
  // requires runtime permissions, disabled for CI + issue with android.support.test.espresso.AppNotIdleException:
  // Looped for 5049 iterations over 60 SECONDS.
  public void testTracking() {
    validateTestSetup();
    onView(withId(R.id.userLocationView)).check(matches(not(isDisplayed())));
    onView(withId(R.id.mapView)).perform(new EnableLocationTrackingAction(mapboxMap));
    onView(withId(R.id.userLocationView)).check(matches(isDisplayed()));
    onView(withId(R.id.userLocationView)).check(matches(new DrawableMatcher(mapboxMap,
      R.drawable.mapbox_mylocation_icon_default, false)));
    onView(withId(R.id.mapView)).perform(new EnableCompassBearingTrackingAction(mapboxMap));
    onView(withId(R.id.userLocationView)).check(matches(new DrawableMatcher(mapboxMap,
      R.drawable.mapbox_mylocation_icon_bearing, true)));
  }

  private class ToggleLocationAction implements ViewAction {

    private MapboxMap mapboxMap;
    private boolean isEnabled;

    ToggleLocationAction(MapboxMap map, boolean enable) {
      mapboxMap = map;
      isEnabled = enable;
    }

    @Override
    public Matcher<View> getConstraints() {
      return isDisplayed();
    }

    @Override
    public String getDescription() {
      return getClass().getSimpleName();
    }

    @SuppressLint("MissingPermission")
    @Override
    public void perform(UiController uiController, View view) {
      if (isEnabled) {
        // move camera above user location
        mapboxMap.moveCamera(
          CameraUpdateFactory.newCameraPosition(
            new CameraPosition.Builder()
              .target(new LatLng(Mapbox.getLocationEngine().getLastLocation()))
              .build()
          )
        );
      }

      // show loction on screen
      mapboxMap.setMyLocationEnabled(isEnabled);
    }
  }

  private class EnableLocationTrackingAction implements ViewAction {

    private MapboxMap mapboxMap;

    EnableLocationTrackingAction(MapboxMap map) {
      mapboxMap = map;
    }

    @Override
    public Matcher<View> getConstraints() {
      return isDisplayed();
    }

    @Override
    public String getDescription() {
      return getClass().getSimpleName();
    }

    @Override
    public void perform(UiController uiController, View view) {
      mapboxMap.getTrackingSettings().setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
    }
  }

  private class EnableCompassBearingTrackingAction implements ViewAction {

    private MapboxMap mapboxMap;

    EnableCompassBearingTrackingAction(MapboxMap map) {
      mapboxMap = map;
    }

    @Override
    public Matcher<View> getConstraints() {
      return isDisplayed();
    }

    @Override
    public String getDescription() {
      return getClass().getSimpleName();
    }

    @Override
    public void perform(UiController uiController, View view) {
      mapboxMap.getTrackingSettings().setMyBearingTrackingMode(MyBearingTracking.COMPASS);
      // wait for next compass update cycle
      uiController.loopMainThreadForAtLeast(500);
    }
  }

  private class DrawableMatcher extends TypeSafeMatcher<View> {

    private MapboxMap mapboxMap;
    private boolean isBearingDrawable;
    private final int expectedId;

    DrawableMatcher(MapboxMap mapboxMap, int expectedId, boolean isBearingDrawable) {
      super(MyLocationView.class);
      this.mapboxMap = mapboxMap;
      this.expectedId = expectedId;
      this.isBearingDrawable = isBearingDrawable;
    }

    @Override
    protected boolean matchesSafely(View target) {
      Drawable currentDrawable = isBearingDrawable
        ? mapboxMap.getMyLocationViewSettings().getForegroundBearingDrawable() :
        mapboxMap.getMyLocationViewSettings().getForegroundDrawable();

      Resources resources = target.getContext().getResources();
      Drawable expectedDrawable = resources.getDrawable(expectedId);
      return areDrawablesIdentical(currentDrawable, expectedDrawable);
    }

    @Override
    public void describeTo(Description description) {
      description.appendText("trying to match MyLocationView drawable to " + expectedId);
    }

    boolean areDrawablesIdentical(Drawable drawableA, Drawable drawableB) {
      Drawable.ConstantState stateA = drawableA.getConstantState();
      Drawable.ConstantState stateB = drawableB.getConstantState();
      return (stateA != null && stateB != null && stateA.equals(stateB))
        || getBitmap(drawableA).sameAs(getBitmap(drawableB));
    }

    Bitmap getBitmap(Drawable drawable) {
      Bitmap result;
      if (drawable instanceof BitmapDrawable) {
        result = ((BitmapDrawable) drawable).getBitmap();
      } else {
        int width = drawable.getIntrinsicWidth();
        int height = drawable.getIntrinsicHeight();
        // Some drawables have no intrinsic width - e.g. solid colours.
        if (width <= 0) {
          width = 1;
        }
        if (height <= 0) {
          height = 1;
        }

        result = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(result);
        drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
        drawable.draw(canvas);
      }
      return result;
    }
  }
}
