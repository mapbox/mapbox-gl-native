package com.mapbox.weartestapp.utils;

import android.support.wearable.view.DefaultOffsettingHelper;
import android.support.wearable.view.WearableRecyclerView;
import android.view.View;

public class OffsettingHelper extends DefaultOffsettingHelper {

  /**
   * How much should we scale the icon at most.
   */
  private static final float MAX_ICON_PROGRESS = 0.65f;

  private float progressToCenter;

  public OffsettingHelper() {
  }

  @Override
  public void updateChild(View child, WearableRecyclerView parent) {
    super.updateChild(child, parent);

    // Figure out % progress from top to bottom
    float centerOffset = ((float) child.getHeight() / 2.0f) / (float) parent.getHeight();
    float yRelativeToCenterOffset = (child.getY() / parent.getHeight()) + centerOffset;

    // Normalize for center
    progressToCenter = Math.abs(0.5f - yRelativeToCenterOffset);
    // Adjust to the maximum scale
    progressToCenter = Math.min(progressToCenter, MAX_ICON_PROGRESS);

    child.setScaleX(1 - progressToCenter);
    child.setScaleY(1 - progressToCenter);
  }

  @Override
  protected void adjustAnchorOffsetXY(View child, float[] anchorOffsetXY) {
    anchorOffsetXY[0] = child.getHeight() / 2.0f;
  }
}
