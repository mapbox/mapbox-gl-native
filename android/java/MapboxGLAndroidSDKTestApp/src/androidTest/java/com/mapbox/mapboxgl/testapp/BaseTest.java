package com.mapbox.mapboxgl.testapp;

import android.support.test.InstrumentationRegistry;
import android.test.ActivityInstrumentationTestCase2;

import com.mapbox.mapboxgl.testapp.utils.ScreenshotUtil;

import org.junit.Before;

/**
 * Base Espresso class for all tests, helps working with ActivityInstrumentationTestCase2
 */
public class BaseTest
        extends ActivityInstrumentationTestCase2<MainActivity> {

    private MainActivity mActivity;
    private final static boolean screenshotOnTearDown = false;

    public BaseTest() {
        super(MainActivity.class);
    }

    /*
     * Get the activity before running every test
     */

    @Before
    public void setUp() throws Exception {
        super.setUp();
        injectInstrumentation(InstrumentationRegistry.getInstrumentation());
        mActivity = getActivity();
    }

    /*
     * Take a screenshot after every test
     */

    @Override
    protected void tearDown() throws Exception {
        if (screenshotOnTearDown) {
            try {
                // Screenshots should be taken on the UI thread
                runTestOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        ScreenshotUtil.take(getActivity(), null);
                    }
                });
            } catch (Throwable throwable) {
                throwable.printStackTrace();
            }
        }

        super.tearDown();
        mActivity = null;
    }

    protected void takeNamedScreenshot(final String name) {
        try {
            // Screenshots should be taken on the UI thread
            runTestOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ScreenshotUtil.take(getActivity(), name);
                }
            });
        } catch (Throwable throwable) {
            throwable.printStackTrace();
        }
    }

}
