package com.mapbox.mapboxsdk.testapp.maps.widgets;

import android.app.Instrumentation;
import android.content.Intent;
import android.net.Uri;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.espresso.intent.Intents;
import android.text.Html;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.text.style.URLSpan;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onData;
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
import static org.hamcrest.CoreMatchers.allOf;
import static org.hamcrest.CoreMatchers.anything;
import static org.hamcrest.core.IsNot.not;

public class AttributionTest extends BaseActivityTest {

  private URLSpan[] urlSpans;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Before
  public void beforeTest() {
    super.beforeTest();
    Intents.init();
  }

  @Test
  public void testDisabled() {
    validateTestSetup();

    // Default
    onView(withId(R.id.attributionView)).check(matches(isDisplayed()));

    // Disabled
    onView(withId(R.id.attributionView))
      .perform(new DisableAction(mapboxMap))
      .check(matches(not(isDisplayed())));
  }

  @Test
  @Ignore
  public void testMapboxStreetsMapboxAttributionLink() {
    validateTestSetup();
    if (urlSpans == null) {
      buildUrlSpans();
    }

    // click on View to open dialog
    onView(withId(R.id.attributionView)).perform(click());
    onView(withText(R.string.mapbox_attributionsDialogTitle)).check(matches(isDisplayed()));

    // test for trigger url intent
    Matcher<Intent> expectedIntent = allOf(hasAction(Intent.ACTION_VIEW), hasData(Uri.parse(urlSpans[0].getURL())));
    intending(expectedIntent).respondWith(new Instrumentation.ActivityResult(0, null));

    // click item and test for url
    onData(anything()).inAdapterView(withId(R.id.select_dialog_listview)).atPosition(0).perform(click());
    intended(expectedIntent);
  }

  @Test
  @Ignore
  public void testMapboxStreetsOpenStreetMapAttributionLink() {
    validateTestSetup();
    if (urlSpans == null) {
      buildUrlSpans();
    }

    // click on View to open dialog
    onView(withId(R.id.attributionView)).perform(click());
    onView(withText(R.string.mapbox_attributionsDialogTitle)).check(matches(isDisplayed()));

    // test for trigger url intent
    Matcher<Intent> expectedIntent = allOf(hasAction(Intent.ACTION_VIEW), hasData(Uri.parse(urlSpans[1].getURL())));
    intending(expectedIntent).respondWith(new Instrumentation.ActivityResult(0, null));

    // click item and test for url
    onData(anything()).inAdapterView(withId(R.id.select_dialog_listview)).atPosition(1).perform(click());
    intended(expectedIntent);
  }

  @Test
  @Ignore
  public void testImproveMapLink() {
    validateTestSetup();
    if (urlSpans == null) {
      buildUrlSpans();
    }

    // click on View to open dialog
    onView(withId(R.id.attributionView)).perform(click());
    onView(withText(R.string.mapbox_attributionsDialogTitle)).check(matches(isDisplayed()));

    // test for trigger url intent
    Matcher<Intent> expectedIntent = hasAction(Intent.ACTION_VIEW);
    intending(expectedIntent).respondWith(new Instrumentation.ActivityResult(0, null));

    // click item and test for url
    onData(anything()).inAdapterView(withId(R.id.select_dialog_listview)).atPosition(2).perform(click());
    intended(expectedIntent);
  }

  @Test
  public void testTelemetryDialog() {
    validateTestSetup();

    // click on View to open dialog
    onView(withId(R.id.attributionView)).perform(click());
    onView(withText(R.string.mapbox_attributionsDialogTitle)).check(matches(isDisplayed()));

    // click on item to open second dialog
    onView(withText(R.string.mapbox_telemetrySettings)).perform(click());
    onView(withText(R.string.mapbox_attributionTelemetryTitle)).check(matches(isDisplayed()));
  }

  @After
  public void afterTest() {
    super.afterTest();
    Intents.release();
  }

  private void buildUrlSpans() {
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      for (Source source : mapboxMap.getSources()) {
        String attributionSource = source.getAttribution();
        if (!TextUtils.isEmpty(attributionSource)) {
          SpannableStringBuilder htmlBuilder = (SpannableStringBuilder) Html.fromHtml(attributionSource);
          urlSpans = htmlBuilder.getSpans(0, htmlBuilder.length(), URLSpan.class);
        }
      }
    }));
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

  private class MapboxMapAction implements ViewAction {

    private InvokeViewAction invokeViewAction;

    MapboxMapAction(InvokeViewAction invokeViewAction) {
      this.invokeViewAction = invokeViewAction;
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
      invokeViewAction.onViewAction(uiController, view);
    }
  }

  interface InvokeViewAction {
    void onViewAction(UiController uiController, View view);
  }
}
