package com.mapbox.mapboxsdk.testapp.activity;

import android.app.Activity;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.IdlingResourceTimeoutException;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import junit.framework.Assert;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;

import timber.log.Timber;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

public abstract class BaseActivityTest {

  @Rule
  public ActivityTestRule<Activity> rule = new ActivityTestRule<>(getActivityClass());
  protected MapboxMap mapboxMap;
  protected OnMapReadyIdlingResource idlingResource;

  @Before
  public void beforeTest() {
    try {
      Timber.e("@Before test: register idle resource");
      idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
      Espresso.registerIdlingResources(idlingResource);
      checkViewIsDisplayed(R.id.mapView);
      mapboxMap = idlingResource.getMapboxMap();
    } catch (IdlingResourceTimeoutException idlingResourceTimeoutException) {
      Timber.e("Idling resource timed out. Couldn't not validate if map is ready.");
      throw new RuntimeException("Could not start test for " + getActivityClass().getSimpleName() + ".\n"
        + "The ViewHierarchy doesn't contain a view with resource id = R.id.mapView or \n"
        + "the Activity doesn't contain an instance variable with a name equal to mapboxMap.\n"
        + "You can resolve this issue be implementing the requirements above or\n add "
        + getActivityClass().getSimpleName() + " to the excludeActivities array in `generate-test-code.js`.\n");
    }
  }

  protected void validateTestSetup() {
    Assert.assertTrue("Device is not connected to the Internet.", isConnected(rule.getActivity()));
    checkViewIsDisplayed(R.id.mapView);
    Assert.assertNotNull(mapboxMap);
  }

  protected MapboxMap getMapboxMap() {
    return mapboxMap;
  }

  protected abstract Class getActivityClass();

  protected void checkViewIsDisplayed(int id) {
    onView(withId(id))
      .check(matches(isDisplayed()));
  }

  protected void waitLoop() {
    onView(withId(R.id.mapView)).perform(new LoopAction(500));
  }

  static boolean isConnected(Context context) {
    ConnectivityManager connectivityManager
      = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
    NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
    return activeNetworkInfo != null && activeNetworkInfo.isConnected();
  }

  @After
  public void afterTest() {
    Timber.e("@After test: unregister idle resource");
    Espresso.unregisterIdlingResources(idlingResource);
  }

  private class LoopAction implements ViewAction {

    private long loopTime;

    public LoopAction(long loopTime) {
      this.loopTime = loopTime;
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
      uiController.loopMainThreadForAtLeast(loopTime);
    }
  }
}

