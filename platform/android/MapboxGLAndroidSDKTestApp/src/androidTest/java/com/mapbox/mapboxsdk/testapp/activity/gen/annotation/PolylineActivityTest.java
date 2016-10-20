// This file is generated. Edit android/platform/scripts/generate-test-code.js, then run `make generate-test-android`.
package com.mapbox.mapboxsdk.testapp.activity.gen.annotation;

import android.support.test.runner.AndroidJUnit4;

import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.annotation.PolylineActivity;

import org.junit.Test;
import org.junit.runner.RunWith;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

/**
 * Sanity test for PolylineActivity
 */
@RunWith(AndroidJUnit4.class)
public class PolylineActivityTest extends BaseActivityTest {

    @Test
    public void testSanity() {
        onView(withId(R.id.mapView)).check(matches(isDisplayed()));
    }

    @Override
    protected Class getActivityClass() {
        return PolylineActivity.class;
    }
}
