package com.mapbox.mapboxsdk.exceptions;

import android.os.Bundle;

import com.mapbox.mapboxsdk.maps.MapView;

/**
 * A {@code TelemetryServiceNotConfiguredException} is thrown by {@link MapView} when it checks and finds that
 * TelemetryService has not been configured in the app's AndroidManifest.xml {@link MapView#onCreate(Bundle)}
 *
 * @see MapView#onCreate(Bundle)
 */
public class TelemetryServiceNotConfiguredException extends RuntimeException {

  public TelemetryServiceNotConfiguredException() {
    super("\nTelemetryService is not configured in your applications AndroidManifest.xml. "
      + "\nPlease add \"com.mapbox.mapboxsdk.telemetry.TelemetryService\" service in your applications "
      + "AndroidManifest.xml"
      + "\nFor an example visit http://goo.gl/cET0Jn. For more information visit https://www.mapbox.com/android-sdk/.");
  }

}