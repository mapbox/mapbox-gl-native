package com.mapbox.mapboxsdk.maps;

import android.support.annotation.StringDef;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.HashMap;

public class Style {

  private NativeMapView nativeMapView;
  private final HashMap<String, Source> sources = new HashMap<>();
  private final HashMap<String, Layer> layers = new HashMap<>();

  public Style(NativeMapView nativeMapView) {
    this.nativeMapView = nativeMapView;
  }

  public void loadStyle(@StyleUrl String styleUrl) {
    for (Source source : sources.values()) {
      if (source != null) {
        source.setDetached();
        nativeMapView.removeSource(source);
      }
    }

    for (Layer layer : layers.values()) {
      if (layer != null) {
        layer.setDetached();
        nativeMapView.removeLayer(layer);
      }
    }

    nativeMapView.setStyleUrl(styleUrl);
  }

  //
  // Source
  //

  public void addSource(Source source) {
    sources.put(source.getId(), source);
    nativeMapView.addSource(source);
  }

  public Source getSource(String id) {
    Source source = sources.get(id);
    if (source == null) {
      source = nativeMapView.getSource(id);
    }
    return source;
  }

  //
  // Layer
  //

  public void addLayer(Layer layer) {
    layers.put(layer.getId(), layer);
    nativeMapView.addLayer(layer);
  }

  public Layer getLayer(String id) {
    Layer layer = layers.get(id);
    if (layer == null) {
      layer = nativeMapView.getLayer(id);
    }
    return layer;
  }

  //
  // Transition
  //


  /**
   * <p>
   * Set the transition duration for style changes.
   * </p>
   * The default value for delay and duration is zero, so any changes take effect without animation.
   *
   * @param transitionOptions the transition options
   */
  public void setTransition(TransitionOptions transitionOptions) {
    nativeMapView.setTransitionDuration(transitionOptions.getDuration());
    nativeMapView.setTransitionDelay(transitionOptions.getDelay());
  }

  /**
   * <p>
   * Get the transition for style changes.
   * </p>
   * The default value for delay and transition is zero, so any changes take effect without animation.
   *
   * @return TransitionOptions the transition options
   */
  public TransitionOptions getTransition() {
    return new TransitionOptions(nativeMapView.getTransitionDuration(), nativeMapView.getTransitionDelay());
  }

  //
  // Style constants
  //

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
