package com.mapbox.mapboxsdk.annotations;

import android.graphics.Color;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import java.util.ArrayList;
import java.util.List;

/**
 * Polygon is a geometry annotation that's a closed loop of coordinates.
 */
public final class Polygon extends BasePointCollection {

  private int fillColor = Color.BLACK; // default fillColor is black
  private int strokeColor = Color.BLACK; // default strokeColor is black
  private List<List<LatLng>> holes;

  Polygon() {
    super();
    holes = new ArrayList<>();
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
   * Get the color of the stroke of the polygon.
   *
   * @return The color of the stroke.
   */
  public int getStrokeColor() {
    return strokeColor;
  }

  /**
   * Returns a copy of the holes.
   *
   * @return A {@link List} of {@link List} of {@link LatLng} points making up the holes.
   */
  public List<List<LatLng>> getHoles() {
    return new ArrayList<>(holes);
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

  /**
   * Sets the holes of this polygon. This method will take a copy of the holes, so further
   * mutations to holes will have no effect on this polygon.
   *
   * @param holes A {@link List} of {@link List} of {@link LatLng} points making up the holes.
   */
  public void setHoles(List<? extends List<LatLng>> holes) {
    this.holes = new ArrayList<>(holes);
    update();
  }

  /**
   * Add a hole to the polygon.
   *
   * @param hole A {@link List} of {@link List} of {@link LatLng} points making up the hole to be added.
   */
  void addHole(List<LatLng> hole) {
    holes.add(hole);
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
