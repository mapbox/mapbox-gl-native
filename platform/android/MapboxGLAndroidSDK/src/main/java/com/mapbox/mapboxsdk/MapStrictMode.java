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
  public static void strictModeViolation(String message, Exception exception) {
    if (strictModeEnabled) {
      throw new MapStrictModeException(String.format("%s - %s", message, exception));
    }
  }

  /**
   * Internal use. Called whenever the strict mode violation occurs.
   */
  public static void strictModeViolation(String message, Error error) {
    if (strictModeEnabled) {
      throw new MapStrictModeException(String.format("%s - %s", message, error));
    }
  }

  /**
   * Internal use. Called whenever the strict mode violation occurs.
   */
  public static void strictModeViolation(Error error) {
    if (strictModeEnabled) {
      throw new MapStrictModeException(String.format("%s", error));
    }
  }

  /**
   * Internal use. Called whenever the strict mode violation occurs.
   */
  public static void strictModeViolation(Exception exception) {
    if (strictModeEnabled) {
      throw new MapStrictModeException(String.format("%s", exception));
    }
  }
}
