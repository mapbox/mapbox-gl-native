package com.mapbox.mapboxgl.testapp;

import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

/**
 * Tests MainActivity
 */
public class MainActivityTest extends BaseTest {

    /*
     * Note that we need to keep the `test` prefix if we want to be able to run these
     * tests on AWS Device Farm, annotations are not enough:
     * https://github.com/awslabs/aws-device-farm-sample-app-for-android/issues/5#issuecomment-138258444
     */

    @Test
    public void testSanity() {
        onView(withId(R.id.mainMapView))
                .check(matches(isDisplayed()));
    }

}
