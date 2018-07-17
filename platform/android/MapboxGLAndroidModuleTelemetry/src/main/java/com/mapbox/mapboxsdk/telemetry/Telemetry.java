package com.mapbox.mapboxsdk.telemetry;

import android.content.Context;
import com.mapbox.android.telemetry.AppUserTurnstile;
import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.MapEventFactory;
import com.mapbox.android.telemetry.MapState;
import com.mapbox.android.telemetry.MapboxTelemetry;
import com.mapbox.android.telemetry.SessionInterval;
import com.mapbox.android.telemetry.TelemetryEnabler;
import com.mapbox.mapboxsdk.module.BuildConfig;
import com.mapbox.mapboxsdk.utils.MapboxConfigurationWrapper;

public class Telemetry implements TelemetryBase {

  private MapboxTelemetry telemetry;

  public Telemetry() {
    Context appContext = MapboxConfigurationWrapper.getContext();
    String accessToken = MapboxConfigurationWrapper.getAccessToken();
    telemetry = new MapboxTelemetry(appContext, accessToken, BuildConfig.MAPBOX_EVENTS_USER_AGENT);
    TelemetryEnabler.State telemetryState = TelemetryEnabler.retrieveTelemetryStateFromPreferences();
    if (TelemetryEnabler.State.ENABLED.equals(telemetryState)) {
      telemetry.enable();
    }
  }

  @Override
  public void onAppUserTurnstileEvent() {
    AppUserTurnstile turnstileEvent = new AppUserTurnstile(BuildConfig.MAPBOX_SDK_IDENTIFIER,
      BuildConfig.MAPBOX_SDK_VERSION);
    telemetry.push(turnstileEvent);
    MapEventFactory mapEventFactory = new MapEventFactory();
    telemetry.push(mapEventFactory.createMapLoadEvent(Event.Type.MAP_LOAD));
  }

  @Override
  public void onGestureInteraction(String eventType, double latitude, double longitude, double zoom) {
    MapEventFactory mapEventFactory = new MapEventFactory();
    MapState state = new MapState(latitude, longitude, zoom);
    state.setGesture(eventType);
    telemetry.push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, state));
  }

  @Override
  public void enableOnUserRequest() {
    TelemetryEnabler.updateTelemetryState(TelemetryEnabler.State.ENABLED);
    telemetry.enable();
  }

  @Override
  public void disableOnUserRequest() {
    telemetry.disable();
    TelemetryEnabler.updateTelemetryState(TelemetryEnabler.State.DISABLED);
  }

  @Override
  public void setDebugLoggingEnabled(boolean debugLoggingEnabled) {
    telemetry.updateDebugLoggingEnabled(debugLoggingEnabled);
  }

  @Override
  public void setSessionIdRotationInterval(int interval) {
    telemetry.updateSessionIdRotationInterval(new SessionInterval(interval));
  }
}
