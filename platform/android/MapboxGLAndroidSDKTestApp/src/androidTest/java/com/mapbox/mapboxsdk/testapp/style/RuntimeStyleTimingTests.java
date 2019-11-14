package com.mapbox.mapboxsdk.testapp.style;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.mapbox.mapboxsdk.testapp.activity.BaseTest;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTimingTestActivity;

import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Basic smoke tests for adding Layer and Source as early as possible (in onCreate)
 */
@RunWith(AndroidJUnit4.class)
public class RuntimeStyleTimingTests extends BaseTest {

  @Override
  protected Class getActivityClass() {
    return RuntimeStyleTimingTestActivity.class;
  }

  @Test
  public void testGetAddRemoveLayer() {
    validateTestSetup();
    // We're good if it didn't crash
  }
}
