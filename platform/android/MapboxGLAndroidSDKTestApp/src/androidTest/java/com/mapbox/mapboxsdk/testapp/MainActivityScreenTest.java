package com.mapbox.mapboxsdk.testapp;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.click;
import static android.support.test.espresso.action.ViewActions.doubleClick;
import static android.support.test.espresso.matcher.ViewMatchers.withContentDescription;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static android.support.test.espresso.matcher.ViewMatchers.withText;

/**
 * Tests on MainActivity with screenshots
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class MainActivityScreenTest extends BaseTest {

    private final static String HOME_BUTTON_STRING = "Navigate up";

    @Rule
    public ActivityTestRule<MainActivity> mActivityRule = new ActivityTestRule<>(
            MainActivity.class);

    private MainActivity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.mainMapView);
    }

    /*
     * Take a screenshot of Mapbox Streets to monitor #1649
     */

    @Test
    public void testMapboxStreetsBlackAndWhite() {
        // Click home and switch to Mapbox streets
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.styleMapboxStreets))
                .perform(click());

        // Zoom in
        onView(withId(R.id.mainMapView))
                .perform(doubleClick());
        onView(withId(R.id.mainMapView))
                .perform(doubleClick());

        // Standard screenshot
        takeNamedScreenshot(mActivity, "testMapboxStreetsBlackAndWhite");
    }

}
