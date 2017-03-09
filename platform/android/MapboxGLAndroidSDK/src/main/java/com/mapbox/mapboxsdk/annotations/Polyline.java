package com.mapbox.mapboxsdk.annotations;

import android.graphics.Color;

import com.mapbox.mapboxsdk.maps.MapboxMap;

/**
 * Polyline is a geometry feature with an unclosed list of coordinates drawn as a line
 */
public final class Polyline extends BasePointCollection {

  private int color = Color.BLACK; // default color is black
  private float width = 10; // As specified by Google API Docs (in pixels)

  Polyline() {
    super();
  }

  /**
   * Gets the color of this polyline.
   *
   * @return The color in ARGB format.
   */
  public int getColor() {
    return color;
  }

  /**
   * Gets the width of this polyline.
   *
   * @return The width in screen pixels.
   */
  public float getWidth() {
    return width;
  }

  /**
   * Sets the color of the polyline.
   *
   * @param color - the color in ARGB format
   */
  public void setColor(int color) {
    this.color = color;
    update();
  }

  /**
   * Sets the width of the polyline.
   *
   * @param width in pixels
   */
  public void setWidth(float width) {
    this.width = width;
    update();
  }

  @Override
  void update() {
    MapboxMap mapboxMap = getMapboxMap();
    if (mapboxMap != null) {
      mapboxMap.updatePolyline(this);
    }
  }
}
