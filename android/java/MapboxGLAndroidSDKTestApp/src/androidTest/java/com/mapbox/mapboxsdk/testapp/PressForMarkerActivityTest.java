package com.mapbox.mapboxsdk.testapp;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on PressForMarkerActivity
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class PressForMarkerActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<PressForMarkerActivity> mActivityRule = new ActivityTestRule<>(
            PressForMarkerActivity.class);

    private PressForMarkerActivity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        // we are adding mapview on the fly.. just validating if we have a toolbar
        checkViewIsDisplayed(R.id.secondToolBar);
    }
}
