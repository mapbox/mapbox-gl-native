package com.mapbox.mapboxsdk.testapp;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on DynamicMarkerChange
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class DynamicMarkerChangeActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<DynamicMarkerChangeActivity> mActivityRule = new ActivityTestRule<>(
            DynamicMarkerChangeActivity.class);

    @Test
     public void testSanity() {
        checkViewIsDisplayed(R.id.mapView);
    }

    @Test
    public void testFabClick() {
        ViewUtils.clickView(R.id.fab);
    }
}
