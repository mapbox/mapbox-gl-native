package com.mapbox.mapboxsdk.testapp.espresso;

import android.app.Activity;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import com.mapbox.mapboxsdk.testapp.DoubleMapActivity;
import com.mapbox.mapboxsdk.testapp.R;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on DoubleMapActivity
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class DoubleMapActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<DoubleMapActivity> mActivityRule = new ActivityTestRule<>(DoubleMapActivity.class);

    private Activity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.mapView);
        checkViewIsDisplayed(R.id.mini_map);
    }
}
