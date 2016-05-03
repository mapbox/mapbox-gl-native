package com.mapbox.mapboxsdk.testapp.espresso;

import android.app.Activity;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.IdlingPolicies;
import android.support.test.espresso.IdlingResource;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.LargeTest;
import android.text.format.DateUtils;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.camera.LatLngBoundsActivity;
import com.mapbox.mapboxsdk.testapp.espresso.utils.ScreenshotUtil;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.concurrent.TimeUnit;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.click;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

@RunWith(AndroidJUnit4.class)
@LargeTest
public class
ScreenshotActivityTest {

    @Rule
    public ActivityTestRule<LatLngBoundsActivity> mActivityRule = new ActivityTestRule<>(LatLngBoundsActivity.class);

    private Activity mActivity = null;

    @Before
    public void setActivity() {
        mActivity = mActivityRule.getActivity();

        // Make sure Espresso does not time out
        IdlingPolicies.setMasterPolicyTimeout(60, TimeUnit.SECONDS);
        IdlingPolicies.setIdlingResourceTimeout(26, TimeUnit.SECONDS);
    }

    @Test
    public void testScreenshot() {
        waitFor(DateUtils.SECOND_IN_MILLIS * 5, mActivity);
    }

    private static void waitFor(long waitingTime, Activity a) {
        // Start
        onView(withId(R.id.mapView)).perform(click());

        // Make sure Espresso does not time out
        IdlingPolicies.setMasterPolicyTimeout(waitingTime * 2, TimeUnit.MILLISECONDS);
        IdlingPolicies.setIdlingResourceTimeout(waitingTime * 2, TimeUnit.MILLISECONDS);

        // Now we wait
        IdlingResource idlingResource = new ElapsedTimeIdlingResource(waitingTime);
        Espresso.registerIdlingResources(idlingResource);

        // Stop and verify
        onView(withId(R.id.mapView)).perform(click());

        ScreenshotUtil.take(a, "testing");

        // Clean up
        Espresso.unregisterIdlingResources(idlingResource);
    }

//    @Test
//    public void takeScreenshot() {
//        // Start
//        checkViewIsDisplayed(R.id.mapView);
//
//
//        // Now we wait
//        IdlingResource idlingResource = new ElapsedTimeIdlingResource(4);
//        Espresso.registerIdlingResources(idlingResource);
//
//        // We take a screenshot
//        ScreenshotUtil.take(mActivity, "test");
//
//        // Clean up
//        Espresso.unregisterIdlingResources(idlingResource);
//    }

//    @Test
//    public void testSanity() {
//        checkViewIsDisplayed(R.id.mapView);
//    }

    public static class ElapsedTimeIdlingResource implements IdlingResource {
        private final long startTime;
        private final long waitingTime;
        private ResourceCallback resourceCallback;

        public ElapsedTimeIdlingResource(long waitingTime) {
            this.startTime = System.currentTimeMillis();
            this.waitingTime = waitingTime;
        }

        @Override
        public String getName() {
            return ElapsedTimeIdlingResource.class.getName() + ":" + waitingTime;
        }

        @Override
        public boolean isIdleNow() {
            long elapsed = System.currentTimeMillis() - startTime;
            boolean idle = (elapsed >= waitingTime);
            if (idle) {
                resourceCallback.onTransitionToIdle();
            }
            return idle;
        }

        @Override
        public void registerIdleTransitionCallback(ResourceCallback resourceCallback) {
            this.resourceCallback = resourceCallback;
        }
    }
}
