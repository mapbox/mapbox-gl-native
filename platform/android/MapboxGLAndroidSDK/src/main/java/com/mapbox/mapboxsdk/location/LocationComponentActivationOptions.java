package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.mapboxsdk.maps.Style;

/**
 * since 7.2.0
 */
public class LocationComponentActivationOptions {

  private Context context;
  private Style style;
  private LocationEngine locationEngine;
  private LocationEngineRequest locationEngineRequest;
  private LocationComponentOptions locationComponentOptions;
  private int styleRes;
  private boolean useDefaultLocationEngine;

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
  @Nullable
  public Context context() {
    return context;
  }

  /**
   * The proxy object for current map style. More info at {@link Style}
   *
   * @return the map's fully loaded Style object
   */
  @Nullable
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
   * the LocationComponent style res
   *
   * @return
   */
  @Nullable
  public int styleRes() {
    return styleRes;
  }

  /**
   * true if you want to initialize and use the built-in location engine or false if there should be no
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

    public Builder() {

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
      return new LocationComponentActivationOptions(context, style, locationEngine, locationEngineRequest,
        locationComponentOptions, styleRes, useDefaultLocationEngine);
    }
  }
}