package com.mapbox.mapboxsdk.maps;


import com.mapbox.android.telemetry.MapboxTelemetry;
import com.mapbox.android.telemetry.TelemetryEnabler;
import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.Mapbox;

class Events {
  static final String TWO_FINGER_TAP = "TwoFingerTap";
  static final String DOUBLE_TAP = "DoubleTap";
  static final String SINGLE_TAP = "SingleTap";
  static final String PAN = "Pan";
  static final String PINCH = "Pinch";
  static final String ROTATION = "Rotation";
  static final String PITCH = "Pitch";
  private MapboxTelemetry telemetry;

  private Events() {
    telemetry = new MapboxTelemetry(Mapbox.getApplicationContext(), Mapbox.getAccessToken(),
      BuildConfig.MAPBOX_EVENTS_USER_AGENT);
    TelemetryEnabler.State telemetryState = TelemetryEnabler.retrieveTelemetryStateFromPreferences();
    if (TelemetryEnabler.State.NOT_INITIALIZED.equals(telemetryState)
      || TelemetryEnabler.State.ENABLED.equals(telemetryState)) {
      telemetry.enable();
    }
  }

  private static class EventsHolder {
    private static final Events INSTANCE = new Events();
  }

  static MapboxTelemetry obtainTelemetry() {
    return EventsHolder.INSTANCE.telemetry;
  }
}
