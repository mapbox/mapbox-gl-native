package com.mapbox.mapboxsdk;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import com.mapbox.mapboxsdk.http.HttpRequest;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.module.http.HttpRequestImpl;
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
    // TODO remove singleton with next major release,
    // this is needed to make static methods on TelemetryImpl
    // backwards compatible without breaking semver
    return TelemetryImpl.getInstance();
  }
}
