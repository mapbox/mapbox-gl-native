package com.mapbox.mapboxsdk.testapp.activity;

import android.app.Activity;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.IdlingRegistry;
import android.support.test.espresso.IdlingResource;
import android.support.test.espresso.IdlingResourceTimeoutException;
import android.support.test.espresso.ViewInteraction;
import android.support.test.rule.ActivityTestRule;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction;
import com.mapbox.mapboxsdk.testapp.action.WaitAction;
import com.mapbox.mapboxsdk.testapp.utils.FinishLoadingStyleIdlingResource;

import com.mapbox.mapboxsdk.testapp.utils.MapboxIdlingResource;
import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.rules.TestName;

import timber.log.Timber;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

/**
 * Base class for all Activity test hooking into an existing Activity that will load style.
 */
public abstract class BaseTest {

  @Rule
  public ActivityTestRule<Activity> rule = new ActivityTestRule<>(getActivityClass());

  @Rule
  public TestName testNameRule = new TestName();

  protected MapboxMap mapboxMap;
  protected MapboxIdlingResource idlingResource;

  @Before
  public void beforeTest() {
    try {
      Timber.e(String.format(
        "%s - %s - %s",
        getClass().getSimpleName(),
        testNameRule.getMethodName(),
        "@Before test: register idle resource"
      ));
      idlingResource = (MapboxIdlingResource) generateIdlingResource();
      IdlingRegistry.getInstance().register(idlingResource);
      Espresso.onIdle();
      mapboxMap = idlingResource.getMapboxMap();
    } catch (IdlingResourceTimeoutException idlingResourceTimeoutException) {
      throw new RuntimeException(
        String.format(
          "Could not start %s test for %s.",
          testNameRule.getMethodName(),
          getActivityClass().getSimpleName()
        )
      );
    }
  }

  protected IdlingResource generateIdlingResource() {
    return new FinishLoadingStyleIdlingResource(rule.getActivity());
  }

  protected void validateTestSetup() {
    if (!Mapbox.isConnected()) {
      Timber.e("Not connected to the internet while running test");
    }

    checkViewIsDisplayed(R.id.mapView);
    Assert.assertNotNull(mapboxMap);
  }

  protected MapboxMap getMapboxMap() {
    return mapboxMap;
  }

  protected abstract Class getActivityClass();

  protected void checkViewIsDisplayed(int id) {
    onView(withId(id)).check(matches(isDisplayed()));
  }

  protected void waitAction() {
    waitAction(500);
  }

  protected void waitAction(long waitTime) {
    onView(withId(R.id.mapView)).perform(new WaitAction(waitTime));
  }

  protected ViewInteraction onMapView() {
    return onView(withId(R.id.mapView));
  }

  protected MapboxMapAction getMapboxMapAction(MapboxMapAction.OnInvokeActionListener onInvokeActionListener) {
    return new MapboxMapAction(onInvokeActionListener, mapboxMap);
  }

  @After
  public void afterTest() {
    Timber.e(String.format("%s - %s", testNameRule.getMethodName(), "@After test: unregister idle resource"));
    IdlingRegistry.getInstance().unregister(idlingResource);
  }
}

