package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.ColorInt;
import android.support.annotation.Dimension;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StyleRes;
import android.util.Log;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.mapboxsdk.R;

import java.util.Arrays;

/**
 * This class exposes options for the Location Component. The options can be set by defining a
 * style in your apps style.xml file and passing in directly into the {@link LocationComponent}
 * class. Alternatively, if properties need to be changed at runtime depending on a specific state,
 * you can build an instance of this class, setting the values you desire, and then passing it into
 * either the {@link LocationComponent} activation method (if it isn't initialized yet) or
 * {@link LocationComponent#applyStyle(LocationComponentOptions)}.
 * <p>
 * When the {@link #createFromAttributes(Context, int)} methods called, any attributes not found
 * inside the style will revert back to using their default set values. Likewise, when building a
 * new {@link LocationComponentOptions} class using the builder, any options neglecting to be set will
 * reset to their default values.
 * <p>
 * If you would like to keep your custom style changes while modifying a single attribute, you can
 * get the currently used options object using {@link LocationComponent#getLocationComponentOptions()}
 * and it's {@code toBuilder} method to modify a single entry while also maintaining the other
 * settings. Once your modifications have been made, you'll need to pass it back into the location
 * component using {@link LocationComponent#applyStyle(LocationComponentOptions)}.
 */
public class LocationComponentOptions implements Parcelable {

  /**
   * Default accuracy alpha
   */
  private static final float ACCURACY_ALPHA_DEFAULT = 0.15f;

  /**
   * Default icon scale factor when the map is zoomed out
   */
  private static final float MIN_ZOOM_ICON_SCALE_DEFAULT = 0.6f;

  /**
   * Default icon scale factor when the map is zoomed in
   */
  private static final float MAX_ZOOM_ICON_SCALE_DEFAULT = 1f;

  /**
   * Default map padding
   */
  private static final int[] PADDING_DEFAULT = {0, 0, 0, 0};

  /**
   * The default value which is used when the stale state is enabled
   */
  private static final long STALE_STATE_DELAY_MS = 30_000L;

  /**
   * Default animation duration multiplier
   */
  private static final float TRACKING_ANIMATION_DURATION_MULTIPLIER_DEFAULT = 1.1f;

  /**
   * Default duration of a single LocationComponent circle pulse.
   */
  private static final long CIRCLE_PULSING_DURATION_DEFAULT_MS = 1500;

  /**
   * Default number of milliseconds which pass after the LocationComponent circle pulse ends and a new pulse begins.
   */
  private static final float CIRCLE_PULSING_FREQUENCY_DEFAULT_MS = 1800;

  /**
   * Default opacity of the LocationComponent circle when it ends a single pulse.
   */
  private static final float CIRCLE_PULSING_FINAL_ALPHA_DEFAULT = 0.6f;

  private float accuracyAlpha;
  private int accuracyColor;
  private int backgroundDrawableStale;
  @Nullable
  private String backgroundStaleName;
  private int foregroundDrawableStale;
  @Nullable
  private String foregroundStaleName;
  private int gpsDrawable;
  @Nullable
  private String gpsName;
  private int foregroundDrawable;
  @Nullable
  private String foregroundName;
  private int backgroundDrawable;
  @Nullable
  private String backgroundName;
  private int bearingDrawable;
  @Nullable
  private String bearingName;
  @Nullable
  private Integer bearingTintColor;
  @Nullable
  private Integer foregroundTintColor;
  @Nullable
  private Integer backgroundTintColor;
  @Nullable
  private Integer foregroundStaleTintColor;
  @Nullable
  private Integer backgroundStaleTintColor;
  private float elevation;
  private boolean enableStaleState;
  private long staleStateTimeout;
  @Nullable
  private int[] padding;
  private float maxZoomIconScale;
  private float minZoomIconScale;
  private boolean trackingGesturesManagement;
  private float trackingInitialMoveThreshold;
  private float trackingMultiFingerMoveThreshold;
  private String layerBelow;
  private float trackingAnimationDurationMultiplier;
  private boolean compassAnimationEnabled;
  private boolean accuracyAnimationEnabled;
  private Boolean pulsingCircleEnabled;
  private Boolean pulsingCircleFadeEnabled;
  private int pulseColor;
  private float pulseSingleDuration;
  private float pulseFrequency;
  private Float pulseAlpha;
  private String pulseInterpolator;

  public LocationComponentOptions(
    float accuracyAlpha,
    int accuracyColor,
    int backgroundDrawableStale,
    @Nullable String backgroundStaleName,
    int foregroundDrawableStale,
    @Nullable String foregroundStaleName,
    int gpsDrawable,
    @Nullable String gpsName,
    int foregroundDrawable,
    @Nullable String foregroundName,
    int backgroundDrawable,
    @Nullable String backgroundName,
    int bearingDrawable,
    @Nullable String bearingName,
    @Nullable Integer bearingTintColor,
    @Nullable Integer foregroundTintColor,
    @Nullable Integer backgroundTintColor,
    @Nullable Integer foregroundStaleTintColor,
    @Nullable Integer backgroundStaleTintColor,
    float elevation,
    boolean enableStaleState,
    long staleStateTimeout,
    @Nullable int[] padding,
    float maxZoomIconScale,
    float minZoomIconScale,
    boolean trackingGesturesManagement,
    float trackingInitialMoveThreshold,
    float trackingMultiFingerMoveThreshold,
    String layerBelow,
    float trackingAnimationDurationMultiplier,
    boolean compassAnimationEnabled,
    boolean accuracyAnimationEnabled,
    Boolean pulsingCircleEnabled,
    Boolean pulsingCircleFadeEnabled,
    Integer pulsingCircleColor,
    Float pulsingCircleDuration,
    Float pulsingCircleFrequency,
    Float pulsingCircleAlpha,
    String pulsingCircleInterpolator) {
    this.accuracyAlpha = accuracyAlpha;
    this.accuracyColor = accuracyColor;
    this.backgroundDrawableStale = backgroundDrawableStale;
    this.backgroundStaleName = backgroundStaleName;
    this.foregroundDrawableStale = foregroundDrawableStale;
    this.foregroundStaleName = foregroundStaleName;
    this.gpsDrawable = gpsDrawable;
    this.gpsName = gpsName;
    this.foregroundDrawable = foregroundDrawable;
    this.foregroundName = foregroundName;
    this.backgroundDrawable = backgroundDrawable;
    this.backgroundName = backgroundName;
    this.bearingDrawable = bearingDrawable;
    this.bearingName = bearingName;
    this.bearingTintColor = bearingTintColor;
    this.foregroundTintColor = foregroundTintColor;
    this.backgroundTintColor = backgroundTintColor;
    this.foregroundStaleTintColor = foregroundStaleTintColor;
    this.backgroundStaleTintColor = backgroundStaleTintColor;
    this.elevation = elevation;
    this.enableStaleState = enableStaleState;
    this.staleStateTimeout = staleStateTimeout;
    if (padding == null) {
      throw new NullPointerException("Null padding");
    }
    this.padding = padding;
    this.maxZoomIconScale = maxZoomIconScale;
    this.minZoomIconScale = minZoomIconScale;
    this.trackingGesturesManagement = trackingGesturesManagement;
    this.trackingInitialMoveThreshold = trackingInitialMoveThreshold;
    this.trackingMultiFingerMoveThreshold = trackingMultiFingerMoveThreshold;
    this.layerBelow = layerBelow;
    this.trackingAnimationDurationMultiplier = trackingAnimationDurationMultiplier;
    this.compassAnimationEnabled = compassAnimationEnabled;
    this.accuracyAnimationEnabled = accuracyAnimationEnabled;
    this.pulsingCircleEnabled = pulsingCircleEnabled;
    this.pulsingCircleFadeEnabled = pulsingCircleFadeEnabled;
    this.pulseColor = pulsingCircleColor;
    this.pulseSingleDuration = pulsingCircleDuration;
    this.pulseFrequency = pulsingCircleFrequency;
    this.pulseAlpha = pulsingCircleAlpha;
    this.pulseInterpolator = pulsingCircleInterpolator;
  }

