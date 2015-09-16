package com.mapbox.mapboxgl.testapp;

import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

/**
 * Tests on SecondMapActivity.
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class SecondMapActivityTest extends BaseTest {

    @Rule
    public ActivityTestRule<SecondMapActivity> mActivityRule = new ActivityTestRule<>(
            SecondMapActivity.class);

    private SecondMapActivity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();
    }

    @Test
    public void testSanity() {
        onView(withId(R.id.secondMapView))
                .check(matches(isDisplayed()));
    }

}
