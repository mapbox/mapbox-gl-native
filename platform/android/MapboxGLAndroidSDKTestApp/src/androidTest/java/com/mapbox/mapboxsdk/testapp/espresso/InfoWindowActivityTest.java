package com.mapbox.mapboxsdk.testapp.espresso;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import com.mapbox.mapboxsdk.testapp.activity.infowindow.InfoWindowActivity;
import com.mapbox.mapboxsdk.testapp.R;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on InfoWindowActivity
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class InfoWindowActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<InfoWindowActivity> mActivityRule = new ActivityTestRule<>(
            InfoWindowActivity.class);

    private InfoWindowActivity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.mapView);
    }
}