  /**
   * Construct a new Location Component Options class using the attributes found within a style
   * resource. It's important to note that you only need to define the attributes you plan to
   * change and can safely ignore the other attributes which will be set to their default value.
   *
   * @param context  your activity's context used for acquiring resources
   * @param styleRes the style id where your custom attributes are defined
   * @return a new {@link LocationComponentOptions} object with the settings you defined in your style
   * resource
   */
  @NonNull
  public static LocationComponentOptions createFromAttributes(@NonNull Context context,
                                                              @StyleRes int styleRes) {

    TypedArray typedArray = context.obtainStyledAttributes(
      styleRes, R.styleable.mapbox_LocationComponent);

    LocationComponentOptions.Builder builder = new LocationComponentOptions.Builder()
      .enableStaleState(true)
      .staleStateTimeout(STALE_STATE_DELAY_MS)
      .maxZoomIconScale(MAX_ZOOM_ICON_SCALE_DEFAULT)
      .minZoomIconScale(MIN_ZOOM_ICON_SCALE_DEFAULT)
      .padding(PADDING_DEFAULT);

    builder.foregroundDrawable(typedArray.getResourceId(
      R.styleable.mapbox_LocationComponent_mapbox_foregroundDrawable, -1));
    if (typedArray.hasValue(R.styleable.mapbox_LocationComponent_mapbox_foregroundTintColor)) {
      builder.foregroundTintColor(typedArray.getColor(
        R.styleable.mapbox_LocationComponent_mapbox_foregroundTintColor, -1));
    }
    builder.backgroundDrawable(typedArray.getResourceId(
      R.styleable.mapbox_LocationComponent_mapbox_backgroundDrawable, -1));
    if (typedArray.hasValue(R.styleable.mapbox_LocationComponent_mapbox_backgroundTintColor)) {
      builder.backgroundTintColor(typedArray.getColor(
        R.styleable.mapbox_LocationComponent_mapbox_backgroundTintColor, -1));
    }
    builder.foregroundDrawableStale(typedArray.getResourceId(
      R.styleable.mapbox_LocationComponent_mapbox_foregroundDrawableStale, -1));
    if (typedArray.hasValue(R.styleable.mapbox_LocationComponent_mapbox_foregroundStaleTintColor)) {
      builder.foregroundStaleTintColor(typedArray.getColor(
        R.styleable.mapbox_LocationComponent_mapbox_foregroundStaleTintColor, -1));
    }
    builder.backgroundDrawableStale(typedArray.getResourceId(
      R.styleable.mapbox_LocationComponent_mapbox_backgroundDrawableStale, -1));
    if (typedArray.hasValue(R.styleable.mapbox_LocationComponent_mapbox_backgroundStaleTintColor)) {
      builder.backgroundStaleTintColor(typedArray.getColor(
        R.styleable.mapbox_LocationComponent_mapbox_backgroundStaleTintColor, -1));
    }
    builder.bearingDrawable(typedArray.getResourceId(
      R.styleable.mapbox_LocationComponent_mapbox_bearingDrawable, -1));
    if (typedArray.hasValue(R.styleable.mapbox_LocationComponent_mapbox_bearingTintColor)) {
      builder.bearingTintColor(typedArray.getColor(
        R.styleable.mapbox_LocationComponent_mapbox_bearingTintColor, -1));
    }
    if (typedArray.hasValue(R.styleable.mapbox_LocationComponent_mapbox_enableStaleState)) {
      builder.enableStaleState(typedArray.getBoolean(
        R.styleable.mapbox_LocationComponent_mapbox_enableStaleState, true));
    }
    if (typedArray.hasValue(R.styleable.mapbox_LocationComponent_mapbox_staleStateTimeout)) {
      builder.staleStateTimeout(typedArray.getInteger(
        R.styleable.mapbox_LocationComponent_mapbox_staleStateTimeout, (int) STALE_STATE_DELAY_MS));
    }
    builder.gpsDrawable(typedArray.getResourceId(
      R.styleable.mapbox_LocationComponent_mapbox_gpsDrawable, -1));
    float elevation = typedArray.getDimension(
      R.styleable.mapbox_LocationComponent_mapbox_elevation, 0);
    builder.accuracyColor(typedArray.getColor(
      R.styleable.mapbox_LocationComponent_mapbox_accuracyColor, -1));
    builder.accuracyAlpha(typedArray.getFloat(
      R.styleable.mapbox_LocationComponent_mapbox_accuracyAlpha, ACCURACY_ALPHA_DEFAULT));
    builder.elevation(elevation);

    builder.trackingGesturesManagement(typedArray.getBoolean(
      R.styleable.mapbox_LocationComponent_mapbox_trackingGesturesManagement, false));
    builder.trackingInitialMoveThreshold(typedArray.getDimension(
      R.styleable.mapbox_LocationComponent_mapbox_trackingInitialMoveThreshold,
      context.getResources().getDimension(R.dimen.mapbox_locationComponentTrackingInitialMoveThreshold)));
    builder.trackingMultiFingerMoveThreshold(typedArray.getDimension(
      R.styleable.mapbox_LocationComponent_mapbox_trackingMultiFingerMoveThreshold,
      context.getResources().getDimension(R.dimen.mapbox_locationComponentTrackingMultiFingerMoveThreshold)));

    builder.padding(new int[] {
      typedArray.getInt(R.styleable.mapbox_LocationComponent_mapbox_iconPaddingLeft, 0),
      typedArray.getInt(R.styleable.mapbox_LocationComponent_mapbox_iconPaddingTop, 0),
      typedArray.getInt(R.styleable.mapbox_LocationComponent_mapbox_iconPaddingRight, 0),
      typedArray.getInt(R.styleable.mapbox_LocationComponent_mapbox_iconPaddingBottom, 0),
    });

    builder.layerBelow(
      typedArray.getString(R.styleable.mapbox_LocationComponent_mapbox_layer_below));

    float minScale = typedArray.getFloat(
      R.styleable.mapbox_LocationComponent_mapbox_minZoomIconScale, MIN_ZOOM_ICON_SCALE_DEFAULT);
    float maxScale = typedArray.getFloat(
      R.styleable.mapbox_LocationComponent_mapbox_maxZoomIconScale, MAX_ZOOM_ICON_SCALE_DEFAULT);
    builder.minZoomIconScale(minScale);
    builder.maxZoomIconScale(maxScale);

    float trackingAnimationDurationMultiplier = typedArray.getFloat(
      R.styleable.mapbox_LocationComponent_mapbox_trackingAnimationDurationMultiplier,
      TRACKING_ANIMATION_DURATION_MULTIPLIER_DEFAULT
    );
    builder.trackingAnimationDurationMultiplier(trackingAnimationDurationMultiplier);

    builder.compassAnimationEnabled = typedArray.getBoolean(
      R.styleable.mapbox_LocationComponent_mapbox_compassAnimationEnabled, true
    );

    builder.accuracyAnimationEnabled = typedArray.getBoolean(
      R.styleable.mapbox_LocationComponent_mapbox_accuracyAnimationEnabled, true
    );

    builder.pulsingCircleEnabled = typedArray.getBoolean(
      R.styleable.mapbox_LocationComponent_mapbox_pulsingLocationCircleEnabled, false
    );

    builder.pulsingCircleFadeEnabled = typedArray.getBoolean(
        R.styleable.mapbox_LocationComponent_mapbox_pulsingLocationCircleFadeEnabled, true
    );

    if (typedArray.hasValue(R.styleable.mapbox_LocationComponent_mapbox_pulsingLocationCircleColor)) {
      builder.pulsingCircleColor(typedArray.getColor(
        R.styleable.mapbox_LocationComponent_mapbox_pulsingLocationCircleColor,
        -1));
    }

    builder.pulsingCircleDuration = typedArray.getFloat(
      R.styleable.mapbox_LocationComponent_mapbox_pulsingLocationCircleDuration, CIRCLE_PULSING_DURATION_DEFAULT_MS
    );

    builder.pulsingCircleFrequency = typedArray.getFloat(
      R.styleable.mapbox_LocationComponent_mapbox_pulsingLocationCircleFrequency, CIRCLE_PULSING_FREQUENCY_DEFAULT_MS
    );

    builder.pulsingCircleAlpha = typedArray.getFloat(
      R.styleable.mapbox_LocationComponent_mapbox_pulsingLocationCircleAlpha, CIRCLE_PULSING_FINAL_ALPHA_DEFAULT
    );

    builder.pulsingCircleInterpolator = typedArray.getString(
      R.styleable.mapbox_LocationComponent_mapbox_pulsingLocationCircleInterpolator);

    typedArray.recycle();

    return builder.build();
  }

  /**
   * Takes the currently constructed {@link LocationComponentOptions} object and provides it's builder
   * with all the values set matching the values in this instance. This allows you to modify a
   * single attribute and then rebuild the object.
   *
   * @return the builder which contains the values defined in this current instance as defaults.
   */
  @NonNull
  public Builder toBuilder() {
    return new Builder(this);
  }

  /**
   * Build a new instance of the {@link LocationComponentOptions} class with all the attributes set
   * automatically to their defined defaults in this library. This allows you to adjust a few
   * attributes while leaving the rest alone and maintaining their default behavior.
   *
   * @param context your activities context used to acquire the style resource
   * @return the builder which contains the default values defined by the style resource
   */
  @NonNull
  public static Builder builder(@NonNull Context context) {
    return LocationComponentOptions.createFromAttributes(context,
      R.style.mapbox_LocationComponent).toBuilder();
  }

