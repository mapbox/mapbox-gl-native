package com.mapbox.mapboxsdk.testapp;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on MapFragmentActivity
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class MapFragmentActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<MapFragmentActivity> mActivityRule = new ActivityTestRule<>(
            MapFragmentActivity.class);

    private MapFragmentActivity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.fragment_container);
    }

    // FIXME need a way to check if fragment was correctly added to Activity
}
