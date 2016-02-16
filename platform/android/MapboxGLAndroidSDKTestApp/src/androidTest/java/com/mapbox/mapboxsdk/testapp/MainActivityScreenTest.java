package com.mapbox.mapboxsdk.testapp;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import com.mapbox.mapboxsdk.testapp.utils.DrawerUtils;
import com.mapbox.mapboxsdk.testapp.utils.GestureUtils;
import com.mapbox.mapboxsdk.testapp.utils.ScreenshotUtil;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.doubleClick;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

/**
 * Tests on MainActivity with screenshots
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class MainActivityScreenTest extends BaseTest {

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

    @Test
    public void testStyleEmerald() {
        DrawerUtils.openDrawer();
        DrawerUtils.clickItem(R.string.styleEmerald);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        takeNamedScreenshot(mActivity, "testEmeraldStyle");
    }

    @Test
    public void testStyleStreets() {
        DrawerUtils.openDrawer();
        DrawerUtils.clickItem(R.string.styleMapboxStreets);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        takeNamedScreenshot(mActivity, "testStreetsStyle");
    }

    @Test
    public void testStyleDark() {
        DrawerUtils.openDrawer();
        DrawerUtils.clickItem(R.string.styleDark);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        takeNamedScreenshot(mActivity, "testDarkStyle");
    }

    @Test
    public void testStyleLight() {
        DrawerUtils.openDrawer();
        DrawerUtils.clickItem(R.string.styleLight);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        takeNamedScreenshot(mActivity, "testLightStyle");
    }

    @Test
    public void testStyleSatellite() {
        DrawerUtils.openDrawer();
        DrawerUtils.clickItem(R.string.styleSatellite);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        GestureUtils.doubleClickGesture(R.id.mainMapView);
        takeNamedScreenshot(mActivity, "testSatelliteStyle");
    }

}
