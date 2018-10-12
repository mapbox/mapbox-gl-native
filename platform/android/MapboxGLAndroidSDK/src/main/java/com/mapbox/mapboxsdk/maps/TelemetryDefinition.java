package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.offline.OfflineRegionDefinition;
import com.mapbox.mapboxsdk.offline.OfflineRegionError;
import com.mapbox.mapboxsdk.offline.OfflineRegionStatus;

/**
 * Definition of TelemetryImpl collection
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
   */
  void onGestureInteraction(String eventType, double latitude, double longitude, double zoom);

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
   * Register an end-user offline download start event.
   *
   * @param offlineDefinition the offline region definition
   */
  void onOfflineDownloadStart(@NonNull OfflineRegionDefinition offlineDefinition);

  /**
   * Register an end-user offline download end with success event.
   *
   * @param offlineDefinition the offline region definition
   */
  void onOfflineDownloadEndSuccess(@NonNull OfflineRegionDefinition offlineDefinition,
                                   @NonNull OfflineRegionStatus status);

  /**
   * Register an end-user offline download end with failure event.
   *
   * @param offlineDefinition the offline region definition
   */
  void onOfflineDownloadEndFailure(@NonNull OfflineRegionDefinition offlineDefinition,
                                   @NonNull OfflineRegionError error);

}
