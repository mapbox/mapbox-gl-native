package com.mapbox.mapboxsdk.maps;

class Image {
  private final byte[] buffer;
  private final float pixelRatio;
  private final String name;
  private final int width;
  private final int height;

  public Image(byte[] buffer, float pixelRatio, String name, int width, int height) {
    this.buffer = buffer;
    this.pixelRatio = pixelRatio;
    this.name = name;
    this.width = width;
    this.height = height;
  }
}
