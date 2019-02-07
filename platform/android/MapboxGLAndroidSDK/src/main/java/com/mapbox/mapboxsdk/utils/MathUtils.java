package com.mapbox.mapboxsdk.utils;

import android.graphics.PointF;
import android.support.annotation.Nullable;

// TODO Remove this class if we finally include it within MAS 3.x (GeoJSON)
public class MathUtils {

  /**
   * Test a value in specified range, returning minimum if it's below, and maximum if it's above
   *
   * @param value Value to test
   * @param min   Minimum value of range
   * @param max   Maximum value of range
   * @return value if it's between min and max, min if it's below, max if it's above
   */
  public static double clamp(double value, double min, double max) {
    return Math.max(min, Math.min(max, value));
  }

  /**
   * Test a value in specified range, returning minimum if it's below, and maximum if it's above
   *
   * @param value Value to test
   * @param min   Minimum value of range
   * @param max   Maximum value of range
   * @return value if it's between min and max, min if it's below, max if it's above
   */
  public static float clamp(float value, float min, float max) {
    return Math.max(min, Math.min(max, value));
  }

  /**
   * Constrains value to the given range (including min, excluding max) via modular arithmetic.
   * <p>
   * Same formula as used in Core GL (wrap.hpp)
   * std::fmod((std::fmod((value - min), d) + d), d) + min;
   *
   * @param value Value to wrap
   * @param min   Minimum value
   * @param max   Maximum value
   * @return Wrapped value
   */
  public static double wrap(double value, double min, double max) {
    double delta = max - min;

    double firstMod = (value - min) % delta;
    double secondMod = (firstMod + delta) % delta;

    return secondMod + min;
  }

  /**
   * Look for the intersection point of two segments, if exist.
   *
   * @param point0 first point of the first segment
   * @param point1 second point of the first segment
   * @param point2 first point of the second segment
   * @param point3 second point of the second segment
   * @return intersection point, if exists
   */
  @Nullable
  public static PointF findSegmentsIntersection(PointF point0, PointF point1, PointF point2, PointF point3) {
    // Java port of one of the algorithms discussed in https://stackoverflow.com/questions/563198/
    float dx1 = point1.x - point0.x;
    float dy1 = point1.y - point0.y;
    float dx2 = point3.x - point2.x;
    float dy2 = point3.y - point2.y;
    float dx3 = point0.x - point2.x;
    float dy3 = point0.y - point2.y;

    float d = dx1 * dy2 - dx2 * dy1;

    if (d != 0) {
      float s = dx1 * dy3 - dx3 * dy1;
      if ((s <= 0 && d < 0 && s >= d) || (s >= 0 && d > 0 && s <= d)) {
        float t = dx2 * dy3 - dx3 * dy2;
        if ((t <= 0 && d < 0 && t > d) || (t >= 0 && d > 0 && t < d)) {
          t = t / d;
          return new PointF(point0.x + t * dx1, point0.y + t * dy1);
        }
      }
    }
    return null;
  }

  /**
   * Returns a distance between two points.
   *
   * @param point0 first point
   * @param point1 second point
   * @return distance in pixels
   */
  public static float distance(PointF point0, PointF point1) {
    return (float) Math.sqrt(Math.pow(point1.x - point0.x, 2) + Math.pow(point1.y - point0.y, 2));
  }

  /**
   * Rounds a number.
   * @param value original value
   * @param precision expected precision
   * @return rounded number
   */
  public static double round(double value, int precision) {
    int scale = (int) Math.pow(10, precision);
    return (double) Math.round(value * scale) / scale;
  }
}
