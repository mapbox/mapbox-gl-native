package com.mapbox.mapboxsdk.telemetry;

import android.graphics.PointF;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.Projection;

import java.io.Serializable;
import java.util.Hashtable;

import timber.log.Timber;

/**
 * Constants for Telemetry Metadata
 */
public class MapboxEvent implements Serializable {
  public static final int VERSION_NUMBER = 2;
  public static final String MAPBOX_EVENTS_BASE_URL = "https://events.mapbox.com";
  public static final String SOURCE_MAPBOX = "mapbox";

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
  public static final String KEY_ZOOM = "zoom";

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
  public static final String ATTRIBUTE_USERID = "userId";
  public static final String ATTRIBUTE_SOURCE = "source";
  public static final String ATTRIBUTE_ENABLED_TELEMETRY = "enabled.telemetry";
  public static final String ATTRIBUTE_SESSION_ID = "sessionId";
  public static final String ATTRIBUTE_VERSION = "version";
  public static final String ATTRIBUTE_CREATED = "created";
  public static final String ATTRIBUTE_VENDOR_ID = "vendorId";
  public static final String ATTRIBUTE_APP_BUNDLE_ID = "appBundleId";
  public static final String ATTRIBUTE_MODEL = "model";
  public static final String ATTRIBUTE_OPERATING_SYSTEM = "operatingSystem";
  public static final String ATTRIBUTE_ORIENTATION = "orientation";
  public static final String ATTRIBUTE_BATTERY_LEVEL = "batteryLevel";
  public static final String ATTRIBUTE_PLUGGED_IN = "pluggedIn";
  public static final String ATTRIBUTE_APPLICATION_STATE = "applicationState";
  public static final String ATTRIBUTE_RESOLUTION = "resolution";
  public static final String ATTRIBUTE_ACCESSIBILITY_FONT_SCALE = "accessibilityFontScale";
  public static final String ATTRIBUTE_CARRIER = "carrier";
  public static final String ATTRIBUTE_CELLULAR_NETWORK_TYPE = "cellularNetworkType";
  public static final String ATTRIBUTE_WIFI = "wifi";

  /**
   * Helper method for tracking gesture events
   *
   * @param projection  Projection of the Map object
   * @param gestureId   Type of Gesture See {@see MapboxEvent#GESTURE_SINGLETAP
   *                    MapboxEvent#GESTURE_DOUBLETAP
   *                    MapboxEvent#GESTURE_TWO_FINGER_SINGLETAP
   *                    MapboxEvent#GESTURE_QUICK_ZOOM
   *                    MapboxEvent#GESTURE_PAN_START
   *                    MapboxEvent#GESTURE_PINCH_START
   *                    MapboxEvent#GESTURE_ROTATION_START
   *                    MapboxEvent#GESTURE_PITCH_START}
   * @param xCoordinate Original x screen coordinate at start of gesture
   * @param yCoordinate Original y screen cooridnate at start of gesture
   * @param zoom        Zoom level to be registered
   */
  public static void trackGestureEvent(@NonNull Projection projection, @NonNull String gestureId, float xCoordinate,
                                       float yCoordinate, double zoom) {
    LatLng tapLatLng = projection.fromScreenLocation(new PointF(xCoordinate, yCoordinate));

    // NaN and Infinite checks to prevent JSON errors at send to server time
    if (Double.isNaN(tapLatLng.getLatitude()) || Double.isNaN(tapLatLng.getLongitude())) {
      Timber.d("trackGestureEvent() has a NaN lat or lon.  Returning.");
      return;
    }

    if (Double.isInfinite(tapLatLng.getLatitude()) || Double.isInfinite(tapLatLng.getLongitude())) {
      Timber.d("trackGestureEvent() has an Infinite lat or lon.  Returning.");
      return;
    }

    Hashtable<String, Object> evt = new Hashtable<>();
    evt.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_MAP_CLICK);
    evt.put(MapboxEvent.ATTRIBUTE_CREATED, MapboxEventManager.generateCreateDate());
    evt.put(MapboxEvent.KEY_GESTURE_ID, gestureId);
    evt.put(MapboxEvent.KEY_LATITUDE, tapLatLng.getLatitude());
    evt.put(MapboxEvent.KEY_LONGITUDE, tapLatLng.getLongitude());
    evt.put(MapboxEvent.KEY_ZOOM, zoom);

    MapboxEventManager.getMapboxEventManager().pushEvent(evt);
  }

  /**
   * Helper method for tracking DragEnd gesture event
   * See {@see MapboxEvent#TYPE_MAP_DRAGEND}
   *
   * @param projection  projection of the Map object.
   * @param xCoordinate Original x screen coordinate at end of drag
   * @param yCoordinate Orginal y screen coordinate at end of drag
   * @param zoom        Zoom level to be registered
   */
  public static void trackGestureDragEndEvent(@NonNull Projection projection, float xCoordinate, float yCoordinate,
                                              double zoom) {
    LatLng tapLatLng = projection.fromScreenLocation(new PointF(xCoordinate, yCoordinate));

    // NaN and Infinite checks to prevent JSON errors at send to server time
    if (Double.isNaN(tapLatLng.getLatitude()) || Double.isNaN(tapLatLng.getLongitude())) {
      Timber.d("trackGestureDragEndEvent() has a NaN lat or lon.  Returning.");
      return;
    }

    if (Double.isInfinite(tapLatLng.getLatitude()) || Double.isInfinite(tapLatLng.getLongitude())) {
      Timber.d("trackGestureDragEndEvent() has an Infinite lat or lon.  Returning.");
      return;
    }

    Hashtable<String, Object> evt = new Hashtable<>();
    evt.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_MAP_DRAGEND);
    evt.put(MapboxEvent.ATTRIBUTE_CREATED, MapboxEventManager.generateCreateDate());
    evt.put(MapboxEvent.KEY_LATITUDE, tapLatLng.getLatitude());
    evt.put(MapboxEvent.KEY_LONGITUDE, tapLatLng.getLongitude());
    evt.put(MapboxEvent.KEY_ZOOM, zoom);

    MapboxEventManager.getMapboxEventManager().pushEvent(evt);
  }

  /**
   * Helper method for tracking map load event
   */
  public static void trackMapLoadEvent() {
    Hashtable<String, Object> evt = new Hashtable<>();
    evt.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_MAP_LOAD);
    evt.put(MapboxEvent.ATTRIBUTE_CREATED, MapboxEventManager.generateCreateDate());
    MapboxEventManager.getMapboxEventManager().pushEvent(evt);
  }
}
