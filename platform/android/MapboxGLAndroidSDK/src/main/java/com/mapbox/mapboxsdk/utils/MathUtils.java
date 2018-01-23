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
   * Convert bearing from core to match Android SDK value.
   *
   * @param nativeBearing bearing value coming from core
   * @return bearing in degrees starting from 0 rotating clockwise
   */
  public static double convertNativeBearing(double nativeBearing) {
    double direction = -nativeBearing;

    while (direction > 360) {
      direction -= 360;
    }
    while (direction < 0) {
      direction += 360;
    }
    return direction;
  }

  /**
   * Returns the smallest angle between two angles.
   *
   * @param alpha First angle in degrees
   * @param beta  Second angle in degrees
   * @return Smallest angle between two angles.
   */
  public static double differenceBetweenAngles(double alpha, double beta) {
    double phi = Math.abs(beta - alpha) % 360;
    return phi > 180 ? 360 - phi : phi;
  }
}
