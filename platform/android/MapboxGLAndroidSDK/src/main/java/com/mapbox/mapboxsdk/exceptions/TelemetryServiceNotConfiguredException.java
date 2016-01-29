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
        super("Using Mapbox Android SDK requires configuring TelemetryService. See the INSTALL.md");
    }

}
