package com.mapbox.mapboxsdk.testapp.espresso;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import com.mapbox.mapboxsdk.testapp.activity.annotation.BulkMarkerActivity;
import com.mapbox.mapboxsdk.testapp.R;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on BulkMarkerActivity
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class BulkMarkerActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<BulkMarkerActivity> mActivityRule = new ActivityTestRule<>(
            BulkMarkerActivity.class);

    private BulkMarkerActivity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.mapView);
    }
}
