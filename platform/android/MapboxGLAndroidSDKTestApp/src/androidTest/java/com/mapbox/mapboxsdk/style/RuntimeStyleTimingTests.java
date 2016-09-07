package com.mapbox.mapboxsdk.style;

import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;

import com.mapbox.mapboxsdk.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTimingTestActivity;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Basic smoke tests for adding Layer and Source as early as possible (in onCreate)
 */
@RunWith(AndroidJUnit4.class)
public class RuntimeStyleTimingTests extends BaseTest {

    @Rule
    public final ActivityTestRule<RuntimeStyleTimingTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTimingTestActivity.class);

    private OnMapReadyIdlingResource idlingResource;

    @Before
    public void registerIdlingResource() {
        idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
        Espresso.registerIdlingResources(idlingResource);
    }

    @Test
    public void testGetAddRemoveLayer() {
        checkViewIsDisplayed(R.id.mapView);
        //We're good if it didn't crash
    }

    @After
    public void unregisterIntentServiceIdlingResource() {
        Espresso.unregisterIdlingResources(idlingResource);
    }
}
