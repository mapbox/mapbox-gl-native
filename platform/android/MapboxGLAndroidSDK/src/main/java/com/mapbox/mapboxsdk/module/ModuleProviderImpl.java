package com.mapbox.mapboxsdk.module;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.ModuleProvider;
import com.mapbox.mapboxsdk.http.HttpRequest;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.module.http.HttpRequestImpl;
import com.mapbox.mapboxsdk.module.loader.LibraryLoaderImpl;
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

  @Nullable
  @Override
  public LibraryLoader obtainLibraryLoader() {
    return new LibraryLoaderImpl();
  }
}
