package com.mapbox.mapboxsdk;

/**
 * Using this class you can enable a strict mode that will throw the {@link MapStrictModeException}
 * whenever the map would fail silently otherwise.
 */
public class MapStrictMode {
  private static volatile boolean strictModeEnabled;

  /**
   * Set the strict mode that will throw the {@link MapStrictModeException}
   * whenever the map would fail silently otherwise.
   *
   * @param strictModeEnabled true to enable the strict mode, false otherwise
   */
  public static synchronized void setStrictModeEnabled(boolean strictModeEnabled) {
    MapStrictMode.strictModeEnabled = strictModeEnabled;
  }

  /**
   * Internal use. Called whenever the strict mode violation occurs.
   */
  public static void strictModeViolation(String message) {
    if (strictModeEnabled) {
      throw new MapStrictModeException(message);
    }
  }

  /**
   * Internal use. Called whenever the strict mode violation occurs.
   */
  public static void strictModeViolation(String message, Throwable throwable) {
    if (strictModeEnabled) {
      throw new MapStrictModeException(String.format("%s - %s", message, throwable));
    }
  }

  /**
   * Internal use. Called whenever the strict mode violation occurs.
   */
  public static void strictModeViolation(Throwable throwable) {
    if (strictModeEnabled) {
      throw new MapStrictModeException(String.format("%s", throwable));
    }
  }
}
