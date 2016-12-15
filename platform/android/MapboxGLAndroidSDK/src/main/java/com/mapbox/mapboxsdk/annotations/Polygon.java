package com.mapbox.mapboxsdk.annotations;

import android.graphics.Color;

import com.mapbox.mapboxsdk.maps.MapboxMap;

/**
 * Polygon is a geometry annotation that's a closed loop of coordinates.
 */
public final class Polygon extends MultiPoint {

  private int fillColor = Color.BLACK; // default fillColor is black
  private int strokeColor = Color.BLACK; // default strokeColor is black

  Polygon() {
    super();
  }

  /**
   * Get the color of the fill region of the polygon.
   *
   * @return The color of the fill.
   */
  public int getFillColor() {
    return fillColor;
  }

  /**
   * Get the color fo the stroke of the polygon.
   *
   * @return The color of the stroke.
   */
  public int getStrokeColor() {
    return strokeColor;
  }

  /**
   * Sets the color of the fill region of the polygon.
   *
   * @param color The color in ARGB format.
   */
  public void setFillColor(int color) {
    fillColor = color;
    update();
  }

  /**
   * Sets the color of the stroke of the polygon.
   *
   * @param color The color in ARGB format.
   */
  public void setStrokeColor(int color) {
    strokeColor = color;
    update();
  }

  @Override
  void update() {
    MapboxMap mapboxMap = getMapboxMap();
    if (mapboxMap != null) {
      mapboxMap.updatePolygon(this);
    }
  }
}
