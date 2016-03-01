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
        super("\nUsing Mapbox Android SDK requires configuring TelemetryService. This only requires two steps:\n    1. Include the WAKE_LOCK permission within your applications AndroidManifest.xml\n    2. Add the \"com.mapbox.mapboxsdk.telemetry.TelemetryService\" service in your applications AndroidManifest.xml\n    For more help, visit https://www.mapbox.com/android-sdk/");
    }

}
