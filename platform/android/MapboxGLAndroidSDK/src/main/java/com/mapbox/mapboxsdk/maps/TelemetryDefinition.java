package com.mapbox.mapboxsdk.maps;

import android.os.Bundle;

import com.mapbox.mapboxsdk.offline.OfflineRegionDefinition;

/**
 * Definition of TelemetryImpl collection.
 * Use{@link TelemetryDefinition} for posting the event.
 */
public interface TelemetryDefinition {

  /**
   * Register the app user turnstile event
   */
  void onAppUserTurnstileEvent();

  /**
   * Register an end-user gesture interaction event.
   *
   * @param eventType type of gesture event occurred
   * @param latitude  the latitude value of the gesture focal point
   * @param longitude the longitude value of the gesture focal point
   * @param zoom      current zoom of the map
   * @deprecated since 7.5.0, this event is no longer supported
   */
  @Deprecated
  void onGestureInteraction(String eventType, double latitude, double longitude, double zoom);

  /**
   * Set the end-user selected state to participate or opt-out in telemetry collection.
   */
  void setUserTelemetryRequestState(boolean enabled);

  /**
   * Disables a started telemetry service for this session only.
   */
  void disableTelemetrySession();

  /**
   * Set the end-user selected state to participate or opt-out in telemetry collection.
   */
  void setDebugLoggingEnabled(boolean debugLoggingEnabled);

  /**
   * Set the telemetry rotation session id interval
   *
   * @param interval the selected session interval
   * @return true if rotation session id was updated
   */
  boolean setSessionIdRotationInterval(int interval);

  /**
   * Register an end-user offline download event.
   *
   * @param offlineDefinition the offline region definition
   */
  void onCreateOfflineRegion(OfflineRegionDefinition offlineDefinition);

  /**
   * Register a performance event.
   *
   * @param data performance event data
   */
  void onPerformanceEvent(Bundle data);
}
