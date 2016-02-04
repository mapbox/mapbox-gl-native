package com.mapbox.mapboxsdk.telemetry;

import java.io.Serializable;

public class MapboxEvent implements Serializable {
    public static final int VERSION = 1;
    public static final String MGLMapboxEventsUserAgent = "Sirius/1.0/ANDROID MapboxEventsiOS/1.1";
    public static final String MGLMapboxEventsAPIBase = "https://api.tiles.mapbox.com";

    // Event Types
    public static final String MAPBOXEVENT_LOCATION = "location";

    public static final String MGLEventKeyLatitude = "lat";
    public static final String MGLEventKeyLongitude = "lng";
    public static final String MGLEventKeySpeed = "speed";
    public static final String MGLEventKeyCourse = "course";
    public static final String MGLEventKeyAltitude = "altitude";
    public static final String MGLEventKeyHorizontalAccuracy = "horizontalAccuracy";
    public static final String MGLEventKeyVerticalAccuracy = "verticalAccuracy";
}
