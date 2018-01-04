package com.mapbox.mapboxsdk.utils;

/**
 * Comparisons from std sdk, which aren't available in API level 15 and below
 */
public class Compare {

  /**
   * @see Integer#compare(int, int)
   * @param x left side
   * @param y right side
   * @return std compare value
   */
  public static int compare(int x, int y) {
    return (x < y) ? -1 : ((x == y) ? 0 : 1);
  }

  /**
   * @see Boolean#compare(boolean, boolean)
   * @param x left side
   * @param y right side
   * @return std compare value
   */
  public static int compare(boolean x, boolean y) {
    return (x == y) ? 0 : (x ? 1 : -1);
  }
}
