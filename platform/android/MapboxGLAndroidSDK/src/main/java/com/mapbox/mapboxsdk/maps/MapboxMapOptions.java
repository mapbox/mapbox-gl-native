package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.ColorInt;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.Gravity;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.utils.ColorUtils;

import java.util.Arrays;

/**
 * Defines configuration MapboxMapMapOptions for a MapboxMap. These options can be used when adding a
 * map to your application programmatically (as opposed to via XML). If you are using a MapFragment,
 * you can pass these options in using the static factory method newInstance(MapboxMapOptions).
 * If you are using a MapView, you can pass these options in using the constructor
 * MapView(Context, MapboxMapOptions). If you add a map using XML, then you can apply these options
 * using custom XML tags.
 */
public class MapboxMapOptions implements Parcelable {

  private static final float DIMENSION_SEVEN_DP = 7f;
  private static final float DIMENSION_TEN_DP = 10f;
  private static final float DIMENSION_SIXTEEN_DP = 16f;
  private static final float DIMENSION_SEVENTY_SIX_DP = 76f;

  private CameraPosition cameraPosition;

  private boolean debugActive;

  private boolean compassEnabled = true;
  private boolean fadeCompassFacingNorth = true;
  private int compassGravity = Gravity.TOP | Gravity.END;
  private int[] compassMargins;

  private boolean logoEnabled = true;
  private int logoGravity = Gravity.BOTTOM | Gravity.START;
  private int[] logoMargins;

  @ColorInt
  private int attributionTintColor = -1;
  private boolean attributionEnabled = true;
  private int attributionGravity = Gravity.BOTTOM;
  private int[] attributionMargins;

  private double minZoom = MapboxConstants.MINIMUM_ZOOM;
  private double maxZoom = MapboxConstants.MAXIMUM_ZOOM;

  private boolean rotateGesturesEnabled = true;
  private boolean scrollGesturesEnabled = true;
  private boolean tiltGesturesEnabled = true;
  private boolean zoomGesturesEnabled = true;
  private boolean zoomControlsEnabled = false;
  private boolean doubleTapGesturesEnabled = true;

  private boolean myLocationEnabled;
  private Drawable myLocationForegroundDrawable;
  private Drawable myLocationForegroundBearingDrawable;
  private Drawable myLocationBackgroundDrawable;
  private int myLocationForegroundTintColor;
  private int myLocationBackgroundTintColor;
  private int[] myLocationBackgroundPadding;
  private int myLocationAccuracyTintColor;
  private int myLocationAccuracyAlpha;

  private String apiBaseUrl;

  private String style;

  /**
   * Creates a new MapboxMapOptions object.
   */
  public MapboxMapOptions() {
  }

  private MapboxMapOptions(Parcel in) {
    cameraPosition = in.readParcelable(CameraPosition.class.getClassLoader());
    debugActive = in.readByte() != 0;

    compassEnabled = in.readByte() != 0;
    compassGravity = in.readInt();
    compassMargins = in.createIntArray();
    fadeCompassFacingNorth = in.readByte() != 0;

    logoEnabled = in.readByte() != 0;
    logoGravity = in.readInt();
    logoMargins = in.createIntArray();

    attributionEnabled = in.readByte() != 0;
    attributionGravity = in.readInt();
    attributionMargins = in.createIntArray();
    attributionTintColor = in.readInt();

    minZoom = in.readDouble();
    maxZoom = in.readDouble();

    rotateGesturesEnabled = in.readByte() != 0;
    scrollGesturesEnabled = in.readByte() != 0;
    tiltGesturesEnabled = in.readByte() != 0;
    zoomControlsEnabled = in.readByte() != 0;
    zoomGesturesEnabled = in.readByte() != 0;
    doubleTapGesturesEnabled = in.readByte() != 0;

    myLocationEnabled = in.readByte() != 0;

    Bitmap foregroundBitmap = in.readParcelable(getClass().getClassLoader());
    if (foregroundBitmap != null) {
      myLocationForegroundDrawable = new BitmapDrawable(foregroundBitmap);
    }

    Bitmap foregroundBearingBitmap = in.readParcelable(getClass().getClassLoader());
    if (foregroundBearingBitmap != null) {
      myLocationForegroundBearingDrawable = new BitmapDrawable(foregroundBearingBitmap);
    }

    Bitmap backgroundBitmap = in.readParcelable(getClass().getClassLoader());
    if (backgroundBitmap != null) {
      myLocationBackgroundDrawable = new BitmapDrawable(backgroundBitmap);
    }

    myLocationForegroundTintColor = in.readInt();
    myLocationBackgroundTintColor = in.readInt();
    myLocationBackgroundPadding = in.createIntArray();
    myLocationAccuracyAlpha = in.readInt();
    myLocationAccuracyTintColor = in.readInt();

    style = in.readString();
    apiBaseUrl = in.readString();
  }

