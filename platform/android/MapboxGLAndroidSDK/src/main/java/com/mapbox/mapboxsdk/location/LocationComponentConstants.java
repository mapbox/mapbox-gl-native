package com.mapbox.mapboxsdk.location;

/**
 * Contains all the constants being used for the Location layer.
 */
final class LocationComponentConstants {

  // Controls the compass update rate in milliseconds
  static final int COMPASS_UPDATE_RATE_MS = 500;

  // Sets the transition animation duration when switching camera modes.
  static final long TRANSITION_ANIMATION_DURATION_MS = 750;

  // Sets the max allowed time for the location icon animation from one LatLng to another.
  static final long MAX_ANIMATION_DURATION_MS = 2000;

  // Sets the duration of change of accuracy radius when a different value is provided.
  static final long ACCURACY_RADIUS_ANIMATION_DURATION = 250;

  // Default animation duration for zooming while tracking.
  static final long DEFAULT_TRACKING_ZOOM_ANIM_DURATION = 750;

  // Default animation duration for tilting while tracking.
  static final long DEFAULT_TRACKING_TILT_ANIM_DURATION = 1250;

  // Sources
  static final String LOCATION_SOURCE = "mapbox-location-source";
  static final String PROPERTY_GPS_BEARING = "mapbox-property-gps-bearing";
  static final String PROPERTY_COMPASS_BEARING = "mapbox-property-compass-bearing";
  static final String PROPERTY_LOCATION_STALE = "mapbox-property-location-stale";
  static final String PROPERTY_ACCURACY_RADIUS = "mapbox-property-accuracy-radius";
  static final String PROPERTY_ACCURACY_COLOR = "mapbox-property-accuracy-color";
  static final String PROPERTY_ACCURACY_ALPHA = "mapbox-property-accuracy-alpha";
  static final String PROPERTY_FOREGROUND_ICON_OFFSET = "mapbox-property-foreground-icon-offset";
  static final String PROPERTY_SHADOW_ICON_OFFSET = "mapbox-property-shadow-icon-offset";
  static final String PROPERTY_FOREGROUND_ICON = "mapbox-property-foreground-icon";
  static final String PROPERTY_BACKGROUND_ICON = "mapbox-property-background-icon";
  static final String PROPERTY_FOREGROUND_STALE_ICON = "mapbox-property-foreground-stale-icon";
  static final String PROPERTY_BACKGROUND_STALE_ICON = "mapbox-property-background-stale-icon";
  static final String PROPERTY_BEARING_ICON = "mapbox-property-shadow-icon";

  // Layers
  static final String SHADOW_LAYER = "mapbox-location-shadow";
  static final String FOREGROUND_LAYER = "mapbox-location-layer";
  static final String BACKGROUND_LAYER = "mapbox-location-stroke-layer";
  static final String ACCURACY_LAYER = "mapbox-location-accuracy-layer";
  static final String BEARING_LAYER = "mapbox-location-bearing-layer";

  // Icons
  static final String FOREGROUND_ICON = "mapbox-location-icon";
  static final String BACKGROUND_ICON = "mapbox-location-stroke-icon";
  static final String FOREGROUND_STALE_ICON = "mapbox-location-stale-icon";
  static final String BACKGROUND_STALE_ICON = "mapbox-location-background-stale-icon";
  static final String SHADOW_ICON = "mapbox-location-shadow-icon";
  static final String BEARING_ICON = "mapbox-location-bearing-icon";

  private LocationComponentConstants() {
    // Class should not be initialized
  }
}
