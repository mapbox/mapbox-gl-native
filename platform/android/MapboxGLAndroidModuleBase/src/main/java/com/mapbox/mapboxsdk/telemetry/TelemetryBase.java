package com.mapbox.mapboxsdk.telemetry;

public interface TelemetryBase {

  void onAppUserTurnstileEvent();
  void onGestureInteraction(String eventType, double latitude, double longitude, double zoom);

  void enableOnUserRequest();
  void disableOnUserRequest();
  void setDebugLoggingEnabled(boolean debugLoggingEnabled);
  void setSessionIdRotationInterval(int interval);

}
