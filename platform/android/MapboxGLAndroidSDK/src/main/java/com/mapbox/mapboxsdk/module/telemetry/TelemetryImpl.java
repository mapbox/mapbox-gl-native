package com.mapbox.mapboxsdk.module.telemetry;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;

import com.mapbox.android.accounts.v1.MapboxAccounts;
import com.mapbox.android.telemetry.AppUserTurnstile;
import com.mapbox.android.telemetry.MapboxTelemetry;
import com.mapbox.android.telemetry.SessionInterval;
import com.mapbox.android.telemetry.TelemetryEnabler;
import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.offline.OfflineRegionDefinition;
import com.mapbox.mapboxsdk.offline.OfflineTilePyramidRegionDefinition;

import java.util.UUID;

public class TelemetryImpl implements TelemetryDefinition {

  private final MapboxTelemetry telemetry;
  private final Context appContext;

  public TelemetryImpl() {
    appContext = Mapbox.getApplicationContext();
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
    if (Mapbox.getSkuToken() != null) {
      turnstileEvent.setSkuId(MapboxAccounts.SKU_ID_MAPS_MAUS);
    }
    telemetry.push(turnstileEvent);
    telemetry.push(MapEventFactory.buildMapLoadEvent(new PhoneState(appContext)));
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
  public void onGestureInteraction(String eventType, double latitude, double longitude,
                                   @FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
                                     to = MapboxConstants.MAXIMUM_ZOOM) double zoom) {
    MapState state = new MapState(latitude, longitude, zoom);
    state.setGesture(eventType);
    telemetry.push(MapEventFactory.buildMapClickEvent(new PhoneState(appContext), state));
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
    telemetry.push(MapEventFactory.buildOfflineDownloadStartEvent(new PhoneState(appContext),
      offlineDefinition instanceof OfflineTilePyramidRegionDefinition ? "tileregion" : "shaperegion",
      offlineDefinition.getMinZoom(),
      offlineDefinition.getMaxZoom(),
      offlineDefinition.getStyleURL())
    );
  }

  @Override
  public void onPerformanceEvent(Bundle data) {
    if (data == null) {
      data = new Bundle();
    }
    telemetry.push(new PerformanceEvent(UUID.randomUUID().toString(), data));
  }
}