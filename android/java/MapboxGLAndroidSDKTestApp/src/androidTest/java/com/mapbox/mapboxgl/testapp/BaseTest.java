package com.mapbox.mapboxgl.testapp;

import android.app.Activity;
import android.support.test.InstrumentationRegistry;
import android.test.ActivityInstrumentationTestCase2;

import com.mapbox.mapboxgl.testapp.utils.ScreenshotUtil;

import org.junit.Before;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

/**
 * Base Espresso class for all tests, helps working with ActivityInstrumentationTestCase2
 */
public class BaseTest {

    /*
     * Shortcuts for common UI tests
     */

    protected void checkViewIsDisplayed(int id) {
        onView(withId(id))
                .check(matches(isDisplayed()));
    }

    /*
     * Screenshots logic
     */

    protected void takeNamedScreenshot(final Activity activity, final String name) {

        // Screenshots need to be taken on the UI thread
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ScreenshotUtil.take(activity, name);
            }
        });

    }

}