  /**
   * Set the opacity of the accuracy view to a value from 0 to 1, where 0 means the accuracy view is
   * completely transparent and 1 means the view is completely opaque.
   *
   * @return the opacity of the accuracy view
   * @attr ref R.styleable#LocationComponent_accuracyAlpha
   */
  public float accuracyAlpha() {
    return accuracyAlpha;
  }

  /**
   * Solid color to use as the accuracy view color property.
   *
   * @return the color of the accuracy view
   * @attr ref R.styleable#LocationComponent_accuracyColor
   */
  @ColorInt
  public int accuracyColor() {
    return accuracyColor;
  }

  /**
   * Defines the drawable used for the stale background icon.
   *
   * @return the drawable resource ID
   * @attr ref R.styleable#LocationComponent_backgroundDrawableStale
   */
  @DrawableRes
  public int backgroundDrawableStale() {
    return backgroundDrawableStale;
  }

  /**
   * String image name, identical to one used in
   * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
   * component, will use this image in place of the provided or default mapbox_foregroundDrawableStale.
   * <p>
   * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
   * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
   * </p>
   *
   * @return String icon or maki-icon name
   */
  @Nullable
  public String backgroundStaleName() {
    return backgroundStaleName;
  }

  /**
   * Defines the drawable used for the stale foreground icon.
   *
   * @return the drawable resource ID
   * @attr ref R.styleable#LocationComponent_foregroundDrawableStale
   */
  @DrawableRes
  public int foregroundDrawableStale() {
    return foregroundDrawableStale;
  }

  /**
   * String image name, identical to one used in
   * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
   * component, will used this image in place of the provided or default mapbox_foregroundDrawableStale.
   * <p>
   * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
   * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
   * </p>
   *
   * @return String icon or maki-icon name
   */
  @Nullable
  public String foregroundStaleName() {
    return foregroundStaleName;
  }

  /**
   * Defines the drawable used for the navigation state icon.
   *
   * @return the drawable resource ID
   * @attr ref R.styleable#LocationComponent_gpsDrawable
   */
  @DrawableRes
  public int gpsDrawable() {
    return gpsDrawable;
  }

  /**
   * String image name, identical to one used in
   * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
   * component, will used this image in place of the provided or default mapbox_gpsDrawable.
   * <p>
   * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
   * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
   * </p>
   *
   * @return String icon or maki-icon name
   */
  @Nullable
  public String gpsName() {
    return gpsName;
  }

  /**
   * Supply a Drawable that is to be rendered on top of all of the content in the Location LayerComponent layer stack.
   *
   * @return the drawable resource used for the foreground layer
   * @attr ref R.styleable#LocationComponent_foregroundDrawable
   */
  @DrawableRes
  public int foregroundDrawable() {
    return foregroundDrawable;
  }

  /**
   * String image name, identical to one used in
   * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
   * component, will used this image in place of the provided or default mapbox_foregroundDrawable.
   * <p>
   * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
   * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
   * </p>
   *
   * @return String icon or maki-icon name
   */
  @Nullable
  public String foregroundName() {
    return foregroundName;
  }

  /**
   * Defines the drawable used for the background state icon.
   *
   * @return the drawable resource ID
   * @attr ref R.styleable#LocationComponent_backgroundDrawable
   */
  @DrawableRes
  public int backgroundDrawable() {
    return backgroundDrawable;
  }

  /**
   * String image name, identical to one used in
   * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
   * component, will used this image in place of the provided or default mapbox_backgroundDrawable.
   * <p>
   * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
   * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
   * </p>
   *
   * @return String icon or maki-icon name
   */
  @Nullable
  public String backgroundName() {
    return backgroundName;
  }

  /**
   * Defines the drawable used for the bearing icon.
   *
   * @return the drawable resource ID
   * @attr ref R.styleable#LocationComponent_bearingDrawable
   */
  @DrawableRes
  public int bearingDrawable() {
    return bearingDrawable;
  }

  /**
   * String image name, identical to one used in
   * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
   * component, will used this image in place of the provided or default mapbox_bearingDrawable.
   * <p>
   * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
   * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
   * </p>
   *
   * @return String icon or maki-icon name
   */
  @Nullable
  public String bearingName() {
    return bearingName;
  }

  /**
   * Defines the bearing icon color as an integer.
   *
   * @return the color integer resource
   * @attr ref R.styleable#LocationComponent_bearingTintColor
   */
  @ColorInt
  @Nullable
  public Integer bearingTintColor() {
    return bearingTintColor;
  }

  /**
   * Defines the foreground color as an integer.
   *
   * @return the color integer resource
   * @attr ref R.styleable#LocationComponent_foregroundTintColor
   */
  @ColorInt
  @Nullable
  public Integer foregroundTintColor() {
    return foregroundTintColor;
  }

  /**
   * Defines the background color as an integer.
   *
   * @return the color integer resource
   * @attr ref R.styleable#LocationComponent_backgroundTintColor
   */
  @ColorInt
  @Nullable
  public Integer backgroundTintColor() {
    return backgroundTintColor;
  }

  /**
   * Defines the foreground stale color as an integer.
   *
   * @return the color integer resource
   * @attr ref R.styleable#LocationComponent_foregroundStaleTintColor
   */
  @ColorInt
  @Nullable
  public Integer foregroundStaleTintColor() {
    return foregroundStaleTintColor;
  }

  /**
   * Defines the background stale color as an integer.
   *
   * @return the color integer resource
   * @attr ref R.styleable#LocationComponent_backgroundStaleTintColor
   */
  @ColorInt
  @Nullable
  public Integer backgroundStaleTintColor() {
    return backgroundStaleTintColor;
  }

  /**
   * Sets the base elevation of this view, in pixels.
   *
   * @return the elevation currently set for the location component icon
   * @attr ref R.styleable#LocationComponent_elevation
   */
  @Dimension
  public float elevation() {
    return elevation;
  }

  /**
   * Enable or disable to stale state mode. This mode indicates to the user that the location being
   * displayed on the map hasn't been updated in a specific amount of time.
   *
   * @return whether the stale state mode is enabled or not
   * @attr ref R.styleable#LocationComponent_enableStaleState
   */
  public boolean enableStaleState() {
    return enableStaleState;
  }

  /**
   * Set the delay before the location icon becomes stale. The timer begins approximately when a new
   * location update comes in and using this defined time, if an update hasn't occured by the end,
   * the location is considered stale.
   *
   * @return the duration in milliseconds which it should take before the location is
   * considered stale
   * @attr ref R.styleable#LocationComponent_staleStateDelay
   */
  public long staleStateTimeout() {
    return staleStateTimeout;
  }

  /**
   * Sets the distance from the edges of the map view’s frame to the edges of the map
   * view’s logical viewport.
   * </p>
   * <p>
   * When the value of this property is equal to {0,0,0,0}, viewport
   * properties such as `centerCoordinate` assume a viewport that matches the map
   * view’s frame. Otherwise, those properties are inset, excluding part of the
   * frame from the viewport. For instance, if the only the top edge is inset, the
   * map center is effectively shifted downward.
   * </p>
   *
   * @return integer array of padding values
   */
  @Nullable
  @SuppressWarnings("mutable")
  public int[] padding() {
    return padding;
  }

  /**
   * The scale factor of the location icon when the map is zoomed in.
   * Scaling is linear.
   *
   * @return icon scale factor
   */
  public float maxZoomIconScale() {
    return maxZoomIconScale;
  }

  /**
   * The scale factor of the location icon when the map is zoomed out.
   * Scaling is linear.
   *
   * @return icon scale factor
   */
  public float minZoomIconScale() {
    return minZoomIconScale;
  }

  /**
   * Returns whether gesture threshold should be adjusted when camera is in one of the tracking modes.
   * This will adjust the focal point and increase thresholds to enable camera manipulation,
   * like zooming in and out, without breaking tracking.
   * <p>
   * <strong>Note</strong>: If set to true, this can overwrite some of the gesture thresholds
   * and the custom {@link com.mapbox.android.gestures.AndroidGesturesManager} that was set with
   * {@link com.mapbox.mapboxsdk.maps.MapboxMap#setGesturesManager(AndroidGesturesManager, boolean, boolean)}.
   *
   * @return true if gestures are adjusted when in one of the camera tracking modes, false otherwise
   * @see Builder#trackingInitialMoveThreshold(float)
   * @see Builder#trackingMultiFingerMoveThreshold(float)
   */
  public boolean trackingGesturesManagement() {
    return trackingGesturesManagement;
  }

  /**
   * Minimum single pointer movement in pixels required to break camera tracking.
   *
   * @return the minimum movement
   */
  public float trackingInitialMoveThreshold() {
    return trackingInitialMoveThreshold;
  }

