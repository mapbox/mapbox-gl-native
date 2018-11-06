package com.mapbox.mapboxsdk.rules;

import android.os.Build;
import android.support.annotation.NonNull;

/**
 * This rule executes a dumpsys graphics data dump after performing the test. If the API level is
 * less than 23 then this rule will do nothing since this dumpsys command isn't supported.
 */
public class GraphicsDumpsysRule extends AbstractDumpsysRule {

  private static final String GFX_INFO = "gfxinfo";
  private static final String FRAME_STATS = "framestats";

  @Override
  protected String dumpsysService() {
    return GFX_INFO;
  }

  @NonNull
  @Override
  protected String extraOptions() {
    return Build.VERSION.SDK_INT >= 23 ? FRAME_STATS : "";
  }
}
