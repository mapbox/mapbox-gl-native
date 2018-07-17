package com.mapbox.mapboxsdk;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import com.mapbox.mapboxsdk.http.HttpRequest;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;

/**
 * Injects concrete instances of configurable abstractions
 */
public interface ModuleProvider {

  /**
   * Create a new concrete implementation of HttpRequest.
   *
   * @return a new instance of an HttpRequest
   */
  @NonNull
  HttpRequest createHttpRequest();

  /**
   * Get the concrete implementation of TelemetryDefinition
   *
   * @return a single instance of Telemetry
   */
  @Nullable
  TelemetryDefinition obtainTelemetry();

}
