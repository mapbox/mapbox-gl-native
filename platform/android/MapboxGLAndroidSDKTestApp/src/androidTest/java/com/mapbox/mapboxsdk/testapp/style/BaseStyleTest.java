package com.mapbox.mapboxsdk.testapp.style;

import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;

/**
 * Base Test class for Style tests
 */
public class BaseStyleTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return RuntimeStyleTestActivity.class;
  }
}
