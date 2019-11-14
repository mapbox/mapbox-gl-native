package com.mapbox.mapboxsdk.location;

import android.content.Context;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.mapboxsdk.maps.Style;

/**
 * A class which holds the various options for activating the Maps SDK's {@link LocationComponent} to eventually
 * show the device location on the map.
 */
public class LocationComponentActivationOptions {

  private final Context context;
  private final Style style;
  private final LocationEngine locationEngine;
  private final LocationEngineRequest locationEngineRequest;
  private final LocationComponentOptions locationComponentOptions;
  private final int styleRes;
  private final boolean useDefaultLocationEngine;

  private LocationComponentActivationOptions(@NonNull Context context, @NonNull Style style,
                                             @Nullable LocationEngine locationEngine,
                                             @Nullable LocationEngineRequest locationEngineRequest,
                                             @Nullable LocationComponentOptions locationComponentOptions,
                                             int styleRes,
                                             boolean useDefaultLocationEngine) {
    this.context = context;
    this.style = style;
    this.locationEngine = locationEngine;
    this.locationEngineRequest = locationEngineRequest;
    this.locationComponentOptions = locationComponentOptions;
    this.styleRes = styleRes;
    this.useDefaultLocationEngine = useDefaultLocationEngine;
  }

  /**
   * Convenience method to retrieve a {@link LocationComponentActivationOptions} object which is ready to build with
   *
   * @return a builder object
   */
  @NonNull
  public static Builder builder(@NonNull Context context, @NonNull Style fullyLoadedMapStyle) {
    return new LocationComponentActivationOptions.Builder(context, fullyLoadedMapStyle);
  }

  /**
   * The application's current context
   *
   * @return the application's current context
   */
  @NonNull
  public Context context() {
    return context;
  }

  /**
   * The proxy object for current map style. More info at {@link Style}
   *
   * @return the map's fully loaded Style object
   */
  @NonNull
  public Style style() {
    return style;
  }

  /**
   * The {@link LocationEngine} which the {@link LocationComponent} should use
   *
   * @return the engine
   */
  @Nullable
  public LocationEngine locationEngine() {
    return locationEngine;
  }

  /**
   * The location request which the {@link LocationComponent} should use
   *
   * @return the LocationEngineRequest object
   */
  @Nullable
  public LocationEngineRequest locationEngineRequest() {
    return locationEngineRequest;
  }

  /**
   * A built {@link LocationComponentOptions} object, which holds the various {@link LocationComponent} styling options
   *
   * @return the options for styling the actual LocationComponent
   */
  @Nullable
  public LocationComponentOptions locationComponentOptions() {
    return locationComponentOptions;
  }

  /**
   * The LocationComponent style resource (e.g. R.style.style_name)
   *
   * @return the style resource.
   */
  public int styleRes() {
    return styleRes;
  }

  /**
   * True if you want to initialize and use the built-in location engine or false if there should be no
   * location engine initialized
   *
   * @return whether the default LocationEngine is used
   */
  public boolean useDefaultLocationEngine() {
    return useDefaultLocationEngine;
  }

  /**
   * Builder class for constructing a new instance of {@link LocationComponentActivationOptions}.
   */
  public static class Builder {
    private final Context context;
    private final Style style;
    private LocationEngine locationEngine;
    private LocationEngineRequest locationEngineRequest;
    private LocationComponentOptions locationComponentOptions;
    private int styleRes;

    /**
     * Set to true as the default in case a true/false value isn't declared via the builder's
     * {@link LocationComponentActivationOptions#useDefaultLocationEngine()} method.
     * <p>
     * Please be aware that this activation boolean is ignored when a non-null
     * {@link LocationEngine} is set via the builder's `locationEngine()` method.
     */
    private boolean useDefaultLocationEngine = true;

    /**
     * Constructor for the {@link LocationComponentActivationOptions} builder class.
     * While other activation options are optional, the activation process always requires
     * context and a fully-loaded map {@link Style} object, which is why the two are in this
     * constructor.
     */
    public Builder(@NonNull Context context, @NonNull Style style) {
      this.context = context;
      this.style = style;
    }

    /**
     * Deliver your own {@link LocationEngine} to the LocationComponent.
     * <p>
     * The true/false
     * {@link LocationComponentActivationOptions.Builder#useDefaultLocationEngine()}
     * activation option is ignored when a non-null {@link LocationEngine} is set via
     * this `locationEngine()` method.
     *
     * @param locationEngine a {@link LocationEngine} object
     * @return the {@link Builder} object being constructed
     */
    @NonNull
    public Builder locationEngine(@Nullable LocationEngine locationEngine) {
      this.locationEngine = locationEngine;
      return this;
    }

    /**
     * @param locationEngineRequest the location request which the
     *                              {@link LocationComponent} should use
     * @return the {@link Builder} object being constructed
     */
    public Builder locationEngineRequest(LocationEngineRequest locationEngineRequest) {
      this.locationEngineRequest = locationEngineRequest;
      return this;
    }

    /**
     * @param locationComponentOptions a built {@link LocationComponentOptions} object,
     *                                 which holds the various {@link LocationComponent}
     *                                 styling options
     * @return the {@link Builder} object being constructed
     */
    public Builder locationComponentOptions(LocationComponentOptions locationComponentOptions) {
      this.locationComponentOptions = locationComponentOptions;
      return this;
    }

    /**
     * @param styleRes the LocationComponent style resource (e.g. R.style.style_name)
     * @return the {@link Builder} object being constructed
     */
    public Builder styleRes(int styleRes) {
      this.styleRes = styleRes;
      return this;
    }

    /**
     * @param useDefaultLocationEngine true if you want to initialize and use the
     *                                 built-in location engine or false if there
     *                                 should be no location engine initialized
     *                                 This is ignored when null is set as the parameter
     *                                 for {@link LocationComponentActivationOptions#builder
     *                                 (Context, Style)#locationEngine()}.
     * @return the {@link Builder} object being constructed
     */
    public Builder useDefaultLocationEngine(boolean useDefaultLocationEngine) {
      this.useDefaultLocationEngine = useDefaultLocationEngine;
      return this;
    }

    /**
     * Method which actually builds the {@link LocationComponentActivationOptions} object while
     * taking the various options into account.
     *
     * @return a built {@link LocationComponentActivationOptions} object
     */
    public LocationComponentActivationOptions build() {
      if (styleRes != 0 && locationComponentOptions != null) {
        throw new IllegalArgumentException(
            "You've provided both a style resource and a LocationComponentOptions object to the "
                + "LocationComponentActivationOptions builder. You can't use both and "
                + "you must choose one of the two to style the LocationComponent.");
      }
      if (context == null) {
        throw new NullPointerException(
            "Context in LocationComponentActivationOptions is null.");
      }
      if (style == null) {
        throw new NullPointerException(
            "Style in LocationComponentActivationOptions is null. Make sure the "
                + "Style object isn't null. Wait for the map to fully load before passing "
                + "the Style object to LocationComponentActivationOptions.");
      }
      if (!style.isFullyLoaded()) {
        throw new IllegalArgumentException(
            "Style in LocationComponentActivationOptions isn't fully loaded. Wait for the "
                + "map to fully load before passing the Style object to "
                + "LocationComponentActivationOptions.");
      }
      return new LocationComponentActivationOptions(context, style, locationEngine,
          locationEngineRequest, locationComponentOptions, styleRes, useDefaultLocationEngine);
    }
  }
}