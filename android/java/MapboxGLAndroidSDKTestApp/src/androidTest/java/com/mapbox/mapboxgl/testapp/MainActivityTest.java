package com.mapbox.mapboxgl.testapp;

import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.click;
import static android.support.test.espresso.action.ViewActions.doubleClick;
import static android.support.test.espresso.action.ViewActions.longClick;
import static android.support.test.espresso.action.ViewActions.swipeDown;
import static android.support.test.espresso.action.ViewActions.swipeLeft;
import static android.support.test.espresso.action.ViewActions.swipeRight;
import static android.support.test.espresso.action.ViewActions.swipeUp;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withContentDescription;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static android.support.test.espresso.matcher.ViewMatchers.withText;
import static org.hamcrest.Matchers.not;

/**
 * Tests on MainActivity
 */
public class MainActivityTest extends BaseTest {

    private final static String HOME_BUTTON_STRING = "Navigate up";

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

    /*
     * Check UI ids are visible
     */

    @Test
    public void testDrawerLayoutIsDisplayed() {
        onView(withId(R.id.drawer_layout))
                .check(matches(isDisplayed()));
    }

    @Test
    public void testToolbarIsDisplayed() {
        onView(withId(R.id.toolbar))
                .check(matches(isDisplayed()));
    }

    @Test
    public void testContentFrameIsDisplayed() {
        onView(withId(R.id.content_frame))
                .check(matches(isDisplayed()));
    }

    @Test
    public void testViewFpsIsDisplayed() {
        // By default, R.id.view_fps is not displayed
        onView(withId(R.id.view_fps))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testLocationFABIsDisplayed() {
        onView(withId(R.id.locationFAB))
                .check(matches(isDisplayed()));
    }

    @Test
    public void testNavViewIsNotDisplayed() {
        // By default, nav_view not displayed
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testNavViewIsDisplayed() {
        // However, it's displayed when we click the home icon
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withId(R.id.nav_view))
                .check(matches(isDisplayed()));
    }

    /*
     * Some more tests on the map view (clicks, gestures)
     */

    @Test
    public void testClickMap() {
        onView(withId(R.id.mainMapView))
                .perform(click())
                .check(matches(isDisplayed()));
    }

    @Test
    public void testDoubleClickMap() {
        onView(withId(R.id.mainMapView))
                .perform(doubleClick())
                .check(matches(isDisplayed()));
    }

    @Test
    public void testLongClickMap() {
        onView(withId(R.id.mainMapView))
                .perform(longClick())
                .check(matches(isDisplayed()));
    }

    @Test
    public void testSwipeLeftMap() {
        onView(withId(R.id.mainMapView))
                .perform(swipeLeft())
                .check(matches(isDisplayed()));
    }

    @Test
    public void testSwipeRightMap() {
        onView(withId(R.id.mainMapView))
                .perform(swipeRight())
                .check(matches(isDisplayed()));
    }

    @Test
    public void testSwipeDownMap() {
        onView(withId(R.id.mainMapView))
                .perform(swipeDown())
                .check(matches(isDisplayed()));
    }

    @Test
    public void testSwipeUpMap() {
        onView(withId(R.id.mainMapView))
                .perform(swipeUp())
                .check(matches(isDisplayed()));
    }

    /*
     * Test the main drawer options
     */

    @Test
    public void testNavViewActionDebug() {
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.action_debug))
                .perform(click());
        
        // Clicking the item closes the drawer
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testNavViewActionPointAnnotations() {
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.action_point_annotations))
                .perform(click());

        // Clicking the item closes the drawer
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testNavViewActionCompass() {
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.action_compass))
                .perform(click());

        // Clicking the item closes the drawer
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testNavViewStyleMapboxStreets() {
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.styleMapboxStreets))
                .perform(click());

        // Clicking the item closes the drawer
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testNavViewStyleEmerald() {
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.styleEmerald))
                .perform(click());

        // Clicking the item closes the drawer
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testNavViewStyleLight() {
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.styleLight))
                .perform(click());

        // Clicking the item closes the drawer
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testNavViewStyleDark() {
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.styleDark))
                .perform(click());

        // Clicking the item closes the drawer
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    @Test
    public void testNavViewStyleSatellite() {
        onView(withContentDescription(HOME_BUTTON_STRING))
                .perform(click());
        onView(withText(R.string.styleSatellite))
                .perform(click());

        // Clicking the item closes the drawer
        onView(withId(R.id.nav_view))
                .check(matches(not(isDisplayed())));
    }

    /*
     * We can also check the map inner elements are visible
     */

    @Test
    public void testMapIconContentDescription() {
        // Mapbox logo
        onView(withContentDescription(getActivity().getResources()
                .getString(R.string.mapboxIconContentDescription)))
                .check(matches(isDisplayed()));
    }

    @Test
    public void testMapCompassContentDescription() {
        // Map compass
        onView(withContentDescription(getActivity().getResources()
                .getString(R.string.compassContentDescription)))
                .check(matches(isDisplayed()));
    }

    @Test
    public void testMapAttributionsIconContentDescription() {
        // Attribution icon
        onView(withContentDescription(getActivity().getResources()
                .getString(R.string.attributionsIconContentDescription)))
                .check(matches(isDisplayed()));
    }

}
