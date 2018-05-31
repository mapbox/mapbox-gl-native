package com.mapbox.mapboxsdk.maps;


import com.mapbox.android.telemetry.MapboxTelemetry;
import com.mapbox.android.telemetry.SessionInterval;
import com.mapbox.android.telemetry.TelemetryEnabler;
import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.Mapbox;

public class Telemetry {
  static final String TWO_FINGER_TAP = "TwoFingerTap";
  static final String DOUBLE_TAP = "DoubleTap";
  static final String SINGLE_TAP = "SingleTap";
  static final String PAN = "Pan";
  static final String PINCH = "Pinch";
  static final String ROTATION = "Rotation";
  static final String PITCH = "Pitch";
  private MapboxTelemetry telemetry;

  private Telemetry() {
    telemetry = new MapboxTelemetry(Mapbox.getApplicationContext(), Mapbox.getAccessToken(),
      BuildConfig.MAPBOX_EVENTS_USER_AGENT);
    TelemetryEnabler.State telemetryState = TelemetryEnabler.retrieveTelemetryStateFromPreferences();
    if (TelemetryEnabler.State.ENABLED.equals(telemetryState)) {
      telemetry.enable();
    }
  }

  public static void initialize() {
    obtainTelemetry();
  }

  /**
   * Set the debug logging state of telemetry.
   *
   * @param debugLoggingEnabled true to enable logging
   */
  public static void updateDebugLoggingEnabled(boolean debugLoggingEnabled) {
    TelemetryHolder.INSTANCE.telemetry.updateDebugLoggingEnabled(debugLoggingEnabled);
  }

  /**
   * Update the telemetry rotation session id interval
   *
   * @param interval the selected session interval
   * @return true if rotation session id was updated
   */
  public static boolean updateSessionIdRotationInterval(SessionInterval interval) {
    return TelemetryHolder.INSTANCE.telemetry.updateSessionIdRotationInterval(interval);
  }

  /**
   * Method to be called when an end-user has selected to participate in telemetry collection.
   */
  public static void enableOnUserRequest() {
    TelemetryEnabler.updateTelemetryState(TelemetryEnabler.State.ENABLED);
    TelemetryHolder.INSTANCE.telemetry.enable();
  }

  /**
   * Method to be called when an end-user has selected to opt-out of telemetry collection.
   */
  public static void disableOnUserRequest() {
    Telemetry.obtainTelemetry().disable();
    TelemetryEnabler.updateTelemetryState(TelemetryEnabler.State.DISABLED);
  }

  private static class TelemetryHolder {
    private static final Telemetry INSTANCE = new Telemetry();
  }

  static MapboxTelemetry obtainTelemetry() {
    return TelemetryHolder.INSTANCE.telemetry;
  }
}
