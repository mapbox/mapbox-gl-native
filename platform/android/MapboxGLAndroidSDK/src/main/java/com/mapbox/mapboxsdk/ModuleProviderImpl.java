package com.mapbox.mapboxsdk;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import com.mapbox.mapboxsdk.http.HttpRequest;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.module.http.HttpRequestImpl;
import com.mapbox.mapboxsdk.module.loader.LibraryLoaderProviderImpl;
import com.mapbox.mapboxsdk.module.telemetry.TelemetryImpl;

public class ModuleProviderImpl implements ModuleProvider {

  @Override
  @NonNull
  public HttpRequest createHttpRequest() {
    return new HttpRequestImpl();
  }

  @Override
  @Nullable
  public TelemetryDefinition obtainTelemetry() {
    return new TelemetryImpl();
  }

  @NonNull
  @Override
  public LibraryLoaderProvider createLibraryLoaderProvider() {
    return new LibraryLoaderProviderImpl();
  }
}
