package com.mapbox.mapboxsdk.module.telemetry;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import com.mapbox.android.telemetry.AppUserTurnstile;
import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.MapEventFactory;
import com.mapbox.android.telemetry.MapState;
import com.mapbox.android.telemetry.MapboxTelemetry;
import com.mapbox.android.telemetry.TelemetryEnabler;
import com.mapbox.android.telemetry.SessionInterval;

import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.offline.OfflineRegionDefinition;
import com.mapbox.mapboxsdk.offline.OfflineTilePyramidRegionDefinition;

public class TelemetryImpl implements TelemetryDefinition {

  @Nullable
  private MapboxTelemetry telemetry;

  public TelemetryImpl() {
    Context appContext = Mapbox.getApplicationContext();
    String accessToken = Mapbox.getAccessToken();
    telemetry = new MapboxTelemetry(appContext, accessToken, BuildConfig.MAPBOX_EVENTS_USER_AGENT);
    TelemetryEnabler.State telemetryState = TelemetryEnabler.retrieveTelemetryStateFromPreferences();
    if (TelemetryEnabler.State.ENABLED.equals(telemetryState)) {
      telemetry.enable();
    }
  }

  /**
   * Register the app user turnstile event
   */
  @Override
  public void onAppUserTurnstileEvent() {
    AppUserTurnstile turnstileEvent = new AppUserTurnstile(BuildConfig.MAPBOX_SDK_IDENTIFIER,
      BuildConfig.MAPBOX_SDK_VERSION);
    telemetry.push(turnstileEvent);
    MapEventFactory mapEventFactory = new MapEventFactory();
    telemetry.push(mapEventFactory.createMapLoadEvent(Event.Type.MAP_LOAD));
  }

  /**
   * Register an end-user gesture interaction event.
   *
   * @param eventType type of gesture event occurred
   * @param latitude  the latitude value of the gesture focal point
   * @param longitude the longitude value of the gesture focal point
   * @param zoom      current zoom of the map
   */
  @Override
  public void onGestureInteraction(String eventType, double latitude, double longitude, double zoom) {
    MapEventFactory mapEventFactory = new MapEventFactory();
    MapState state = new MapState(latitude, longitude, zoom);
    state.setGesture(eventType);
    telemetry.push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, state));
  }

  /**
   * Set the end-user selected state to participate or opt-out in telemetry collection.
   */
  @Override
  public void setUserTelemetryRequestState(boolean enabledTelemetry) {
    if (enabledTelemetry) {
      TelemetryEnabler.updateTelemetryState(TelemetryEnabler.State.ENABLED);
      telemetry.enable();
    } else {
      telemetry.disable();
      TelemetryEnabler.updateTelemetryState(TelemetryEnabler.State.DISABLED);
    }
  }

  /**
   * Set the debug logging state of telemetry.
   *
   * @param debugLoggingEnabled true to enable logging
   */
  @Override
  public void setDebugLoggingEnabled(boolean debugLoggingEnabled) {
    telemetry.updateDebugLoggingEnabled(debugLoggingEnabled);
  }

  /**
   * Set the telemetry rotation session id interval
   *
   * @param interval the selected session interval
   * @return true if rotation session id was updated
   */
  @Override
  public boolean setSessionIdRotationInterval(int interval) {
    return telemetry.updateSessionIdRotationInterval(new SessionInterval(interval));
  }

  @Override
  public void onCreateOfflineRegion(@NonNull OfflineRegionDefinition offlineDefinition) {
    MapEventFactory mapEventFactory = new MapEventFactory();
    telemetry.push(mapEventFactory.createOfflineDownloadStartEvent(
      offlineDefinition instanceof OfflineTilePyramidRegionDefinition ? "tileregion" : "shaperegion",
      offlineDefinition.getMinZoom(),
      offlineDefinition.getMaxZoom(),
      offlineDefinition.getStyleURL())
    );
  }
}