package com.mapbox.mapboxsdk.testapp.espresso;

import android.support.test.rule.ActivityTestRule;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.TiltActivity;

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
