package com.mapbox.mapboxsdk.maps;

import com.mapbox.android.telemetry.SessionInterval;
import com.mapbox.mapboxsdk.module.telemetry.TelemetryImpl;

/**
 * @deprecated use {@link TelemetryImpl} instead.
 */
@Deprecated
public class Telemetry {

  @Deprecated
  public static void initialize() {
    // no-op
  }

  /**
   * Set the debug logging state of telemetry.
   *
   * @param debugLoggingEnabled true to enable logging
   * @deprecated use {@link TelemetryImpl#updateDebugLoggingEnabled(boolean)} instead
   */
  @Deprecated
  public static void updateDebugLoggingEnabled(boolean debugLoggingEnabled) {
    TelemetryImpl.updateDebugLoggingEnabled(debugLoggingEnabled);
  }

  /**
   * Update the telemetry rotation session id interval
   *
   * @param interval the selected session interval
   * @return true if rotation session id was updated
   * @deprecated use {@link TelemetryImpl#setSessionIdRotationInterval(int)} instead
   */
  @Deprecated
  public static boolean updateSessionIdRotationInterval(SessionInterval interval) {
    return TelemetryImpl.updateSessionIdRotationInterval(interval);
  }

  /**
   * Method to be called when an end-user has selected to participate in telemetry collection.
   *
   * @deprecated use {@link TelemetryImpl#setUserTelemetryRequestState(boolean)}
   * with parameter true instead
   */
  @Deprecated
  public static void enableOnUserRequest() {
    TelemetryImpl.enableOnUserRequest();
  }

  /**
   * Method to be called when an end-user has selected to opt-out of telemetry collection.
   *
   * @deprecated use {@link TelemetryImpl#setUserTelemetryRequestState(boolean)}
   * with parameter false instead
   */
  @Deprecated
  public static void disableOnUserRequest() {
    TelemetryImpl.disableOnUserRequest();
  }
}