package com.mapbox.mapboxsdk;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
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


  /**
   * Get the concrete implementation of LibraryLoaderProvider
   *
   * @return a new instance of LibraryLoaderProvider
   */
  @NonNull
  LibraryLoaderProvider createLibraryLoaderProvider();

}
