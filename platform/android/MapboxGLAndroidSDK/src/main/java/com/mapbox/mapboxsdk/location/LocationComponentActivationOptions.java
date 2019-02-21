package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.mapboxsdk.maps.Style;

/**
 * A class which holds the various options for activating the Maps SDK's {@link LocationComponent} to eventually
 * show the device location on the map.
 *
 * since 7.2.0
 */
public class LocationComponentActivationOptions {

  private final Context context;
  private final Style style;
  private final LocationEngine locationEngine;
  private final LocationEngineRequest locationEngineRequest;
  private final LocationComponentOptions locationComponentOptions;
  private final int styleRes;
  private final boolean useDefaultLocationEngine;

  LocationComponentActivationOptions(@NonNull Context context, @NonNull Style style,
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
   * @return the engine, or null if you'd like to only force location updates
   */
  @Nullable
  public LocationEngine locationEngine() {
    return locationEngine;
  }

  /**
   * The location request which the {@link LocationComponent} should use
   * @return
   */
  @Nullable
  public LocationEngineRequest locationEngineRequest() {
    return locationEngineRequest;
  }

  /**
   * A built {@link LocationComponentOptions} object, which holds the various {@link LocationComponent} styling options
   * @return the options for styling the actual LocationComponent
   */
  @Nullable
  public LocationComponentOptions locationComponentOptions() {
    return locationComponentOptions;
  }

  /**
   * the LocationComponent style resource (e.g. R.style.style_name)
   *
   * @return the style resource.
   */
  @Nullable
  public int styleRes() {
    return styleRes;
  }

  /**
   * True if you want to initialize and use the built-in location engine or false if there should be no
   * location engine initialized
   * @return whether the default LocationEngine is used
   */
  @Nullable
  public boolean useDefaultLocationEngine() {
    return useDefaultLocationEngine;
  }

  /**
   * Builder class for constructing a new instance of {@link LocationComponentActivationOptions}.
   *
   * since 7.2.0
   */
  public static class Builder {
    private Context context;
    private Style style;
    private LocationEngine locationEngine;
    private LocationEngineRequest locationEngineRequest;
    private LocationComponentOptions locationComponentOptions;
    private int styleRes;
    private boolean useDefaultLocationEngine;

    public Builder(Context context, Style style) {
      this.context = context;
      this.style = style;
    }

    public Builder locationEngine(LocationEngine locationEngine) {
      this.locationEngine = locationEngine;
      return this;
    }

    public Builder locationEngineRequest(LocationEngineRequest locationEngineRequest) {
      this.locationEngineRequest = locationEngineRequest;
      return this;
    }

    public Builder locationComponentOptions(LocationComponentOptions locationComponentOptions) {
      this.locationComponentOptions = locationComponentOptions;
      return this;
    }

    public Builder styleRes(int styleRes) {
      this.styleRes = styleRes;
      return this;
    }

    public Builder useDefaultLocationEngine(boolean useDefaultLocationEngine) {
      this.useDefaultLocationEngine = useDefaultLocationEngine;
      return this;
    }

    public LocationComponentActivationOptions build() {
      if (styleRes != 0 && locationComponentOptions != null) {
        throw new IllegalArgumentException(
          "You've provided both a style resource and a LocationComponentOptions object to the " +
            "LocationComponentActivationOptions builder. You can't use both and " +
            "you must choose one of the two to style the LocationComponent.");
      }
      if (context == null) {
        throw new NullPointerException(
          "Context in LocationComponentActivationOptions is null.");
      }
      if (style == null) {
        throw new NullPointerException(
          "Style in LocationComponentActivationOptions is null. Make sure the Style object isn't null." +
            " Wait for the map to fully load before passing the Style object to LocationComponentActivationOptions.");
      }
      if (!style.isFullyLoaded()) {
        throw new IllegalArgumentException(
          "Style in LocationComponentActivationOptions isn't fully loaded. Wait for the map to fully load before " +
            "passing the Style object to LocationComponentActivationOptions.");
      }
      return new LocationComponentActivationOptions(context, style, locationEngine, locationEngineRequest,
        locationComponentOptions, styleRes, useDefaultLocationEngine);
    }
  }
}