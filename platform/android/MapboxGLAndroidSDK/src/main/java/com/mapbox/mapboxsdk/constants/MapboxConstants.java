package com.mapbox.mapboxsdk.constants;

import java.util.Locale;

/**
 * MapboxConstants exposes Mapbox related constants
 */
public class MapboxConstants {

  /**
   * Default Locale for data processing (ex: String.toLowerCase(MAPBOX_LOCALE, "foo"))
   */
  public static final Locale MAPBOX_LOCALE = Locale.US;

  /**
   * Key used to store staging data server url in AndroidManifest.xml
   */
  public static final String KEY_META_DATA_STAGING_SERVER = "com.mapbox.TestEventsServer";

  /**
   * Key used to store staging data server access token in AndroidManifest.xml
   */
  public static final String KEY_META_DATA_STAGING_ACCESS_TOKEN = "com.mapbox.TestEventsAccessToken";

  /**
   * Key used to switch storage to external in AndroidManifest.xml
   */
  public static final String KEY_META_DATA_SET_STORAGE_EXTERNAL = "com.mapbox.SetStorageExternal";

  /**
   * Default value for KEY_META_DATA_SET_STORAGE_EXTERNAL (default is internal storage)
   */
  public static final boolean DEFAULT_SET_STORAGE_EXTERNAL = false;

  /**
   * Unmeasured state
   */
  public static final float UNMEASURED = -1f;

  /**
   * Default animation time
   */
  public static final int ANIMATION_DURATION = 300;

  /**
   * Default short animation time
   */
  public static final int ANIMATION_DURATION_SHORT = 150;

  /**
   * Animation time of a fling gesture
   */
  public static final long ANIMATION_DURATION_FLING_BASE = ANIMATION_DURATION_SHORT;

  /**
   * Velocity threshold for a fling gesture
   */
  public static final long VELOCITY_THRESHOLD_IGNORE_FLING = 1000;

  /**
   * The currently supported minimum zoom level.
   */
  public static final float MINIMUM_ZOOM = 0.0f;

  /**
   * The currently supported maximum zoom level.
   */
  public static final float MAXIMUM_ZOOM = 20.0f;

  /**
   * The currently supported maximum tilt value.
   */
  public static final double MAXIMUM_TILT = 60;

  /**
   * The currently supported minimum tilt value.
   */
  public static final double MINIMUM_TILT = 0;

  /**
   * The currently supported maximum direction
   */
  public static final double MAXIMUM_DIRECTION = 360;

  /**
   * The currently supported minimum direction
   */
  public static final double MINIMUM_DIRECTION = 0;

  /**
   * The currently used minimun scale factor to clamp to when a quick zoom gesture occurs
   */
  public static final float MINIMUM_SCALE_FACTOR_CLAMP = 0.65f;

  /**
   * The currently used maximum scale factor to clamp to when a quick zoom gesture occurs
   */
  public static final float MAXIMUM_SCALE_FACTOR_CLAMP = 1.35f;

  /**
   * Fragment Argument Key for MapboxMapOptions
   */
  public static final String FRAG_ARG_MAPBOXMAPOPTIONS = "MapboxMapOptions";

  // Save instance state keys
  public static final String STATE_HAS_SAVED_STATE = "savedState";
  public static final String STATE_CAMERA_POSITION = "cameraPosition";
  public static final String STATE_ZOOM_ENABLED = "zoomEnabled";
  public static final String STATE_ZOOM_ENABLED_CHANGE = "zoomEnabledChange";
  public static final String STATE_SCROLL_ENABLED = "scrollEnabled";
  public static final String STATE_SCROLL_ENABLED_CHANGE = "scrollEnabledChange";
  public static final String STATE_ROTATE_ENABLED = "rotateEnabled";
  public static final String STATE_ROTATE_ENABLED_CHANGE = "rotateEnabledChange";
  public static final String STATE_TILT_ENABLED = "tiltEnabled";
  public static final String STATE_TILT_ENABLED_CHANGE = "tiltEnabledChange";
  public static final String STATE_ZOOM_CONTROLS_ENABLED = "zoomControlsEnabled";
  public static final String STATE_DEBUG_ACTIVE = "debugActive";
  public static final String STATE_STYLE_URL = "styleUrl";
  public static final String STATE_MY_LOCATION_ENABLED = "myLocationEnabled";
  public static final String STATE_MY_LOCATION_TRACKING_MODE = "myLocationTracking";
  public static final String STATE_MY_BEARING_TRACKING_MODE = "myBearingTracking";
  public static final String STATE_MY_LOCATION_TRACKING_DISMISS = "myLocationTrackingDismiss";
  public static final String STATE_MY_BEARING_TRACKING_DISMISS = "myBearingTrackingDismiss";
  public static final String STATE_COMPASS_ENABLED = "compassEnabled";
  public static final String STATE_COMPASS_GRAVITY = "compassGravity";
  public static final String STATE_COMPASS_MARGIN_LEFT = "compassMarginLeft";
  public static final String STATE_COMPASS_MARGIN_TOP = "compassMarginTop";
  public static final String STATE_COMPASS_MARGIN_RIGHT = "compassMarginRight";
  public static final String STATE_COMPASS_MARGIN_BOTTOM = "compassMarginBottom";
  public static final String STATE_COMPASS_FADE_WHEN_FACING_NORTH = "compassFade";
  public static final String STATE_LOGO_GRAVITY = "logoGravity";
  public static final String STATE_LOGO_MARGIN_LEFT = "logoMarginLeft";
  public static final String STATE_LOGO_MARGIN_TOP = "logoMarginTop";
  public static final String STATE_LOGO_MARGIN_RIGHT = "logoMarginRight";
  public static final String STATE_LOGO_MARGIN_BOTTOM = "logoMarginBottom";
  public static final String STATE_LOGO_ENABLED = "logoEnabled";
  public static final String STATE_ATTRIBUTION_GRAVITY = "attrGravity";
  public static final String STATE_ATTRIBUTION_MARGIN_LEFT = "attrMarginLeft";
  public static final String STATE_ATTRIBUTION_MARGIN_TOP = "attrMarginTop";
  public static final String STATE_ATTRIBUTION_MARGIN_RIGHT = "attrMarginRight";
  public static final String STATE_ATTRIBUTION_MARGIN_BOTTOM = "atrrMarginBottom";
  public static final String STATE_ATTRIBUTION_ENABLED = "atrrEnabled";

  public static final String MAPBOX_SHARED_PREFERENCES_FILE = "MapboxSharedPreferences";
  public static final String MAPBOX_SHARED_PREFERENCE_KEY_VENDORID = "mapboxVendorId";
  public static final String MAPBOX_SHARED_PREFERENCE_KEY_TELEMETRY_ENABLED = "mapboxTelemetryEnabled";
  public static final String MAPBOX_SHARED_PREFERENCE_KEY_TELEMETRY_STAGING_URL = "mapboxTelemetryStagingUrl";
  public static final String MAPBOX_SHARED_PREFERENCE_KEY_TELEMETRY_STAGING_ACCESS_TOKEN =
    "mapboxTelemetryStagingAccessToken";
}
