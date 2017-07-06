package com.mapbox.mapboxsdk.constants;

import android.support.annotation.StringDef;

import com.mapbox.mapboxsdk.maps.MapView;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * <p>
 * Style provides URLs to several professional styles designed by Mapbox.
 * </p>
 * These styles are all ready to go in your app. To load one, pass it into {@link MapView#setStyleUrl(String)}
 *
 * @see MapView#setStyleUrl(String)
 */
public class Style {

  /**
   * Indicates the parameter accepts one of the values from Style. Using one of these
   * constants means your map style will always use the latest version and may change as we
   * improve the style
   */
  @StringDef( {MAPBOX_STREETS, OUTDOORS, LIGHT, DARK, SATELLITE, SATELLITE_STREETS, TRAFFIC_DAY, TRAFFIC_NIGHT})
  @Retention(RetentionPolicy.SOURCE)
  public @interface StyleUrl {
  }

  // IMPORTANT: If you change any of these you also need to edit them in strings.xml

  /**
   * Mapbox Streets: A complete basemap, perfect for incorporating your own data. Using this
   * constant means your map style will always use the latest version and may change as we
   * improve the style.
   */
  public static final String MAPBOX_STREETS = "mapbox://styles/mapbox/streets-v10";

  /**
   * Outdoors: A general-purpose style tailored to outdoor activities. Using this constant means
   * your map style will always use the latest version and may change as we improve the style.
   */
  public static final String OUTDOORS = "mapbox://styles/mapbox/outdoors-v10";

  /**
   * Light: Subtle light backdrop for data visualizations. Using this constant means your map
   * style will always use the latest version and may change as we improve the style.
   */
  public static final String LIGHT = "mapbox://styles/mapbox/light-v9";

  /**
   * Dark: Subtle dark backdrop for data visualizations. Using this constant means your map style
   * will always use the latest version and may change as we improve the style.
   */
  public static final String DARK = "mapbox://styles/mapbox/dark-v9";

  /**
   * Satellite: A beautiful global satellite and aerial imagery layer. Using this constant means
   * your map style will always use the latest version and may change as we improve the style.
   */
  public static final String SATELLITE = "mapbox://styles/mapbox/satellite-v9";

  /**
   * Satellite Streets: Global satellite and aerial imagery with unobtrusive labels. Using this
   * constant means your map style will always use the latest version and may change as we
   * improve the style.
   */
  public static final String SATELLITE_STREETS = "mapbox://styles/mapbox/satellite-streets-v10";

  /**
   * Traffic Day: Color-coded roads based on live traffic congestion data. Traffic data is currently
   * available in
   * <a href="https://www.mapbox.com/api-documentation/pages/traffic-countries.html">these select
   * countries</a>. Using this constant means your map style will always use the latest version and
   * may change as we improve the style.
   */
  public static final String TRAFFIC_DAY = "mapbox://styles/mapbox/traffic-day-v2";

  /**
   * Traffic Night: Color-coded roads based on live traffic congestion data, designed to maximize
   * legibility in low-light situations. Traffic data is currently available in
   * <a href="https://www.mapbox.com/api-documentation/pages/traffic-countries.html">these select
   * countries</a>. Using this constant means your map style will always use the latest version and
   * may change as we improve the style.
   */
  public static final String TRAFFIC_NIGHT = "mapbox://styles/mapbox/traffic-night-v2";
}
