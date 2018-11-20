package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import com.mapbox.mapboxsdk.offline.OfflineRegionDefinition;

/**
 * Definition of Telemetry
 */
public interface TelemetryDefinition {

  //
  // Configuration
  //

  /**
   * Set the end-user selected state to participate or opt-out in telemetry collection.
   */
  void setUserTelemetryRequestState(boolean enabled);

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
   * Set the API base url for end point configuration
   *
   * @param baseUrl the end point url for telemetry collection
   */
  void setApiBaseUrl(@NonNull String baseUrl);

  //
  // Events
  //

  /**
   * Register an end-user offline download event.
   *
   * @param offlineDefinition the offline region definition
   */
  void onCreateOfflineRegion(OfflineRegionDefinition offlineDefinition);

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
   */
  void onGestureInteraction(String eventType, double latitude, double longitude, double zoom);

}