  /**
   * Minimum multi pointer movement in pixels required to break camera tracking (for example during scale gesture).
   *
   * @return the minimum movement
   */
  public float trackingMultiFingerMoveThreshold() {
    return trackingMultiFingerMoveThreshold;
  }

  /**
   * Gets the id of the layer to add the location component above to.
   *
   * @return layerBelow the id of the layer to add the location component above to
   */
  public String layerBelow() {
    return layerBelow;
  }

  /**
   * Get the tracking animation duration multiplier.
   *
   * @return tracking animation duration multiplier
   */
  public float trackingAnimationDurationMultiplier() {
    return trackingAnimationDurationMultiplier;
  }

  /**
   * Enable or disable smooth animation of compass values for {@link com.mapbox.mapboxsdk.location.modes.CameraMode}
   * and {@link com.mapbox.mapboxsdk.location.modes.RenderMode}.
   *
   * @return whether smooth compass animation is enabled
   */
  public boolean compassAnimationEnabled() {
    return compassAnimationEnabled;
  }

  /**
   * Enable or disable smooth animation of the accuracy circle around the user's position.
   *
   * @return whether smooth animation of the accuracy circle is enabled
   */
  public boolean accuracyAnimationEnabled() {
    return accuracyAnimationEnabled;
  }

  /**
   * Enable or disable the LocationComponent's pulsing circle.
   *
   * @return whether the LocationComponent's pulsing circle is enabled
   */
  public Boolean pulseEnabled() {
    return pulsingCircleEnabled;
  }


  /**
   * Enable or disable fading of the LocationComponent's pulsing circle. If it fades, the circle's
   * opacity decreases as its radius increases.
   *
   * @return whether fading of the LocationComponent's pulsing circle is enabled
   */
  public Boolean pulsingCircleFadeEnabled() {
    return pulsingCircleFadeEnabled;
  }

  /**
   * Color of the LocationComponent's pulsing circle as it pulses.
   *
   * @return the current set color of the circle
   */
  public Integer pulseColor() {
    return pulseColor;
  }

  /**
   * The number of milliseconds it takes for a single pulse of the LocationComponent's pulsing circle.
   *
   * @return the current set length of time for a single pulse
   */
  public float pulseSingleDuration() {
    return pulseSingleDuration;
  }

  /**
   * The number of milliseconds between each pulse of the LocationComponent's circle
   *
   * @return the current set length of time between pulses
   */
  public float pulseFrequency() {
    return pulseFrequency;
  }

  /**
   * The opacity of the LocationComponent's circle as it pulses.
   *
   * @return the current set opacity of the LocationComponent's circle
   */
  public Float pulseAlpha() {
    return pulseAlpha;
  }

  /**
   * The interpolator type of animation for the movement of the LocationComponent's circle
   *
   * @return the current set type of animation interpolator for the pulsing circle
   */
  public String pulseInterpolator() {
    return pulseInterpolator;
  }

