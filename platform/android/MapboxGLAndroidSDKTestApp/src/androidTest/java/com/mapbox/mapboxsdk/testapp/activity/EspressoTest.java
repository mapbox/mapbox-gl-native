package com.mapbox.mapboxsdk.testapp.activity;

import android.support.test.espresso.IdlingResource;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.LoadStyleIdlingResource;

/**
 * Base class for all tests using EspressoTestActivity as wrapper.
 * <p>
 * Uses {@link LoadStyleIdlingResource} to load "assets/streets.json" as style.
 * </p>
 */
public class EspressoTest extends BaseTest {

  @Override
  protected IdlingResource generateIdlingResource() {
    return new LoadStyleIdlingResource(rule.getActivity());
  }

  @Override
  protected final Class getActivityClass() {
    return EspressoTestActivity.class;
  }
}
