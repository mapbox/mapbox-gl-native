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

  public static void updateDebugLoggingEnabled(boolean debugLoggingEnabled) {
    TelemetryHolder.INSTANCE.telemetry.updateDebugLoggingEnabled(debugLoggingEnabled);
  }

  public static boolean updateSessionIdRotationInterval(SessionInterval interval) {
    return TelemetryHolder.INSTANCE.telemetry.updateSessionIdRotationInterval(interval);
  }

  private static class TelemetryHolder {
    private static final Telemetry INSTANCE = new Telemetry();
  }

  static MapboxTelemetry obtainTelemetry() {
    return TelemetryHolder.INSTANCE.telemetry;
  }
}
