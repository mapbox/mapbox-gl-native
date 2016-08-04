package com.mapbox.mapboxsdk.activity;

import android.app.Activity;
import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.util.Log;

import com.mapbox.mapboxsdk.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.utils.ScreenshotUtil;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

public abstract class BaseTest {

    @Rule
    public ActivityTestRule<Activity> rule = new ActivityTestRule<>(getActivityClass());
    protected MapboxMap mapboxMap;
    protected OnMapReadyIdlingResource idlingResource;

    @Before
    public void beforeTest() {
        Log.e(MapboxConstants.TAG, "@Before test: register idle resource");
        idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
        Espresso.registerIdlingResources(idlingResource);
        checkViewIsDisplayed(R.id.mapView);
        mapboxMap = idlingResource.getMapboxMap();
    }

    protected abstract Class getActivityClass();

    protected void checkViewIsDisplayed(int id) {
        onView(withId(id))
                .check(matches(isDisplayed()));
    }

    protected void takeScreenshot(final String name) {
        final Activity activity = rule.getActivity();
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ScreenshotUtil.take(activity, name);
            }
        });
    }

    @After
    public void afterTest() {
        Log.e(MapboxConstants.TAG, "@After test: unregister idle resource");
        Espresso.unregisterIdlingResources(idlingResource);
    }
}

