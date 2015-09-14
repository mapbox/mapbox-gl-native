package com.mapbox.mapboxgl.testapp;

import android.support.test.InstrumentationRegistry;
import android.test.ActivityInstrumentationTestCase2;

import org.junit.Before;

/**
 * Base Espresso class for all tests, helps working with ActivityInstrumentationTestCase2
 */
public class BaseTest
        extends ActivityInstrumentationTestCase2<MainActivity> {

    private MainActivity mActivity;

    public BaseTest() {
        super(MainActivity.class);
    }

    @Before
    public void setUp() throws Exception {
        super.setUp();
        injectInstrumentation(InstrumentationRegistry.getInstrumentation());
        mActivity = getActivity();
    }

}
