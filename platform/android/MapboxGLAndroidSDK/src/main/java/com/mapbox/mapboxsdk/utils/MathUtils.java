package com.mapbox.mapboxsdk.utils;

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
   * Scale a value from an arbitrary range to a normalized range.
   *
   * @param x              The value to be normalized.
   * @param dataLow        lowest expected value from a data set
   * @param dataHigh       highest expected value from a data set
   * @param normalizedLow  normalized lowest value
   * @param normalizedHigh normalized highest value
   * @return The result of the normalization.
   */
  public static double normalize(double x, double dataLow, double dataHigh,
                                 double normalizedLow, double normalizedHigh) {
    return ((x - dataLow) / (dataHigh - dataLow)) * (normalizedHigh - normalizedLow) + normalizedLow;
  }
}
