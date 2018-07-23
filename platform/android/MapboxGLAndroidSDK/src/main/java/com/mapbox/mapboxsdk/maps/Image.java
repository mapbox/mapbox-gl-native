package com.mapbox.mapboxsdk.maps;

import android.support.annotation.Keep;

@Keep
class Image {
  private final byte[] buffer;
  private final float pixelRatio;
  private final String name;
  private final int width;
  private final int height;
  private final boolean sdf;

  public Image(byte[] buffer, float pixelRatio, String name, int width, int height, boolean sdf) {
    this.buffer = buffer;
    this.pixelRatio = pixelRatio;
    this.name = name;
    this.width = width;
    this.height = height;
    this.sdf = sdf;
  }
}
