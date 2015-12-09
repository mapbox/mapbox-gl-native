package com.mapbox.mapboxsdk.testapp;

import android.support.test.rule.ActivityTestRule;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

public class TiltActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<TiltActivity> mActivityRule = new ActivityTestRule<>(
            TiltActivity.class);

    private TiltActivity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.tiltMapView);
    }

}
