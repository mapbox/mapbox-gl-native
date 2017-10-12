package com.mapbox.mapboxsdk.snapshotter;

import android.graphics.Bitmap;


public class Snapshot {
  private final Bitmap bitmap;

  public Snapshot(Bitmap bitmap) {
    this.bitmap = bitmap;
  }

  public Bitmap getBitmap() {
    return bitmap;
  }
}
