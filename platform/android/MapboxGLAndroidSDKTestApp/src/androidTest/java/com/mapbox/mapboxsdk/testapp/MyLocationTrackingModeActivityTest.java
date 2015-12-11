package com.mapbox.mapboxsdk.testapp;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on User LocationTrackingModeActivity
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class MyLocationTrackingModeActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<MyLocationTrackingModeActivity> mActivityRule = new ActivityTestRule<>(MyLocationTrackingModeActivity.class);

    private MyLocationTrackingModeActivity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.mapView);
    }
}