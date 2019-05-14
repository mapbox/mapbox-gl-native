package com.mapbox.mapboxsdk.testapp.activity.render;

import android.content.Context;
import androidx.annotation.NonNull;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshot;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshotter;

public class RenderTestSnapshotter extends MapSnapshotter {

  RenderTestSnapshotter(@NonNull Context context, @NonNull Options options) {
    super(context, options);
  }

  @Override
  protected void addOverlay(MapSnapshot mapSnapshot) {
    // don't add an overlay
  }
}
