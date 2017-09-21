package com.mapbox.mapboxsdk.testapp.utils;

import android.support.test.espresso.IdlingResource;

import com.mapbox.mapboxsdk.testapp.activity.render.RenderTestActivity;

public class SnapshotterIdlingResource implements IdlingResource, RenderTestActivity.OnSnapshotReadyListener {

  private IdlingResource.ResourceCallback resourceCallback;
  private boolean isSnapshotReady;

  public SnapshotterIdlingResource(RenderTestActivity activity) {
    activity.setOnSnapshotReadyListener(this);
  }

  @Override
  public String getName() {
    return "SnapshotterIdlingResource";
  }

  @Override
  public boolean isIdleNow() {
    return isSnapshotReady;
  }

  @Override
  public void registerIdleTransitionCallback(ResourceCallback resourceCallback) {
    this.resourceCallback = resourceCallback;
  }

  @Override
  public void onSnapshotReady() {
    isSnapshotReady = true;
    if (resourceCallback != null) {
      resourceCallback.onTransitionToIdle();
    }
  }
}
