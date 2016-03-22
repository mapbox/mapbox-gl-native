package com.mapbox.mapboxsdk.testapp.espresso;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.ScrollByActivity;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Tests on ScrollByActivity
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class ScrollByActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<ScrollByActivity> mActivityRule = new ActivityTestRule<>(ScrollByActivity.class);

    @Test
    public void testSanity() {
        checkViewIsDisplayed(R.id.mapView);
    }
}