  public static Bitmap getBitmapFromDrawable(Drawable drawable) {
    if (drawable instanceof BitmapDrawable) {
      return ((BitmapDrawable) drawable).getBitmap();
    } else {
      Bitmap bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(),
        Bitmap.Config.ARGB_8888);
      Canvas canvas = new Canvas(bitmap);
      drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
      drawable.draw(canvas);
      return bitmap;
    }
  }

  /**
   * Creates a MapboxMapsOptions from the attribute set.s
   *
   * @param context Context related to a map view.
   * @param attrs   Attributeset containing configuration
   * @return the MapboxMapOptions created from attributes
   */
  public static MapboxMapOptions createFromAttributes(@NonNull Context context, @Nullable AttributeSet attrs) {
    MapboxMapOptions mapboxMapOptions = new MapboxMapOptions();
    float pxlRatio = context.getResources().getDisplayMetrics().density;
    TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.mapbox_MapView, 0, 0);
    try {
      mapboxMapOptions.camera(new CameraPosition.Builder(typedArray).build());
      mapboxMapOptions.styleUrl(typedArray.getString(R.styleable.mapbox_MapView_mapbox_styleUrl));
      mapboxMapOptions.apiBaseUrl(typedArray.getString(R.styleable.mapbox_MapView_mapbox_apiBaseUrl));

      mapboxMapOptions.zoomGesturesEnabled(
        typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_uiZoomGestures, true));
      mapboxMapOptions.scrollGesturesEnabled(
        typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_uiScrollGestures, true));
      mapboxMapOptions.rotateGesturesEnabled(
        typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_uiRotateGestures, true));
      mapboxMapOptions.tiltGesturesEnabled(
        typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_uiTiltGestures, true));
      mapboxMapOptions.zoomControlsEnabled(
        typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_uiZoomControls, false));
      mapboxMapOptions.doubleTapGesturesEnabled(
        typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_uiDoubleTapGestures, true));

      mapboxMapOptions.maxZoomPreference(typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraZoomMax,
        MapboxConstants.MAXIMUM_ZOOM));
      mapboxMapOptions.minZoomPreference(typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraZoomMin,
        MapboxConstants.MINIMUM_ZOOM));

      mapboxMapOptions.compassEnabled(typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_uiCompass, true));
      mapboxMapOptions.compassGravity(typedArray.getInt(R.styleable.mapbox_MapView_mapbox_uiCompassGravity,
        Gravity.TOP | Gravity.END));
      mapboxMapOptions.compassMargins(new int[] {
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiCompassMarginLeft,
          DIMENSION_TEN_DP * pxlRatio)),
        ((int) typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiCompassMarginTop,
          DIMENSION_TEN_DP * pxlRatio)),
        ((int) typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiCompassMarginRight,
          DIMENSION_TEN_DP * pxlRatio)),
        ((int) typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiCompassMarginBottom,
          DIMENSION_TEN_DP * pxlRatio))});
      mapboxMapOptions.compassFadesWhenFacingNorth(typedArray.getBoolean(
        R.styleable.mapbox_MapView_mapbox_uiCompassFadeFacingNorth, true));

      mapboxMapOptions.logoEnabled(typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_uiLogo, true));
      mapboxMapOptions.logoGravity(typedArray.getInt(R.styleable.mapbox_MapView_mapbox_uiLogoGravity,
        Gravity.BOTTOM | Gravity.START));
      mapboxMapOptions.logoMargins(new int[] {
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiLogoMarginLeft,
          DIMENSION_SIXTEEN_DP * pxlRatio)),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiLogoMarginTop,
          DIMENSION_SIXTEEN_DP * pxlRatio)),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiLogoMarginRight,
          DIMENSION_SIXTEEN_DP * pxlRatio)),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiLogoMarginBottom,
          DIMENSION_SIXTEEN_DP * pxlRatio))});

      mapboxMapOptions.attributionTintColor(typedArray.getColor(
        R.styleable.mapbox_MapView_mapbox_uiAttributionTintColor, -1));
      mapboxMapOptions.attributionEnabled(typedArray.getBoolean(
        R.styleable.mapbox_MapView_mapbox_uiAttribution, true));
      mapboxMapOptions.attributionGravity(typedArray.getInt(
        R.styleable.mapbox_MapView_mapbox_uiAttributionGravity, Gravity.BOTTOM));
      mapboxMapOptions.attributionMargins(new int[] {
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiAttributionMarginLeft,
          DIMENSION_SEVENTY_SIX_DP) * pxlRatio),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiAttributionMarginTop,
          DIMENSION_SEVEN_DP * pxlRatio)),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiAttributionMarginRight,
          DIMENSION_SEVEN_DP * pxlRatio)),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_uiAttributionMarginBottom,
          DIMENSION_SEVEN_DP * pxlRatio))});

      mapboxMapOptions.locationEnabled(typedArray.getBoolean(R.styleable.mapbox_MapView_mapbox_myLocation, false));
      mapboxMapOptions.myLocationForegroundTintColor(
        typedArray.getColor(R.styleable.mapbox_MapView_mapbox_myLocationTintColor,
          ColorUtils.getPrimaryColor(context)));
      mapboxMapOptions.myLocationBackgroundTintColor(
        typedArray.getColor(R.styleable.mapbox_MapView_mapbox_myLocationBackgroundTintColor, Color.WHITE));

      Drawable foregroundDrawable = typedArray.getDrawable(R.styleable.mapbox_MapView_mapbox_myLocationDrawable);
      if (foregroundDrawable == null) {
        foregroundDrawable = ContextCompat.getDrawable(context, R.drawable.mapbox_mylocation_icon_default);
      }

      Drawable foregroundBearingDrawable = typedArray.getDrawable(
        R.styleable.mapbox_MapView_mapbox_myLocationBearingDrawable);
      if (foregroundBearingDrawable == null) {
        foregroundBearingDrawable = ContextCompat.getDrawable(context, R.drawable.mapbox_mylocation_icon_bearing);
      }

      Drawable backgroundDrawable = typedArray.getDrawable(
        R.styleable.mapbox_MapView_mapbox_myLocationBackgroundDrawable);
      if (backgroundDrawable == null) {
        backgroundDrawable = ContextCompat.getDrawable(context, R.drawable.mapbox_mylocation_bg_shape);
      }

      mapboxMapOptions.myLocationForegroundDrawables(foregroundDrawable, foregroundBearingDrawable);
      mapboxMapOptions.myLocationBackgroundDrawable(backgroundDrawable);
      mapboxMapOptions.myLocationBackgroundPadding(new int[] {
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_myLocationBackgroundMarginLeft,
          0) * pxlRatio),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_myLocationBackgroundMarginTop,
          0) * pxlRatio),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_myLocationBackgroundMarginRight,
          0) * pxlRatio),
        (int) (typedArray.getDimension(R.styleable.mapbox_MapView_mapbox_myLocationBackgroundMarginBottom,
          0) * pxlRatio)
      });
      mapboxMapOptions.myLocationAccuracyAlpha(
        typedArray.getInt(R.styleable.mapbox_MapView_mapbox_myLocationAccuracyAlpha, 100));
      mapboxMapOptions.myLocationAccuracyTint(
        typedArray.getColor(R.styleable.mapbox_MapView_mapbox_myLocationAccuracyTintColor,
          ColorUtils.getPrimaryColor(context)));
    } finally {
      typedArray.recycle();
    }
    return mapboxMapOptions;
  }

  /**
   * Specifies the URL used for API endpoint.
   *
   * @param apiBaseUrl The base of our API endpoint
   * @return This
   */
  public MapboxMapOptions apiBaseUrl(String apiBaseUrl) {
    this.apiBaseUrl = apiBaseUrl;
    return this;
  }

  /**
   * Specifies a the initial camera position for the map view.
   *
   * @param cameraPosition Inital camera position
   * @return This
   */
  public MapboxMapOptions camera(CameraPosition cameraPosition) {
    this.cameraPosition = cameraPosition;
    return this;
  }

  /**
   * Specifies the style url associated with a map view.
   *
   * @param styleUrl Url to be used to load a style
   * @return This
   */
  public MapboxMapOptions styleUrl(String styleUrl) {
    style = styleUrl;
    return this;
  }

  /**
   * Specifies the used debug type for a map view.
   *
   * @param enabled True is debug is enabled
   * @return This
   */
  public MapboxMapOptions debugActive(boolean enabled) {
    debugActive = enabled;
    return this;
  }

  /**
   * Specifies the used minimum zoom level for a map view.
   *
   * @param minZoom Zoom level to be used
   * @return This
   */
  public MapboxMapOptions minZoomPreference(double minZoom) {
    this.minZoom = minZoom;
    return this;
  }

  /**
   * Specifies the used maximum zoom level for a map view.
   *
   * @param maxZoom Zoom level to be used
   * @return This
   */
  public MapboxMapOptions maxZoomPreference(double maxZoom) {
    this.maxZoom = maxZoom;
    return this;
  }

  /**
   * Specifies the visibility state of a mapbox_compass_icon for a map view.
   *
   * @param enabled True and mapbox_compass_icon is shown
   * @return This
   */
  public MapboxMapOptions compassEnabled(boolean enabled) {
    compassEnabled = enabled;
    return this;
  }

  /**
   * Specifies the gravity state of mapbox_compass_icon for a map view.
   *
   * @param gravity see {@link android.view.Gravity}
   * @return This
   */
  public MapboxMapOptions compassGravity(int gravity) {
    compassGravity = gravity;
    return this;
  }

  /**
   * Specifies the margin state of mapbox_compass_icon for a map view
   *
   * @param margins 4 long array for LTRB margins
   * @return This
   */
  public MapboxMapOptions compassMargins(int[] margins) {
    compassMargins = margins;
    return this;
  }

  /**
   * Specifies if the mapbox_compass_icon fades to invisible when facing north.
   * <p>
   * By default this value is true.
   * </p>
   *
   * @param compassFadeWhenFacingNorth true is mapbox_compass_icon fades to invisble
   * @return This
   */
  public MapboxMapOptions compassFadesWhenFacingNorth(boolean compassFadeWhenFacingNorth) {
    this.fadeCompassFacingNorth = compassFadeWhenFacingNorth;
    return this;
  }

  /**
   * Specifies the visibility state of a logo for a map view.
   *
   * @param enabled True and logo is shown
   * @return This
   */
  public MapboxMapOptions logoEnabled(boolean enabled) {
    logoEnabled = enabled;
    return this;
  }

  /**
   * Specifies the gravity state of logo for a map view.
   *
   * @param gravity see {@link android.view.Gravity}
   * @return This
   */
  public MapboxMapOptions logoGravity(int gravity) {
    logoGravity = gravity;
    return this;
  }

  /**
   * Specifies the margin state of logo for a map view
   *
   * @param margins 4 long array for LTRB margins
   * @return This
   */
  public MapboxMapOptions logoMargins(int[] margins) {
    logoMargins = margins;
    return this;
  }

  /**
   * Specifies the visibility state of a attribution for a map view.
   *
   * @param enabled True and attribution is shown
   * @return This
   */
  public MapboxMapOptions attributionEnabled(boolean enabled) {
    attributionEnabled = enabled;
    return this;
  }

  /**
   * Specifies the gravity state of attribution for a map view.
   *
   * @param gravity see {@link android.view.Gravity}
   * @return This
   */
  public MapboxMapOptions attributionGravity(int gravity) {
    attributionGravity = gravity;
    return this;
  }

  /**
   * Specifies the margin state of attribution for a map view
   *
   * @param margins 4 long array for LTRB margins
   * @return This
   */
  public MapboxMapOptions attributionMargins(int[] margins) {
    attributionMargins = margins;
    return this;
  }

  /**
   * Specifies the tint color of the attribution for a map view
   *
   * @param color integer resembling a color
   * @return This
   */
  public MapboxMapOptions attributionTintColor(@ColorInt int color) {
    attributionTintColor = color;
    return this;
  }

  /**
   * Specifies if the rotate gesture is enabled for a map view.
   *
   * @param enabled True and gesture will be enabled
   * @return This
   */
  public MapboxMapOptions rotateGesturesEnabled(boolean enabled) {
    rotateGesturesEnabled = enabled;
    return this;
  }

  /**
   * Specifies if the scroll gesture is enabled for a map view.
   *
   * @param enabled True and gesture will be enabled
   * @return This
   */
  public MapboxMapOptions scrollGesturesEnabled(boolean enabled) {
    scrollGesturesEnabled = enabled;
    return this;
  }

  /**
   * Specifies if the tilt gesture is enabled for a map view.
   *
   * @param enabled True and gesture will be enabled
   * @return This
   */
  public MapboxMapOptions tiltGesturesEnabled(boolean enabled) {
    tiltGesturesEnabled = enabled;
    return this;
  }

  /**
   * Specifies if the zoom controls are enabled for a map view.
   *
   * @param enabled True and gesture will be enabled
   * @return This
   */
  public MapboxMapOptions zoomControlsEnabled(boolean enabled) {
    zoomControlsEnabled = enabled;
    return this;
  }

  /**
   * Specifies if the zoom gesture is enabled for a map view.
   *
   * @param enabled True and gesture will be enabled
   * @return This
   */
  public MapboxMapOptions zoomGesturesEnabled(boolean enabled) {
    zoomGesturesEnabled = enabled;
    return this;
  }

  /**
   * Specifies if the double tap gesture is enabled for a map view.
   *
   * @param enabled True and gesture will be enabled
   * @return This
   */
  public MapboxMapOptions doubleTapGesturesEnabled(boolean enabled) {
    doubleTapGesturesEnabled = enabled;
    return this;
  }

  /**
   * Specifies if the user location view is enabled for a map view.
   *
   * @param locationEnabled True and gesture will be enabled
   * @return This
   */
  public MapboxMapOptions locationEnabled(boolean locationEnabled) {
    this.myLocationEnabled = locationEnabled;
    return this;
  }

  /**
   * Set the foreground drawables of the MyLocationView.
   *
   * @param myLocationForegroundDrawable the drawable to show as foreground without bearing
   * @param myLocationBearingDrawable    the drawable to show as foreground when bearing is disabled
   * @return This
   */
  public MapboxMapOptions myLocationForegroundDrawables(Drawable myLocationForegroundDrawable,
                                                        Drawable myLocationBearingDrawable) {
    this.myLocationForegroundDrawable = myLocationForegroundDrawable;
    this.myLocationForegroundBearingDrawable = myLocationBearingDrawable;
    return this;
  }

  /**
   * Set the foreground drawable of the MyLocationView.
   * <p>
   * The same drawable will be used for both bearing as non bearing modes.
   * </p>
   *
   * @param myLocationForegroundDrawable the drawable to show as foreground
   * @return This
   */
  public MapboxMapOptions myLocationForegroundDrawable(Drawable myLocationForegroundDrawable) {
    this.myLocationForegroundDrawable = myLocationForegroundDrawable;
    return this;
  }

  /**
   * Set the background drawable of MyLocationView.
   * <p>
   * Padding can be added to provide an offset to the background.
   * </p>
   *
   * @param myLocationBackgroundDrawable the drawable to show as background
   * @return This
   */
  public MapboxMapOptions myLocationBackgroundDrawable(Drawable myLocationBackgroundDrawable) {
    this.myLocationBackgroundDrawable = myLocationBackgroundDrawable;
    return this;
  }

  /**
   * Set the foreground tint color of MyLocationView.
   * <p>
   * The color will tint both the foreground and the bearing foreground drawable.
   * </p>
   *
   * @param myLocationForegroundTintColor the color to tint the foreground drawable
   * @return This
   */
  public MapboxMapOptions myLocationForegroundTintColor(@ColorInt int myLocationForegroundTintColor) {
    this.myLocationForegroundTintColor = myLocationForegroundTintColor;
    return this;
  }

  /**
   * Set the background tint color of MyLocationView.
   *
   * @param myLocationBackgroundTintColor the color to tint the background
   * @return This
   */
  public MapboxMapOptions myLocationBackgroundTintColor(@ColorInt int myLocationBackgroundTintColor) {
    this.myLocationBackgroundTintColor = myLocationBackgroundTintColor;
    return this;
  }

  /**
   * Set the MyLocationView padding.
   *
   * @param myLocationBackgroundPadding the color to tint the background
   * @return This
   */
  public MapboxMapOptions myLocationBackgroundPadding(int[] myLocationBackgroundPadding) {
    this.myLocationBackgroundPadding = myLocationBackgroundPadding;
    return this;
  }

  /**
   * Set the MyLocationView accuracy circle tint color.
   *
   * @param myLocationAccuracyTintColor the color to tint the accuracy circle
   * @return This
   */
  public MapboxMapOptions myLocationAccuracyTint(@ColorInt int myLocationAccuracyTintColor) {
    this.myLocationAccuracyTintColor = myLocationAccuracyTintColor;
    return this;
  }

  /**
   * Set the MyLocationView accuracy alpha value.
   *
   * @param alpha the alpha value
   * @return This
   */
  public MapboxMapOptions myLocationAccuracyAlpha(@IntRange(from = 0, to = 255) int alpha) {
    this.myLocationAccuracyAlpha = alpha;
    return this;
  }

  /**
   * Get the current configured API endpoint base URL.
   *
   * @return Base URL to be used API endpoint.
   */
  public String getApiBaseUrl() {
    return apiBaseUrl;
  }

  /**
   * Get the current configured initial camera position for a map view.
   *
   * @return CameraPosition to be initially used.
   */
  public CameraPosition getCamera() {
    return cameraPosition;
  }

  /**
   * Get the current configured min zoom for a map view.
   *
   * @return Mininum zoom level to be used.
   */
  public double getMinZoomPreference() {
    return minZoom;
  }

  /**
   * Get the current configured maximum zoom for a map view.
   *
   * @return Maximum zoom to be used.
   */
  public double getMaxZoomPreference() {
    return maxZoom;
  }

  /**
   * Get the current configured visibility state for mapbox_compass_icon for a map view.
   *
   * @return Visibility state of the mapbox_compass_icon
   */
  public boolean getCompassEnabled() {
    return compassEnabled;
  }

  /**
   * Get the current configured gravity state for mapbox_compass_icon for a map view.
   *
   * @return Gravity state of the mapbox_compass_icon
   */
  public int getCompassGravity() {
    return compassGravity;
  }

  /**
   * Get the current configured margins for mapbox_compass_icon for a map view.
   *
   * @return Margins state of the mapbox_compass_icon
   */
  public int[] getCompassMargins() {
    return compassMargins;
  }

  /**
   * Get the current configured state for fading the mapbox_compass_icon when facing north.
   *
   * @return True if mapbox_compass_icon fades to invisible when facing north
   */
  public boolean getCompassFadeFacingNorth() {
    return fadeCompassFacingNorth;
  }

  /**
   * Get the current configured visibility state for mapbox_compass_icon for a map view.
   *
   * @return Visibility state of the mapbox_compass_icon
   */
  public boolean getLogoEnabled() {
    return logoEnabled;
  }

  /**
   * Get the current configured gravity state for logo for a map view.
   *
   * @return Gravity state of the logo
   */
  public int getLogoGravity() {
    return logoGravity;
  }

  /**
   * Get the current configured margins for logo for a map view.
   *
   * @return Margins state of the logo
   */
  public int[] getLogoMargins() {
    return logoMargins;
  }

  /**
   * Get the current configured style url for a map view.
   *
   * @return Style url to be used.
   */
  public String getStyle() {
    return style;
  }

  /**
   * Get the current configured rotate gesture state for a map view.
   *
   * @return True indicates gesture is enabled
   */
  public boolean getRotateGesturesEnabled() {
    return rotateGesturesEnabled;
  }

  /**
   * Get the current configured scroll gesture state for a map view.
   *
   * @return True indicates gesture is enabled
   */
  public boolean getScrollGesturesEnabled() {
    return scrollGesturesEnabled;
  }

  /**
   * Get the current configured tilt gesture state for a map view.
   *
   * @return True indicates gesture is enabled
   */
  public boolean getTiltGesturesEnabled() {
    return tiltGesturesEnabled;
  }

  /**
   * Get the current configured zoom controls state for a map view.
   *
   * @return True indicates gesture is enabled
   */
  public boolean getZoomControlsEnabled() {
    return zoomControlsEnabled;
  }

  /**
   * Get the current configured zoom gesture state for a map view.
   *
   * @return True indicates gesture is enabled
   */
  public boolean getZoomGesturesEnabled() {
    return zoomGesturesEnabled;
  }

  /**
   * Get the current configured double tap gesture state for a map view.
   *
   * @return True indicates gesture is enabled
   */
  public boolean getDoubleTapGesturesEnabled() {
    return doubleTapGesturesEnabled;
  }

  /**
   * Get the current configured visibility state for attribution for a map view.
   *
   * @return Visibility state of the attribution
   */
  public boolean getAttributionEnabled() {
    return attributionEnabled;
  }

  /**
   * Get the current configured gravity state for attribution for a map view.
   *
   * @return Gravity state of the logo
   */
  public int getAttributionGravity() {
    return attributionGravity;
  }

  /**
   * Get the current configured margins for attribution for a map view.
   *
   * @return Margins state of the logo
   */
  public int[] getAttributionMargins() {
    return attributionMargins;
  }

  /**
   * Get the current configured tint color for attribution for a map view.
   *
   * @return the tint color
   */
  @ColorInt
  public int getAttributionTintColor() {
    return attributionTintColor;
  }

  /**
   * Get the current configured user location view state for a map view.
   *
   * @return True and user location will be shown
   */
  public boolean getLocationEnabled() {
    return myLocationEnabled;
  }

  /**
   * Get the current configured MyLocationView foreground drawable.
   *
   * @return the drawable used as foreground
   */
  public Drawable getMyLocationForegroundDrawable() {
    return myLocationForegroundDrawable;
  }

  /**
   * Get the current configured MyLocationView foreground bearing drawable.
   *
   * @return the drawable used as foreground when bearing is enabled
   */
  public Drawable getMyLocationForegroundBearingDrawable() {
    return myLocationForegroundBearingDrawable;
  }

  /**
   * Get the current configured MyLocationView background drawable.
   *
   * @return the drawable used as background
   */
  public Drawable getMyLocationBackgroundDrawable() {
    return myLocationBackgroundDrawable;
  }

  /**
   * Get the current configured MyLocationView foreground tint color.
   *
   * @return the tint color
   */
  public int getMyLocationForegroundTintColor() {
    return myLocationForegroundTintColor;
  }

  /**
   * Get the current configured MyLocationView background tint color.
   *
   * @return the tint color
   */
  public int getMyLocationBackgroundTintColor() {
    return myLocationBackgroundTintColor;
  }

  /**
   * Get the current configured MyLocationView background padding.
   *
   * @return an array describing the padding in a LTRB manner
   */
  public int[] getMyLocationBackgroundPadding() {
    return myLocationBackgroundPadding;
  }

  /**
   * Get the current configured MyLocationView accuracy circle color tint value.
   *
   * @return the tint color
   */
  public int getMyLocationAccuracyTintColor() {
    return myLocationAccuracyTintColor;
  }

  /**
   * Get the current configured MyLocationView accuracy circle alpha value.
   *
   * @return the alpha value
   */
  public int getMyLocationAccuracyAlpha() {
    return myLocationAccuracyAlpha;
  }

  /**
   * Get the current configured debug state for a map view.
   *
   * @return True indicates debug is enabled.
   */
  public boolean getDebugActive() {
    return debugActive;
  }

  public static final Parcelable.Creator<MapboxMapOptions> CREATOR = new Parcelable.Creator<MapboxMapOptions>() {
    public MapboxMapOptions createFromParcel(Parcel in) {
      return new MapboxMapOptions(in);
    }

    public MapboxMapOptions[] newArray(int size) {
      return new MapboxMapOptions[size];
    }
  };

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
    dest.writeParcelable(cameraPosition, flags);
    dest.writeByte((byte) (debugActive ? 1 : 0));

    dest.writeByte((byte) (compassEnabled ? 1 : 0));
    dest.writeInt(compassGravity);
    dest.writeIntArray(compassMargins);
    dest.writeByte((byte) (fadeCompassFacingNorth ? 1 : 0));

    dest.writeByte((byte) (logoEnabled ? 1 : 0));
    dest.writeInt(logoGravity);
    dest.writeIntArray(logoMargins);

    dest.writeByte((byte) (attributionEnabled ? 1 : 0));
    dest.writeInt(attributionGravity);
    dest.writeIntArray(attributionMargins);
    dest.writeInt(attributionTintColor);

    dest.writeDouble(minZoom);
    dest.writeDouble(maxZoom);

    dest.writeByte((byte) (rotateGesturesEnabled ? 1 : 0));
    dest.writeByte((byte) (scrollGesturesEnabled ? 1 : 0));
    dest.writeByte((byte) (tiltGesturesEnabled ? 1 : 0));
    dest.writeByte((byte) (zoomControlsEnabled ? 1 : 0));
    dest.writeByte((byte) (zoomGesturesEnabled ? 1 : 0));
    dest.writeByte((byte) (doubleTapGesturesEnabled ? 1 : 0));

    dest.writeByte((byte) (myLocationEnabled ? 1 : 0));

    dest.writeParcelable(myLocationForegroundDrawable != null
      ? getBitmapFromDrawable(myLocationForegroundDrawable) : null, flags);
    dest.writeParcelable(myLocationForegroundBearingDrawable != null
      ? getBitmapFromDrawable(myLocationForegroundBearingDrawable) : null, flags);
    dest.writeParcelable(myLocationBackgroundDrawable != null
      ? getBitmapFromDrawable(myLocationBackgroundDrawable) : null, flags);
    dest.writeInt(myLocationForegroundTintColor);
    dest.writeInt(myLocationBackgroundTintColor);
    dest.writeIntArray(myLocationBackgroundPadding);
    dest.writeInt(myLocationAccuracyAlpha);
    dest.writeInt(myLocationAccuracyTintColor);

    dest.writeString(style);
    dest.writeString(apiBaseUrl);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    MapboxMapOptions options = (MapboxMapOptions) o;

    if (debugActive != options.debugActive) {
      return false;
    }
    if (compassEnabled != options.compassEnabled) {
      return false;
    }
    if (fadeCompassFacingNorth != options.fadeCompassFacingNorth) {
      return false;
    }
    if (compassGravity != options.compassGravity) {
      return false;
    }
    if (logoEnabled != options.logoEnabled) {
      return false;
    }
    if (logoGravity != options.logoGravity) {
      return false;
    }
    if (attributionTintColor != options.attributionTintColor) {
      return false;
    }
    if (attributionEnabled != options.attributionEnabled) {
      return false;
    }
    if (attributionGravity != options.attributionGravity) {
      return false;
    }
    if (Double.compare(options.minZoom, minZoom) != 0) {
      return false;
    }
    if (Double.compare(options.maxZoom, maxZoom) != 0) {
      return false;
    }
    if (rotateGesturesEnabled != options.rotateGesturesEnabled) {
      return false;
    }
    if (scrollGesturesEnabled != options.scrollGesturesEnabled) {
      return false;
    }
    if (tiltGesturesEnabled != options.tiltGesturesEnabled) {
      return false;
    }
    if (zoomGesturesEnabled != options.zoomGesturesEnabled) {
      return false;
    }
    if (zoomControlsEnabled != options.zoomControlsEnabled) {
      return false;
    }
    if (doubleTapGesturesEnabled != options.doubleTapGesturesEnabled) {
      return false;
    }
    if (myLocationEnabled != options.myLocationEnabled) {
      return false;
    }
    if (myLocationForegroundTintColor != options.myLocationForegroundTintColor) {
      return false;
    }
    if (myLocationBackgroundTintColor != options.myLocationBackgroundTintColor) {
      return false;
    }
    if (myLocationAccuracyTintColor != options.myLocationAccuracyTintColor) {
      return false;
    }
    if (myLocationAccuracyAlpha != options.myLocationAccuracyAlpha) {
      return false;
    }
    if (cameraPosition != null ? !cameraPosition.equals(options.cameraPosition) : options.cameraPosition != null) {
      return false;
    }
    if (!Arrays.equals(compassMargins, options.compassMargins)) {
      return false;
    }
    if (!Arrays.equals(logoMargins, options.logoMargins)) {
      return false;
    }
    if (!Arrays.equals(attributionMargins, options.attributionMargins)) {
      return false;
    }
    if (myLocationForegroundDrawable != null
      ? !myLocationForegroundDrawable.equals(options.myLocationForegroundDrawable)
      : options.myLocationForegroundDrawable != null) {
      return false;
    }
    if (myLocationForegroundBearingDrawable != null
      ? !myLocationForegroundBearingDrawable.equals(options.myLocationForegroundBearingDrawable)
      : options.myLocationForegroundBearingDrawable != null) {
      return false;
    }
    if (myLocationBackgroundDrawable != null
      ? !myLocationBackgroundDrawable.equals(options.myLocationBackgroundDrawable)
      : options.myLocationBackgroundDrawable != null) {
      return false;
    }
    if (!Arrays.equals(myLocationBackgroundPadding, options.myLocationBackgroundPadding)) {
      return false;
    }
    if (style != null ? !style.equals(options.style) : options.style != null) {
      return false;
    }
    if (apiBaseUrl != null ? !apiBaseUrl.equals(options.apiBaseUrl) : options.apiBaseUrl != null) {
      return false;
    }
    return false;
  }

  @Override
  public int hashCode() {
    int result;
    long temp;
    result = cameraPosition != null ? cameraPosition.hashCode() : 0;
    result = 31 * result + (debugActive ? 1 : 0);
    result = 31 * result + (compassEnabled ? 1 : 0);
    result = 31 * result + (fadeCompassFacingNorth ? 1 : 0);
    result = 31 * result + compassGravity;
    result = 31 * result + Arrays.hashCode(compassMargins);
    result = 31 * result + (logoEnabled ? 1 : 0);
    result = 31 * result + logoGravity;
    result = 31 * result + Arrays.hashCode(logoMargins);
    result = 31 * result + attributionTintColor;
    result = 31 * result + (attributionEnabled ? 1 : 0);
    result = 31 * result + attributionGravity;
    result = 31 * result + Arrays.hashCode(attributionMargins);
    temp = Double.doubleToLongBits(minZoom);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(maxZoom);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    result = 31 * result + (rotateGesturesEnabled ? 1 : 0);
    result = 31 * result + (scrollGesturesEnabled ? 1 : 0);
    result = 31 * result + (tiltGesturesEnabled ? 1 : 0);
    result = 31 * result + (zoomGesturesEnabled ? 1 : 0);
    result = 31 * result + (zoomControlsEnabled ? 1 : 0);
    result = 31 * result + (doubleTapGesturesEnabled ? 1 : 0);
    result = 31 * result + (myLocationEnabled ? 1 : 0);
    result = 31 * result + (myLocationForegroundDrawable != null ? myLocationForegroundDrawable.hashCode() : 0);
    result = 31 * result + (myLocationForegroundBearingDrawable != null
      ? myLocationForegroundBearingDrawable.hashCode() : 0);
    result = 31 * result + (myLocationBackgroundDrawable != null ? myLocationBackgroundDrawable.hashCode() : 0);
    result = 31 * result + myLocationForegroundTintColor;
    result = 31 * result + myLocationBackgroundTintColor;
    result = 31 * result + Arrays.hashCode(myLocationBackgroundPadding);
    result = 31 * result + myLocationAccuracyTintColor;
    result = 31 * result + myLocationAccuracyAlpha;
    result = 31 * result + (apiBaseUrl != null ? apiBaseUrl.hashCode() : 0);
    result = 31 * result + (style != null ? style.hashCode() : 0);
    return result;
  }
}