  @NonNull
  @Override
  public String toString() {
    return "LocationComponentOptions{"
      + "accuracyAlpha=" + accuracyAlpha + ", "
      + "accuracyColor=" + accuracyColor + ", "
      + "backgroundDrawableStale=" + backgroundDrawableStale + ", "
      + "backgroundStaleName=" + backgroundStaleName + ", "
      + "foregroundDrawableStale=" + foregroundDrawableStale + ", "
      + "foregroundStaleName=" + foregroundStaleName + ", "
      + "gpsDrawable=" + gpsDrawable + ", "
      + "gpsName=" + gpsName + ", "
      + "foregroundDrawable=" + foregroundDrawable + ", "
      + "foregroundName=" + foregroundName + ", "
      + "backgroundDrawable=" + backgroundDrawable + ", "
      + "backgroundName=" + backgroundName + ", "
      + "bearingDrawable=" + bearingDrawable + ", "
      + "bearingName=" + bearingName + ", "
      + "bearingTintColor=" + bearingTintColor + ", "
      + "foregroundTintColor=" + foregroundTintColor + ", "
      + "backgroundTintColor=" + backgroundTintColor + ", "
      + "foregroundStaleTintColor=" + foregroundStaleTintColor + ", "
      + "backgroundStaleTintColor=" + backgroundStaleTintColor + ", "
      + "elevation=" + elevation + ", "
      + "enableStaleState=" + enableStaleState + ", "
      + "staleStateTimeout=" + staleStateTimeout + ", "
      + "padding=" + Arrays.toString(padding) + ", "
      + "maxZoomIconScale=" + maxZoomIconScale + ", "
      + "minZoomIconScale=" + minZoomIconScale + ", "
      + "trackingGesturesManagement=" + trackingGesturesManagement + ", "
      + "trackingInitialMoveThreshold=" + trackingInitialMoveThreshold + ", "
      + "trackingMultiFingerMoveThreshold=" + trackingMultiFingerMoveThreshold + ", "
      + "layerBelow=" + layerBelow
      + "enablePulsingCircle=" + pulsingCircleEnabled
      + "enablePulsingCircleFade=" + pulsingCircleFadeEnabled
      + "pulseColor=" + pulseColor
      + "pulseSingleDuration=" + pulseSingleDuration
      + "pulseFrequency=" + pulseFrequency
      + "pulseAlpha=" + pulseAlpha
      + "pulseInterpolator=" + pulseInterpolator
      + "}";
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }
    if (o instanceof LocationComponentOptions) {
      LocationComponentOptions that = (LocationComponentOptions) o;
      return (Float.floatToIntBits(this.accuracyAlpha) == Float.floatToIntBits(that.accuracyAlpha()))
        && (this.accuracyColor == that.accuracyColor())
        && (this.backgroundDrawableStale == that.backgroundDrawableStale())
        && ((this.backgroundStaleName == null) ? (that.backgroundStaleName() == null)
        : this.backgroundStaleName.equals(that.backgroundStaleName()))
        && (this.foregroundDrawableStale == that.foregroundDrawableStale())
        && ((this.foregroundStaleName == null) ? (that.foregroundStaleName() == null)
        : this.foregroundStaleName.equals(that.foregroundStaleName()))
        && (this.gpsDrawable == that.gpsDrawable())
        && ((this.gpsName == null) ? (that.gpsName() == null) : this.gpsName.equals(that.gpsName()))
        && (this.foregroundDrawable == that.foregroundDrawable())
        && ((this.foregroundName == null) ? (that.foregroundName() == null)
        : this.foregroundName.equals(that.foregroundName()))
        && (this.backgroundDrawable == that.backgroundDrawable())
        && ((this.backgroundName == null) ? (that.backgroundName() == null)
        : this.backgroundName.equals(that.backgroundName()))
        && (this.bearingDrawable == that.bearingDrawable())
        && ((this.bearingName == null) ? (that.bearingName() == null)
        : this.bearingName.equals(that.bearingName()))
        && ((this.bearingTintColor == null) ? (that.bearingTintColor() == null)
        : this.bearingTintColor.equals(that.bearingTintColor()))
        && ((this.foregroundTintColor == null) ? (that.foregroundTintColor() == null)
        : this.foregroundTintColor.equals(that.foregroundTintColor()))
        && ((this.backgroundTintColor == null) ? (that.backgroundTintColor() == null)
        : this.backgroundTintColor.equals(that.backgroundTintColor()))
        && ((this.foregroundStaleTintColor == null) ? (that.foregroundStaleTintColor() == null)
        : this.foregroundStaleTintColor.equals(that.foregroundStaleTintColor()))
        && ((this.backgroundStaleTintColor == null) ? (that.backgroundStaleTintColor() == null)
        : this.backgroundStaleTintColor.equals(that.backgroundStaleTintColor()))
        && (Float.floatToIntBits(this.elevation) == Float.floatToIntBits(that.elevation()))
        && (this.enableStaleState == that.enableStaleState())
        && (this.staleStateTimeout == that.staleStateTimeout())
        && (Arrays.equals(this.padding, that.padding())
        && (Float.floatToIntBits(this.maxZoomIconScale) == Float.floatToIntBits(that.maxZoomIconScale()))
        && (Float.floatToIntBits(this.minZoomIconScale) == Float.floatToIntBits(that.minZoomIconScale()))
        && (this.trackingGesturesManagement == that.trackingGesturesManagement())
        && (Float.floatToIntBits(this.trackingInitialMoveThreshold)
        == Float.floatToIntBits(that.trackingInitialMoveThreshold()))
        && (Float.floatToIntBits(this.trackingMultiFingerMoveThreshold)
        == Float.floatToIntBits(that.trackingMultiFingerMoveThreshold()))
        && layerBelow.equals(that.layerBelow))
        && (Float.floatToIntBits(this.trackingAnimationDurationMultiplier)
        == Float.floatToIntBits(that.trackingAnimationDurationMultiplier()))
        && (this.pulsingCircleEnabled == that.pulseEnabled())
        && (this.pulsingCircleFadeEnabled == that.pulseEnabled())
        && (this.pulseColor == that.pulseColor())
        && (Float.floatToIntBits(this.pulseSingleDuration)
        == Float.floatToIntBits(that.pulseSingleDuration()))
        && (Float.floatToIntBits(this.pulseFrequency)
        == Float.floatToIntBits(that.pulseFrequency()))
        && (Float.floatToIntBits(this.pulseAlpha)
        == Float.floatToIntBits(that.pulseAlpha()))
        && ((this.pulseInterpolator == null) ? (that.pulseInterpolator() == null)
        : this.pulseInterpolator.equals(that.pulseInterpolator()));
    }
    return false;
  }

  @Override
  public int hashCode() {
    int h$ = 1;
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(accuracyAlpha);
    h$ *= 1000003;
    h$ ^= accuracyColor;
    h$ *= 1000003;
    h$ ^= backgroundDrawableStale;
    h$ *= 1000003;
    h$ ^= (backgroundStaleName == null) ? 0 : backgroundStaleName.hashCode();
    h$ *= 1000003;
    h$ ^= foregroundDrawableStale;
    h$ *= 1000003;
    h$ ^= (foregroundStaleName == null) ? 0 : foregroundStaleName.hashCode();
    h$ *= 1000003;
    h$ ^= gpsDrawable;
    h$ *= 1000003;
    h$ ^= (gpsName == null) ? 0 : gpsName.hashCode();
    h$ *= 1000003;
    h$ ^= foregroundDrawable;
    h$ *= 1000003;
    h$ ^= (foregroundName == null) ? 0 : foregroundName.hashCode();
    h$ *= 1000003;
    h$ ^= backgroundDrawable;
    h$ *= 1000003;
    h$ ^= (backgroundName == null) ? 0 : backgroundName.hashCode();
    h$ *= 1000003;
    h$ ^= bearingDrawable;
    h$ *= 1000003;
    h$ ^= (bearingName == null) ? 0 : bearingName.hashCode();
    h$ *= 1000003;
    h$ ^= (bearingTintColor == null) ? 0 : bearingTintColor.hashCode();
    h$ *= 1000003;
    h$ ^= (foregroundTintColor == null) ? 0 : foregroundTintColor.hashCode();
    h$ *= 1000003;
    h$ ^= (backgroundTintColor == null) ? 0 : backgroundTintColor.hashCode();
    h$ *= 1000003;
    h$ ^= (foregroundStaleTintColor == null) ? 0 : foregroundStaleTintColor.hashCode();
    h$ *= 1000003;
    h$ ^= (backgroundStaleTintColor == null) ? 0 : backgroundStaleTintColor.hashCode();
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(elevation);
    h$ *= 1000003;
    h$ ^= enableStaleState ? 1231 : 1237;
    h$ *= 1000003;
    h$ ^= (int) ((staleStateTimeout >>> 32) ^ staleStateTimeout);
    h$ *= 1000003;
    h$ ^= Arrays.hashCode(padding);
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(maxZoomIconScale);
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(minZoomIconScale);
    h$ *= 1000003;
    h$ ^= trackingGesturesManagement ? 1231 : 1237;
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(trackingInitialMoveThreshold);
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(trackingMultiFingerMoveThreshold);
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(trackingAnimationDurationMultiplier);
    h$ *= 1000003;
    h$ ^= compassAnimationEnabled ? 1231 : 1237;
    h$ *= 1000003;
    h$ ^= accuracyAnimationEnabled ? 1231 : 1237;
    h$ ^= 1000003;
    h$ ^= pulsingCircleEnabled ? 1231 : 1237;
    h$ ^= 1000003;
    h$ ^= pulsingCircleFadeEnabled ? 1231 : 1237;
    h$ ^= 1000003;
    h$ ^= pulseColor;
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(pulseSingleDuration);
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(pulseFrequency);
    h$ *= 1000003;
    h$ ^= Float.floatToIntBits(pulseAlpha);
    h$ *= 1000003;
    h$ ^= (pulseInterpolator == null) ? 0 : pulseInterpolator.hashCode();
    h$ *= 1000003;
    return h$;
  }

  public static final Parcelable.Creator<LocationComponentOptions> CREATOR =
    new Parcelable.Creator<LocationComponentOptions>() {
      @Override
      public LocationComponentOptions createFromParcel(Parcel in) {
        return new LocationComponentOptions(
          in.readFloat(),
          in.readInt(),
          in.readInt(),
          in.readInt() == 0 ? in.readString() : null,
          in.readInt(),
          in.readInt() == 0 ? in.readString() : null,
          in.readInt(),
          in.readInt() == 0 ? in.readString() : null,
          in.readInt(),
          in.readInt() == 0 ? in.readString() : null,
          in.readInt(),
          in.readInt() == 0 ? in.readString() : null,
          in.readInt(),
          in.readInt() == 0 ? in.readString() : null,
          in.readInt() == 0 ? in.readInt() : null,
          in.readInt() == 0 ? in.readInt() : null,
          in.readInt() == 0 ? in.readInt() : null,
          in.readInt() == 0 ? in.readInt() : null,
          in.readInt() == 0 ? in.readInt() : null,
          in.readFloat(),
          in.readInt() == 1,
          in.readLong(),
          in.createIntArray(),
          in.readFloat(),
          in.readFloat(),
          in.readInt() == 1,
          in.readFloat(),
          in.readFloat(),
          in.readString(),
          in.readFloat(),
          in.readInt() == 1,
          in.readInt() == 1,
          in.readInt() == 1,
          in.readInt() == 1,
          in.readInt() == 0 ? in.readInt() : null,
          in.readFloat(),
          in.readFloat(),
          in.readFloat(),
          in.readString()
        );
      }

      @Override
      public LocationComponentOptions[] newArray(int size) {
        return new LocationComponentOptions[size];
      }
    };

  @Override
  public void writeToParcel(@NonNull Parcel dest, int flags) {
    dest.writeFloat(accuracyAlpha());
    dest.writeInt(accuracyColor());
    dest.writeInt(backgroundDrawableStale());
    if (backgroundStaleName() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeString(backgroundStaleName());
    }
    dest.writeInt(foregroundDrawableStale());
    if (foregroundStaleName() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeString(foregroundStaleName());
    }
    dest.writeInt(gpsDrawable());
    if (gpsName() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeString(gpsName());
    }
    dest.writeInt(foregroundDrawable());
    if (foregroundName() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeString(foregroundName());
    }
    dest.writeInt(backgroundDrawable());
    if (backgroundName() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeString(backgroundName());
    }
    dest.writeInt(bearingDrawable());
    if (bearingName() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeString(bearingName());
    }
    if (bearingTintColor() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeInt(bearingTintColor());
    }
    if (foregroundTintColor() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeInt(foregroundTintColor());
    }
    if (backgroundTintColor() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeInt(backgroundTintColor());
    }
    if (foregroundStaleTintColor() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeInt(foregroundStaleTintColor());
    }
    if (backgroundStaleTintColor() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeInt(backgroundStaleTintColor());
    }
    dest.writeFloat(elevation());
    dest.writeInt(enableStaleState() ? 1 : 0);
    dest.writeLong(staleStateTimeout());
    dest.writeIntArray(padding());
    dest.writeFloat(maxZoomIconScale());
    dest.writeFloat(minZoomIconScale());
    dest.writeInt(trackingGesturesManagement() ? 1 : 0);
    dest.writeFloat(trackingInitialMoveThreshold());
    dest.writeFloat(trackingMultiFingerMoveThreshold());
    dest.writeString(layerBelow());
    dest.writeFloat(trackingAnimationDurationMultiplier);
    dest.writeInt(compassAnimationEnabled() ? 1 : 0);
    dest.writeInt(accuracyAnimationEnabled() ? 1 : 0);
    dest.writeInt(pulseEnabled() ? 1 : 0);
    dest.writeInt(pulsingCircleFadeEnabled() ? 1 : 0);
    if (pulseColor() == null) {
      dest.writeInt(1);
    } else {
      dest.writeInt(0);
      dest.writeInt(pulseColor());
    }
    dest.writeFloat(pulseSingleDuration());
    dest.writeFloat(pulseFrequency());
    dest.writeFloat(pulseAlpha());
    dest.writeString(pulseInterpolator());
  }

  @Override
  public int describeContents() {
    return 0;
  }

  /**
   * Builder class for constructing a new instance of {@link LocationComponentOptions}.
   */
  public static class Builder {

    /**
     * Build a new instance of this {@link LocationComponentOptions} class.
     *
     * @return a new instance of {@link LocationComponentOptions}
     */
    @NonNull
    public LocationComponentOptions build() {
      LocationComponentOptions locationComponentOptions = autoBuild();
      if (locationComponentOptions.accuracyAlpha() < 0 || locationComponentOptions.accuracyAlpha() > 1) {
        throw new IllegalArgumentException(
          "Accuracy alpha value must be between 0.0 and 1.0.");
      }

      if (locationComponentOptions.elevation() < 0f) {
        throw new IllegalArgumentException("Invalid shadow size "
          + locationComponentOptions.elevation() + ". Must be >= 0");
      }

      // TODO: Finish this logic below

      if (locationComponentOptions.pulseEnabled() == null) {

        Log.d("LCOptions", "autoBuild: this.pulsingCircleFadeEnabled == null ");

        String pulsingSetupError = "";

        if (locationComponentOptions.pulsingCircleFadeEnabled() != null) {
          pulsingSetupError += " pulsingCircleFadeEnabled";
        }
        if (locationComponentOptions.pulseColor() != null) {
          pulsingSetupError += " pulsingCircleColor";
        }
        if ((Float) locationComponentOptions.pulseSingleDuration() == null) {

          pulsingSetupError += " pulsingCircleDuration";
        }
        if ((Float) locationComponentOptions.pulseFrequency() != null) {
          pulsingSetupError += " pulsingCircleFrequency";
        }
        if (locationComponentOptions.pulseAlpha() != null) {
          pulsingSetupError += " accuracyAlpha";
        }
        if (locationComponentOptions.pulseInterpolator() != null) {
          pulsingSetupError += " pulsingCircleInterpolator";
        }
        if (!pulsingSetupError.isEmpty()) {
          throw new IllegalStateException("You've set up the following pulsing circle options but have not enabled" +
            " the pulsing circle via the LocationComponentOptions builder:" + pulsingSetupError + ". Enable the pulsing" +
            " circle if you're going to set pulsing options.");
        }
      }
      if (locationComponentOptions.pulseFrequency() < locationComponentOptions.pulseSingleDuration()) {
        throw new IllegalArgumentException("Invalid relationship between the LocationComponent " +
          "pulsing circle frequency of " + locationComponentOptions.pulseFrequency() + " and duration of " +
          locationComponentOptions.pulseSingleDuration() + " . The frequency of the pulsing must be >= to the duration " +
          "of a single pulse.");
      }
      return locationComponentOptions;
    }

    private Float accuracyAlpha;
    private Integer accuracyColor;
    private Integer backgroundDrawableStale;
    @Nullable
    private String backgroundStaleName;
    private Integer foregroundDrawableStale;
    @Nullable
    private String foregroundStaleName;
    private Integer gpsDrawable;
    @Nullable
    private String gpsName;
    private Integer foregroundDrawable;
    @Nullable
    private String foregroundName;
    private Integer backgroundDrawable;
    @Nullable
    private String backgroundName;
    private Integer bearingDrawable;
    @Nullable
    private String bearingName;
    @Nullable
    private Integer bearingTintColor;
    @Nullable
    private Integer foregroundTintColor;
    @Nullable
    private Integer backgroundTintColor;
    @Nullable
    private Integer foregroundStaleTintColor;
    @Nullable
    private Integer backgroundStaleTintColor;
    private Float elevation;
    private Boolean enableStaleState;
    private Long staleStateTimeout;
    @Nullable
    private int[] padding;
    private Float maxZoomIconScale;
    private Float minZoomIconScale;
    private Boolean trackingGesturesManagement;
    private Float trackingInitialMoveThreshold;
    private Float trackingMultiFingerMoveThreshold;
    private String layerBelow;
    private Float trackingAnimationDurationMultiplier;
    private Boolean compassAnimationEnabled;
    private Boolean accuracyAnimationEnabled;
    private Boolean pulsingCircleEnabled;
    private Boolean pulsingCircleFadeEnabled;
    private Integer pulsingCircleColor;
    private Float pulsingCircleDuration;
    private Float pulsingCircleFrequency;
    private Float pulsingCircleAlpha;
    private String pulsingCircleInterpolator;

    Builder() {
    }

    private Builder(LocationComponentOptions source) {
      this.accuracyAlpha = source.accuracyAlpha();
      this.accuracyColor = source.accuracyColor();
      this.backgroundDrawableStale = source.backgroundDrawableStale();
      this.backgroundStaleName = source.backgroundStaleName();
      this.foregroundDrawableStale = source.foregroundDrawableStale();
      this.foregroundStaleName = source.foregroundStaleName();
      this.gpsDrawable = source.gpsDrawable();
      this.gpsName = source.gpsName();
      this.foregroundDrawable = source.foregroundDrawable();
      this.foregroundName = source.foregroundName();
      this.backgroundDrawable = source.backgroundDrawable();
      this.backgroundName = source.backgroundName();
      this.bearingDrawable = source.bearingDrawable();
      this.bearingName = source.bearingName();
      this.bearingTintColor = source.bearingTintColor();
      this.foregroundTintColor = source.foregroundTintColor();
      this.backgroundTintColor = source.backgroundTintColor();
      this.foregroundStaleTintColor = source.foregroundStaleTintColor();
      this.backgroundStaleTintColor = source.backgroundStaleTintColor();
      this.elevation = source.elevation();
      this.enableStaleState = source.enableStaleState();
      this.staleStateTimeout = source.staleStateTimeout();
      this.padding = source.padding();
      this.maxZoomIconScale = source.maxZoomIconScale();
      this.minZoomIconScale = source.minZoomIconScale();
      this.trackingGesturesManagement = source.trackingGesturesManagement();
      this.trackingInitialMoveThreshold = source.trackingInitialMoveThreshold();
      this.trackingMultiFingerMoveThreshold = source.trackingMultiFingerMoveThreshold();
      this.layerBelow = source.layerBelow();
      this.trackingAnimationDurationMultiplier = source.trackingAnimationDurationMultiplier();
      this.compassAnimationEnabled = source.compassAnimationEnabled();
      this.accuracyAnimationEnabled = source.accuracyAnimationEnabled();
      this.pulsingCircleEnabled = source.pulsingCircleEnabled;
      this.pulsingCircleFadeEnabled = source.pulsingCircleFadeEnabled;
      this.pulsingCircleColor = source.pulseColor;
      this.pulsingCircleDuration = source.pulseSingleDuration;
      this.pulsingCircleFrequency = source.pulseFrequency;
      this.pulsingCircleAlpha = source.pulseAlpha;
      this.pulsingCircleInterpolator = source.pulseInterpolator;
    }

    /**
     * Set the opacity of the accuracy view to a value from 0 to 1, where 0 means the accuracy view
     * is completely transparent and 1 means the view is completely opaque.
     *
     * @param accuracyAlpha the opacity of the accuracy view
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_accuracyAlpha
     */
    @NonNull
    public LocationComponentOptions.Builder accuracyAlpha(float accuracyAlpha) {
      this.accuracyAlpha = accuracyAlpha;
      return this;
    }

    /**
     * Solid color to use as the accuracy view color property.
     *
     * @param accuracyColor the color of the accuracy view
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_accuracyColor
     */
    @NonNull
    public LocationComponentOptions.Builder accuracyColor(int accuracyColor) {
      this.accuracyColor = accuracyColor;
      return this;
    }

    /**
     * Defines the drawable used for the stale background icon.
     *
     * @param backgroundDrawableStale the drawable resource ID
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_backgroundDrawableStale
     */
    @NonNull
    public LocationComponentOptions.Builder backgroundDrawableStale(int backgroundDrawableStale) {
      this.backgroundDrawableStale = backgroundDrawableStale;
      return this;
    }

    /**
     * Given a String image name, identical to one used in
     * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
     * component, will used this image in place of the provided or default mapbox_backgroundDrawableStale.
     * <p>
     * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
     * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
     * </p>
     *
     * @param backgroundStaleName String icon or maki-icon name
     * @return this builder for chaining options together
     */
    @NonNull
    public LocationComponentOptions.Builder backgroundStaleName(@Nullable String backgroundStaleName) {
      this.backgroundStaleName = backgroundStaleName;
      return this;
    }

    /**
     * Defines the drawable used for the stale foreground icon.
     *
     * @param foregroundDrawableStale the drawable resource ID
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_foregroundDrawableStale
     */
    @NonNull
    public LocationComponentOptions.Builder foregroundDrawableStale(int foregroundDrawableStale) {
      this.foregroundDrawableStale = foregroundDrawableStale;
      return this;
    }

    /**
     * Given a String image name, identical to one used in
     * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
     * component, will used this image in place of the provided or default mapbox_foregroundDrawableStale.
     * <p>
     * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
     * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
     * </p>
     *
     * @param foregroundStaleName String icon or maki-icon name
     * @return this builder for chaining options together
     */
    @NonNull
    public LocationComponentOptions.Builder foregroundStaleName(@Nullable String foregroundStaleName) {
      this.foregroundStaleName = foregroundStaleName;
      return this;
    }

    /**
     * Defines the drawable used for the navigation state icon.
     *
     * @param gpsDrawable the drawable resource ID
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_gpsDrawable
     */
    @NonNull
    public LocationComponentOptions.Builder gpsDrawable(int gpsDrawable) {
      this.gpsDrawable = gpsDrawable;
      return this;
    }

    /**
     * Given a String image name, identical to one used in
     * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
     * component, will used this image in place of the provided or default mapbox_gpsDrawable.
     * <p>
     * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
     * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
     * </p>
     *
     * @param gpsName String icon or maki-icon name
     * @return this builder for chaining options together
     */
    @NonNull
    public LocationComponentOptions.Builder gpsName(@Nullable String gpsName) {
      this.gpsName = gpsName;
      return this;
    }

    /**
     * Supply a Drawable that is to be rendered on top of all of the content in the Location Component layer stack.
     *
     * @param foregroundDrawable the drawable resource used for the foreground layer
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_foregroundDrawable
     */
    @NonNull
    public LocationComponentOptions.Builder foregroundDrawable(int foregroundDrawable) {
      this.foregroundDrawable = foregroundDrawable;
      return this;
    }

    /**
     * Given a String image name, identical to one used in
     * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
     * component, will used this image in place of the provided or default mapbox_foregroundDrawable.
     * <p>
     * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
     * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
     * </p>
     *
     * @param foregroundName String icon or maki-icon name
     * @return this builder for chaining options together
     */
    @NonNull
    public LocationComponentOptions.Builder foregroundName(@Nullable String foregroundName) {
      this.foregroundName = foregroundName;
      return this;
    }

    /**
     * Defines the drawable used for the background state icon.
     *
     * @param backgroundDrawable the drawable resource ID
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_backgroundDrawable
     */
    @NonNull
    public LocationComponentOptions.Builder backgroundDrawable(int backgroundDrawable) {
      this.backgroundDrawable = backgroundDrawable;
      return this;
    }

    /**
     * Given a String image name, identical to one used in
     * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
     * component, will used this image in place of the provided or default mapbox_backgroundDrawable.
     * <p>
     * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
     * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
     * </p>
     *
     * @param backgroundName String icon or maki-icon name
     * @return this builder for chaining options together
     */
    @NonNull
    public LocationComponentOptions.Builder backgroundName(@Nullable String backgroundName) {
      this.backgroundName = backgroundName;
      return this;
    }

    /**
     * Defines the drawable used for the bearing icon.
     *
     * @param bearingDrawable the drawable resource ID
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_bearingDrawable
     */
    @NonNull
    public LocationComponentOptions.Builder bearingDrawable(int bearingDrawable) {
      this.bearingDrawable = bearingDrawable;
      return this;
    }

    /**
     * Given a String image name, identical to one used in
     * the first parameter of {@link com.mapbox.mapboxsdk.maps.Style.Builder#addImage(String, Bitmap)}, the
     * component, will used this image in place of the provided or default mapbox_bearingDrawable.
     * <p>
     * A maki-icon name (example: "circle-15") may also be provided.  These are images that can be loaded
     * with certain styles.  Note, this will fail if the provided icon name is not provided by the loaded map style.
     * </p>
     *
     * @param bearingName String icon or maki-icon name
     * @return this builder for chaining options together
     */
    @NonNull
    public LocationComponentOptions.Builder bearingName(@Nullable String bearingName) {
      this.bearingName = bearingName;
      return this;
    }

    /**
     * Defines the bearing icon color as an integer.
     *
     * @param bearingTintColor the color integer resource
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_bearingTintColor
     */
    @NonNull
    public LocationComponentOptions.Builder bearingTintColor(@Nullable Integer bearingTintColor) {
      this.bearingTintColor = bearingTintColor;
      return this;
    }

    /**
     * Defines the foreground color as an integer.
     *
     * @param foregroundTintColor the color integer resource
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_foregroundTintColor
     */
    @NonNull
    public LocationComponentOptions.Builder foregroundTintColor(@Nullable Integer foregroundTintColor) {
      this.foregroundTintColor = foregroundTintColor;
      return this;
    }

    /**
     * Defines the background color as an integer.
     *
     * @param backgroundTintColor the color integer resource
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_backgroundTintColor
     */
    @NonNull
    public LocationComponentOptions.Builder backgroundTintColor(@Nullable Integer backgroundTintColor) {
      this.backgroundTintColor = backgroundTintColor;
      return this;
    }

    /**
     * Defines the foreground stale color as an integer.
     *
     * @param foregroundStaleTintColor the color integer resource
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_foregroundStaleTintColor
     */
    @NonNull
    public LocationComponentOptions.Builder foregroundStaleTintColor(@Nullable Integer foregroundStaleTintColor) {
      this.foregroundStaleTintColor = foregroundStaleTintColor;
      return this;
    }

    /**
     * Defines the background stale color as an integer.
     *
     * @param backgroundStaleTintColor the color integer resource
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_backgroundStaleTintColor
     */
    @NonNull
    public LocationComponentOptions.Builder backgroundStaleTintColor(@Nullable Integer backgroundStaleTintColor) {
      this.backgroundStaleTintColor = backgroundStaleTintColor;
      return this;
    }

    /**
     * Sets the base elevation of this view, in pixels.
     *
     * @param elevation the elevation currently set for the location icon
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_elevation
     */
    @NonNull
    public LocationComponentOptions.Builder elevation(float elevation) {
      this.elevation = elevation;
      return this;
    }

    /**
     * Enable or disable to stale state mode. This mode indicates to the user that the location
     * being displayed on the map hasn't been updated in a specific amount of time.
     *
     * @param enabled whether the stale state mode is enabled or not
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_enableStaleState
     */
    @NonNull
    public LocationComponentOptions.Builder enableStaleState(boolean enabled) {
      this.enableStaleState = enabled;
      return this;
    }

    /**
     * Set the timeout before the location icon becomes stale. The timer begins approximately when a
     * new location update comes in and using this defined time, if an update hasn't occurred by the
     * end, the location is considered stale.
     *
     * @param timeout the duration in milliseconds which it should take before the location is
     *                considered stale
     * @return this builder for chaining options together
     * @attr ref R.styleable#LocationComponent_staleStateTimeout
     */
    @NonNull
    public LocationComponentOptions.Builder staleStateTimeout(long timeout) {
      this.staleStateTimeout = timeout;
      return this;
    }

    /**
     * Sets the distance from the edges of the map view’s frame to the edges of the map
     * view’s logical viewport.
     * </p>
     * <p>
     * When the value of this property is equal to {0,0,0,0}, viewport
     * properties such as `centerCoordinate` assume a viewport that matches the map
     * view’s frame. Otherwise, those properties are inset, excluding part of the
     * frame from the viewport. For instance, if the only the top edge is inset, the
     * map center is effectively shifted downward.
     * </p>
     *
     * @param padding The margins for the map in pixels (left, top, right, bottom).
     */
    @NonNull
    public LocationComponentOptions.Builder padding(@Nullable int[] padding) {
      if (padding == null) {
        throw new NullPointerException("Null padding");
      }
      this.padding = padding;
      return this;
    }

    /**
     * Sets the scale factor of the location icon when the map is zoomed in.
     * Scaling is linear and the new pixel size of the image will be the original pixel size multiplied by the argument.
     * <p>
     * Set both this and {@link #minZoomIconScale(float)} to 1f to disable location icon scaling.
     * <p>
     * Scaling is based on the maps minimum and maximum zoom levels in time of component's style application.
     *
     * @param maxZoomIconScale icon scale factor
     */
    @NonNull
    public LocationComponentOptions.Builder maxZoomIconScale(float maxZoomIconScale) {
      this.maxZoomIconScale = maxZoomIconScale;
      return this;
    }

    /**
     * Sets the scale factor of the location icon when the map is zoomed out.
     * Scaling is linear and the new pixel size of the image will be the original pixel size multiplied by the argument.
     * <p>
     * Set both this and {@link #maxZoomIconScale(float)} to 1f to disable location icon scaling.
     * <p>
     * Scaling is based on the maps minimum and maximum zoom levels in time of component's style application.
     *
     * @param minZoomIconScale icon scale factor
     */
    @NonNull
    public LocationComponentOptions.Builder minZoomIconScale(float minZoomIconScale) {
      this.minZoomIconScale = minZoomIconScale;
      return this;
    }

    /**
     * Set whether gesture threshold should be adjusted when camera is in one of the tracking modes.
     * This will adjust the focal point and increase thresholds to enable camera manipulation,
     * like zooming in and out, without breaking tracking.
     * <p>
     * <strong>Note</strong>: This can overwrite some of the gesture thresholds
     * and the custom {@link com.mapbox.android.gestures.AndroidGesturesManager} that was set with
     * {@link com.mapbox.mapboxsdk.maps.MapboxMap#setGesturesManager(AndroidGesturesManager, boolean, boolean)}.
     *
     * @param trackingGesturesManagement true if gestures should be adjusted when in one of the camera tracking modes,
     *                                   false otherwise
     * @see Builder#trackingInitialMoveThreshold(float)
     * @see Builder#trackingMultiFingerMoveThreshold(float)
     */
    @NonNull
    public LocationComponentOptions.Builder trackingGesturesManagement(boolean trackingGesturesManagement) {
      this.trackingGesturesManagement = trackingGesturesManagement;
      return this;
    }

    /**
     * Sets minimum single pointer movement (map pan) in pixels required to break camera tracking.
     *
     * @param moveThreshold the minimum movement
     */
    @NonNull
    public LocationComponentOptions.Builder trackingInitialMoveThreshold(float moveThreshold) {
      this.trackingInitialMoveThreshold = moveThreshold;
      return this;
    }

    /**
     * Sets minimum multi pointer movement (map pan) in pixels required to break camera tracking
     * (for example during scale gesture).
     *
     * @param moveThreshold the minimum movement
     */
    @NonNull
    public LocationComponentOptions.Builder trackingMultiFingerMoveThreshold(float moveThreshold) {
      this.trackingMultiFingerMoveThreshold = moveThreshold;
      return this;
    }

    /**
     * Sets the layer id to set the location component below to.
     *
     * @param layerBelow the id to set the location component below to.
     */
    @NonNull
    public LocationComponentOptions.Builder layerBelow(String layerBelow) {
      this.layerBelow = layerBelow;
      return this;
    }

    /**
     * Sets the tracking animation duration multiplier.
     *
     * @param trackingAnimationDurationMultiplier the tracking animation duration multiplier
     */
    @NonNull
    public LocationComponentOptions.Builder trackingAnimationDurationMultiplier(
      float trackingAnimationDurationMultiplier) {
      this.trackingAnimationDurationMultiplier = trackingAnimationDurationMultiplier;
      return this;
    }

    /**
     * Enable or disable smooth animation of compass values for {@link com.mapbox.mapboxsdk.location.modes.CameraMode}
     * and {@link com.mapbox.mapboxsdk.location.modes.RenderMode}.
     *
     * @return whether smooth compass animation is enabled
     */
    public LocationComponentOptions.Builder compassAnimationEnabled(Boolean compassAnimationEnabled) {
      this.compassAnimationEnabled = compassAnimationEnabled;
      return this;
    }

    /**
     * Enable or disable smooth animation of the accuracy circle around the user's position.
     *
     * @return whether smooth animation of the accuracy circle is enabled
     */
    public LocationComponentOptions.Builder accuracyAnimationEnabled(Boolean accuracyAnimationEnabled) {
      this.accuracyAnimationEnabled = accuracyAnimationEnabled;
      return this;
    }

    /**
     * Enable or disable the LocationComponent's pulsing circle.
     *
     * @return whether the LocationComponent's pulsing circle is enabled
     */
    public LocationComponentOptions.Builder  pulsingCircleEnabled(Boolean pulsingCircleEnabled) {
      this.pulsingCircleEnabled = pulsingCircleEnabled;
      return this;
    }

    /**
     * Enable or disable fading of the LocationComponent's pulsing circle. If it fades, the circle's
     * opacity decreases as its radius increases.
     *
     * @return whether fading of the LocationComponent's pulsing circle is enabled
     */
    public LocationComponentOptions.Builder  pulsingCircleFadeEnabled(Boolean pulsingCircleFadeEnabled) {
      this.pulsingCircleFadeEnabled = pulsingCircleFadeEnabled;
      return this;
    }

    /**
     * Sets the color of the LocationComponent's pulsing circle.
     *
     * @return the current set color of the circle
     */
    public LocationComponentOptions.Builder pulsingCircleColor(Integer pulsingCircleColor) {
      this.pulsingCircleColor = pulsingCircleColor;
      return this;
    }

    /**
     * Sets the number of milliseconds it takes for a single pulse of the LocationComponent's pulsing circle.
     *
     * @return the current set length of time for a single pulse
     */
    public LocationComponentOptions.Builder pulsingCircleDuration(Float pulsingCircleDuration) {
      this.pulsingCircleDuration = pulsingCircleDuration;
      return this;
    }

    /**
     * Sets the number of milliseconds that are waiting between pulses of the LocationComponent's pulsing circle.
     *
     * @return the current set length of time between pulses
     */
    public LocationComponentOptions.Builder pulsingCircleFrequency(Float pulsingCircleFrequency) {
      this.pulsingCircleFrequency = pulsingCircleFrequency;
      return this;
    }

    /**
     * Sets the opacity of the LocationComponent's pulsing circle.
     *
     * @return the current set opacity of the LocationComponent's circle
     */
    public LocationComponentOptions.Builder pulsingCircleAlpha(Float pulsingCircleAlpha) {
      this.pulsingCircleAlpha = pulsingCircleAlpha;
      return this;
    }

    /**
     * Sets the pulsing circle's interpolator animation.
     *
     * @return the interpolator animation which the pulsing circle is using
     */
    public LocationComponentOptions.Builder pulsingCircleInterpolator(String pulsingCircleInterpolator) {
      this.pulsingCircleInterpolator = pulsingCircleInterpolator;
      return this;
    }

    @Nullable
    LocationComponentOptions autoBuild() {
      String missing = "";
      if (this.accuracyAlpha == null) {
        missing += " accuracyAlpha";
      }
      if (this.accuracyColor == null) {
        missing += " accuracyColor";
      }
      if (this.backgroundDrawableStale == null) {
        missing += " backgroundDrawableStale";
      }
      if (this.foregroundDrawableStale == null) {
        missing += " foregroundDrawableStale";
      }
      if (this.gpsDrawable == null) {
        missing += " gpsDrawable";
      }
      if (this.foregroundDrawable == null) {
        missing += " foregroundDrawable";
      }
      if (this.backgroundDrawable == null) {
        missing += " backgroundDrawable";
      }
      if (this.bearingDrawable == null) {
        missing += " bearingDrawable";
      }
      if (this.elevation == null) {
        missing += " elevation";
      }
      if (this.enableStaleState == null) {
        missing += " enableStaleState";
      }
      if (this.staleStateTimeout == null) {
        missing += " staleStateTimeout";
      }
      if (this.padding == null) {
        missing += " padding";
      }
      if (this.maxZoomIconScale == null) {
        missing += " maxZoomIconScale";
      }
      if (this.minZoomIconScale == null) {
        missing += " minZoomIconScale";
      }
      if (this.trackingGesturesManagement == null) {
        missing += " trackingGesturesManagement";
      }
      if (this.trackingInitialMoveThreshold == null) {
        missing += " trackingInitialMoveThreshold";
      }
      if (this.trackingMultiFingerMoveThreshold == null) {
        missing += " trackingMultiFingerMoveThreshold";
      }
      if (this.pulsingCircleInterpolator == null) {
        missing += " pulseInterpolator";
      }
      if (!missing.isEmpty()) {
        throw new IllegalStateException("Missing required properties:" + missing);
      }
      return new LocationComponentOptions(
        this.accuracyAlpha,
        this.accuracyColor,
        this.backgroundDrawableStale,
        this.backgroundStaleName,
        this.foregroundDrawableStale,
        this.foregroundStaleName,
        this.gpsDrawable,
        this.gpsName,
        this.foregroundDrawable,
        this.foregroundName,
        this.backgroundDrawable,
        this.backgroundName,
        this.bearingDrawable,
        this.bearingName,
        this.bearingTintColor,
        this.foregroundTintColor,
        this.backgroundTintColor,
        this.foregroundStaleTintColor,
        this.backgroundStaleTintColor,
        this.elevation,
        this.enableStaleState,
        this.staleStateTimeout,
        this.padding,
        this.maxZoomIconScale,
        this.minZoomIconScale,
        trackingGesturesManagement,
        this.trackingInitialMoveThreshold,
        this.trackingMultiFingerMoveThreshold,
        this.layerBelow,
        this.trackingAnimationDurationMultiplier,
        this.compassAnimationEnabled,
        this.accuracyAnimationEnabled,
        this.pulsingCircleEnabled,
        this.pulsingCircleFadeEnabled,
        this.pulsingCircleColor,
        this.pulsingCircleDuration,
        this.pulsingCircleFrequency,
        this.pulsingCircleAlpha,
        this.pulsingCircleInterpolator);
    }
  }
}
