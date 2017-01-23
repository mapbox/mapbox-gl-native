package com.mapbox.mapboxsdk.testapp.maps.widgets;

import android.app.Instrumentation;
import android.content.Intent;
import android.net.Uri;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.espresso.intent.Intents;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.click;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.intent.Intents.intended;
import static android.support.test.espresso.intent.Intents.intending;
import static android.support.test.espresso.intent.matcher.IntentMatchers.hasAction;
import static android.support.test.espresso.intent.matcher.IntentMatchers.hasData;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static android.support.test.espresso.matcher.ViewMatchers.withText;
import static org.hamcrest.Matchers.allOf;
import static org.hamcrest.core.IsNot.not;

public class AttributionTest {

  @Rule
  public final ActivityTestRule<EspressoTestActivity> rule = new ActivityTestRule<>(EspressoTestActivity.class);

  private OnMapReadyIdlingResource idlingResource;

  private String[] dialogTexts;
  private String[] dialogLinks;

  @Before
  public void beforeTest() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
    Intents.init();
    dialogTexts = rule.getActivity().getResources().getStringArray(R.array.mapbox_attribution_names);
    dialogLinks = rule.getActivity().getResources().getStringArray(R.array.mapbox_attribution_links);
  }

  @Test
  public void testDisabled() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    // Default
    onView(withId(R.id.attributionView)).check(matches(isDisplayed()));

    // Disabled
    onView(withId(R.id.attributionView))
      .perform(new DisableAction(mapboxMap))
      .check(matches(not(isDisplayed())));
  }

  @Test
  public void testMapboxLink() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);

    // click on View to open dialog
    onView(withId(R.id.attributionView)).perform(click());
    onView(withText(R.string.mapbox_attributionsDialogTitle)).check(matches(isDisplayed()));

    // click on link and validate browser opening
    Matcher<Intent> expectedIntent = allOf(hasAction(Intent.ACTION_VIEW), hasData(Uri.parse(dialogLinks[0])));
    intending(expectedIntent).respondWith(new Instrumentation.ActivityResult(0, null));
    onView(withText(dialogTexts[0])).perform(click());
    intended(expectedIntent);
  }

  @Test
  public void testOpenStreetMapLink() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);

    // click on View to open dialog
    onView(withId(R.id.attributionView)).perform(click());
    onView(withText(R.string.mapbox_attributionsDialogTitle)).check(matches(isDisplayed()));

    // click on link and validate browser opening
    Matcher<Intent> expectedIntent = allOf(hasAction(Intent.ACTION_VIEW), hasData(Uri.parse(dialogLinks[1])));
    intending(expectedIntent).respondWith(new Instrumentation.ActivityResult(0, null));
    onView(withText(dialogTexts[1])).perform(click());
  }

  @Test
  public void testImproveMapLink() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);

    // click on View to open dialog
    onView(withId(R.id.attributionView)).perform(click());
    onView(withText(R.string.mapbox_attributionsDialogTitle)).check(matches(isDisplayed()));

    // click on Mapbox link and validate browser opening
    Matcher<Intent> expectedIntent = allOf(hasAction(Intent.ACTION_VIEW), hasData(Uri.parse(dialogLinks[3])));
    intending(expectedIntent).respondWith(new Instrumentation.ActivityResult(0, null));
    onView(withText(dialogTexts[3])).perform(click());
  }

  @Test
  public void testTelemetryDialog() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);

    // click on View to open dialog
    onView(withId(R.id.attributionView)).perform(click());
    onView(withText(R.string.mapbox_attributionsDialogTitle)).check(matches(isDisplayed()));

    // click on item to open second dialog
    onView(withText(dialogTexts[3])).perform(click());
    onView(withText(R.string.mapbox_attributionTelemetryTitle)).check(matches(isDisplayed()));
  }

  @After
  public void afterTest() {
    Intents.release();
    Espresso.unregisterIdlingResources(idlingResource);
  }

  private class DisableAction implements ViewAction {

    private MapboxMap mapboxMap;

    DisableAction(MapboxMap map) {
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
      mapboxMap.getUiSettings().setAttributionEnabled(false);
    }
  }
}
