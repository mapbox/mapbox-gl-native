package com.mapbox.mapboxsdk.telemetry;

import java.io.Serializable;

public class MapboxEvent implements Serializable {
    public static final int VERSION_NUMBER = 1;
    public static final String MGLMapboxEventsUserAgent = "MapboxEventsAndroid/1.1";
    public static final String MGLMapboxEventsAPIBase = "https://api.tiles.mapbox.com";

    // Event Types
    public static final String TYPE_TURNSTILE = "appUserTurnstile";
    public static final String TYPE_MAP_LOAD = "map.load";
    public static final String TYPE_MAP_CLICK = "map.click";
    public static final String TYPE_MAP_DRAGEND = "map.dragend";
    public static final String TYPE_LOCATION = "location";
    public static final String TYPE_VISIT = "visit";

    // Event Keys
    public static final String KEY_LATITUDE = "lat";
    public static final String KEY_LONGITUDE = "lng";
    public static final String KEY_SPEED = "speed";
    public static final String KEY_COURSE = "course";
    public static final String KEY_ALTITUDE = "altitude";
    public static final String KEY_HORIZONTAL_ACCURACY = "horizontalAccuracy";

    public static final String KEY_PUSH_ENABLED = "enabled.push";
    public static final String KEY_EMAIL_ENABLED = "enabled.email";
    public static final String KEY_GESTURE_ID = "gesture";
    public static final String KEY_ARRIVAL_DATE = "arrivalDate";
    public static final String KEY_DEPARTURE_DATE = "departureDate";

    public static final String GESTURE_SINGLETAP = "SingleTap";
    public static final String GESTURE_DOUBLETAP = "DoubleTap";
    public static final String GESTURE_TWO_FINGER_SINGLETAP = "TwoFingerTap";
    public static final String GESTURE_QUICK_ZOOM = "QuickZoom";
    public static final String GESTURE_PAN_START = "Pan";
    public static final String GESTURE_PINCH_START = "Pinch";
    public static final String GESTURE_ROTATION_START = "Rotation";
    public static final String GESTURE_PITCH_START = "Pitch";

    // Event Attributes
    public static final String ATTRIBUTE_EVENT = "event";
    public static final String ATTRIBUTE_SESSION_ID = "sessionId";
    public static final String ATTRIBUTE_VERSION = "version";
    public static final String ATTRIBUTE_CREATED = "created";
    public static final String ATTRIBUTE_VENDOR_ID = "vendorId";
    public static final String ATTRIBUTE_APP_BUNDLE_ID = "appBundleId";
    public static final String ATTRIBUTE_MODEL = "model";
    public static final String ATTRIBUTE_OPERATING_SYSTEM= "operatingSystem";
    public static final String ATTRIBUTE_ORIENTATION = "orientation";
    public static final String ATTRIBUTE_BATTERY_LEVEL = "batteryLevel";
    public static final String ATTRIBUTE_APPLICATION_STATE = "applicationState";
    public static final String ATTRIBUTE_RESOLUTION = "resolution";
    public static final String ATTRIBUTE_ACCESSIBILITY_FONT_SCALE = "accessibilityFontScale";
    public static final String ATTRIBUTE_CARRIER = "carrier";
    public static final String ATTRIBUTE_CELLULAR_NETWORK_TYPE = "cellularNetworkType";
    public static final String ATTRIBUTE_WIFI = "wifi";
}
