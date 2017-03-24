package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

/**
 * Multipoint is an abstract annotation for combining geographical locations.
 */
public abstract class BasePointCollection extends Annotation {

  private List<LatLng> points;
  private List<LatLng> holePoints;
  private float alpha = 1.0f;

  protected BasePointCollection() {
    super();
    points = new ArrayList<>();
  }

  /**
   * Returns a copy of the points.
   *
   * @return A {@link List} of points.
   */
  public List<LatLng> getPoints() {
    return new ArrayList<>(points);
  }

  /**
   * Sets the points of this polyline. This method will take a copy of the points, so further
   * mutations to points will have no effect on this polyline.
   *
   * @param points A {@link List} of {@link LatLng} points making up the polyline.
   */
  public void setPoints(List<LatLng> points) {
    this.points = new ArrayList<>(points);
    update();
  }

  /**
   * Add a point to the polyline.
   *
   * @param point A {@link LatLng} point to be added.
   */
  public void addPoint(LatLng point) {
    points.add(point);
    update();
  }

  /**
   * Returns a copy of the hole points.
   *
   * @return A {@link List} of points.
   */
  public List<LatLng> getHolePoints() {
    return new ArrayList<>(holePoints);
  }

  /**
   * Sets the points of this polyline hole. This method will take a copy of the points, so further
   * mutations to points will have no effect on this polyline hole.
   *
   * @param holePoints A {@link List} of {@link LatLng} points making up the polyline hole.
   */
  public void setHolePoints(List<LatLng> holePoints) {
    this.holePoints = new ArrayList<>(holePoints);
    update();
  }

  /**
   * Add a point to the polyline hole.
   *
   * @param holePoint A {@link LatLng} point to be added.
   */
  public void addHolePoint(LatLng holePoint) {
    holePoints.add(holePoint);
    update();
  }

  /**
   * Value between 0 and 1 defining the polyline alpha.
   *
   * @return float value between 0 and 1.
   */
  public float getAlpha() {
    return alpha;
  }

  /**
   * Set this {@link BasePointCollection}s alpha.
   *
   * @param alpha float value between 0 and 1.
   */
  public void setAlpha(float alpha) {
    this.alpha = alpha;
    update();
  }

  abstract void update();
}
