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
   * Value by which the default rotation threshold will be increased when scaling
   */
  public static final float ROTATION_THRESHOLD_INCREASE_WHEN_SCALING = 25f;

  /**
   * The currently supported minimum zoom level.
   */
  public static final float MINIMUM_ZOOM = 0.0f;

  /**
   * The currently supported maximum zoom level.
   */
  public static final float MAXIMUM_ZOOM = 25.5f;

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
   * The currently used minimum scale factor to clamp to when a quick zoom gesture occurs
   */
  public static final float MINIMUM_SCALE_FACTOR_CLAMP = 0.00f;

  /**
   * The currently used maximum scale factor to clamp to when a quick zoom gesture occurs
   */
  public static final float MAXIMUM_SCALE_FACTOR_CLAMP = 0.15f;

  /**
   * Fragment Argument Key for MapboxMapOptions
   */
  public static final String FRAG_ARG_MAPBOXMAPOPTIONS = "MapboxMapOptions";

  // Save instance state keys
  public static final String STATE_HAS_SAVED_STATE = "mapbox_savedState";
  public static final String STATE_CAMERA_POSITION = "mapbox_cameraPosition";
  public static final String STATE_ZOOM_ENABLED = "mapbox_zoomEnabled";
  public static final String STATE_ZOOM_ENABLED_CHANGE = "mapbox_zoomEnabledChange";
  public static final String STATE_SCROLL_ENABLED = "mapbox_scrollEnabled";
  public static final String STATE_SCROLL_ENABLED_CHANGE = "mapbox_scrollEnabledChange";
  public static final String STATE_ROTATE_ENABLED = "mapbox_rotateEnabled";
  public static final String STATE_ROTATE_ENABLED_CHANGE = "mapbox_rotateEnabledChange";
  public static final String STATE_TILT_ENABLED = "mapbox_tiltEnabled";
  public static final String STATE_TILT_ENABLED_CHANGE = "mapbox_tiltEnabledChange";
  public static final String STATE_ZOOM_CONTROLS_ENABLED = "mapbox_zoomControlsEnabled";
  public static final String STATE_DOUBLE_TAP_ENABLED = "mapbox_doubleTapEnabled";
  public static final String STATE_DOUBLE_TAP_ENABLED_CHANGE = "mapbox_doubleTapEnabledChange";
  public static final String STATE_DEBUG_ACTIVE = "mapbox_debugActive";
  public static final String STATE_STYLE_URL = "mapbox_styleUrl";
  public static final String STATE_MY_LOCATION_ENABLED = "mapbox_myLocationEnabled";
  public static final String STATE_MY_LOCATION_TRACKING_MODE = "mapbox_myLocationTracking";
  public static final String STATE_MY_BEARING_TRACKING_MODE = "mapbox_myBearingTracking";
  public static final String STATE_MY_LOCATION_TRACKING_DISMISS = "mapbox_myLocationTrackingDismiss";
  public static final String STATE_MY_BEARING_TRACKING_DISMISS = "mapbox_myBearingTrackingDismiss";
  public static final String STATE_COMPASS_ENABLED = "mapbox_compassEnabled";
  public static final String STATE_COMPASS_GRAVITY = "mapbox_compassGravity";
  public static final String STATE_COMPASS_MARGIN_LEFT = "mapbox_compassMarginLeft";
  public static final String STATE_COMPASS_MARGIN_TOP = "mapbox_compassMarginTop";
  public static final String STATE_COMPASS_MARGIN_RIGHT = "mapbox_compassMarginRight";
  public static final String STATE_COMPASS_MARGIN_BOTTOM = "mapbox_compassMarginBottom";
  public static final String STATE_COMPASS_FADE_WHEN_FACING_NORTH = "mapbox_compassFade";
  public static final String STATE_COMPASS_IMAGE_BITMAP = "mapbox_compassImage";
  public static final String STATE_LOGO_GRAVITY = "mapbox_logoGravity";
  public static final String STATE_LOGO_MARGIN_LEFT = "mapbox_logoMarginLeft";
  public static final String STATE_LOGO_MARGIN_TOP = "mapbox_logoMarginTop";
  public static final String STATE_LOGO_MARGIN_RIGHT = "mapbox_logoMarginRight";
  public static final String STATE_LOGO_MARGIN_BOTTOM = "mapbox_logoMarginBottom";
  public static final String STATE_LOGO_ENABLED = "mapbox_logoEnabled";
  public static final String STATE_ATTRIBUTION_GRAVITY = "mapbox_attrGravity";
  public static final String STATE_ATTRIBUTION_MARGIN_LEFT = "mapbox_attrMarginLeft";
  public static final String STATE_ATTRIBUTION_MARGIN_TOP = "mapbox_attrMarginTop";
  public static final String STATE_ATTRIBUTION_MARGIN_RIGHT = "mapbox_attrMarginRight";
  public static final String STATE_ATTRIBUTION_MARGIN_BOTTOM = "mapbox_atrrMarginBottom";
  public static final String STATE_ATTRIBUTION_ENABLED = "mapbox_atrrEnabled";
  public static final String STATE_LOCATION_CHANGE_ANIMATION_ENABLED = "mapbox_locationChangeAnimationEnabled";
  public static final String STATE_USING_CUSTOM_LOCATION_SOURCE = "mapbox_usingCustomLocationSource";
  public static final String STATE_LOCATION_VIEW_ENABLED = "mapbox_locViewEnabled";
  public static final String STATE_LOCATION_VIEW_FOREGROUND_DRAWABLE = "mapbox_locViewForegroundDrawable";
  public static final String STATE_LOCATION_VIEW_FOREGROUND_BEARING_DRAWABLE = "mapbox_locViewBearingDrawable";
  public static final String STATE_LOCATION_VIEW_FOREGROUND_TINT_COLOR = "mapbox_locViewForegroundTintColor";
  public static final String STATE_LOCATION_VIEW_BACKGROUND_DRAWABLE = "mapbox_locViewBackgroundDrawable";
  public static final String STATE_LOCATION_VIEW_BACKGROUND_OFFSET = "mapbox_locViewBackgroundOffset";
  public static final String STATE_LOCATION_VIEW_BACKGROUND_TINT_COLOR = "mapbox_locViewBackgroundTintColor";
  public static final String STATE_LOCATION_VIEW_ACCURACY_ALPHA = "mapbox_locViewAccuracyAlpha";
  public static final String STATE_LOCATION_VIEW_ACCURACY_TINT_COLOR = "mapbox_locViewAccuracyTintColor";
  public static final String STATE_LOCATION_VIEW_ACCURACY_THRESHOLD = "mapbox_locViewAccuracyThreshold";
  public static final String STATE_LOCATION_VIEW_PADDING = "mapbox_locViewPadding";
  public static final String STATE_DESELECT_MARKER_ON_TAP = "mapbox_deselectMarkerOnTap";
  public static final String STATE_USER_FOCAL_POINT = "mapbox_userFocalPoint";

}
