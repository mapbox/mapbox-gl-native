package com.mapbox.mapboxsdk.maps;

class Image {
  private byte[] buffer;
  private float pixelRatio;
  private String name;
  private int width;
  private int height;

  public Image(byte[] buffer, float pixelRatio, String name, int width, int height) {
    this.buffer = buffer;
    this.pixelRatio = pixelRatio;
    this.name = name;
    this.width = width;
    this.height = height;
  }
}
