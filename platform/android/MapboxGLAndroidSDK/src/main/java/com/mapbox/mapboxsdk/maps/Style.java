package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringDef;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class Style {

  private final NativeMapView nativeMapView;
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

  @Nullable
  public String getUrl(){
    return nativeMapView.getStyleUrl();
  }

  @Nullable
  public String getJson(){
    return nativeMapView.getStyleJson();
  }

  //
  // Source
  //

  /**
   * Retrieve all the sources in the style
   *
   * @return all the sources in the current style
   */
  public List<Source> getSources() {
    return nativeMapView.getSources();
  }

  /**
   * Adds the source to the map. The source must be newly created and not added to the map before
   *
   * @param source the source to add
   */
  public void addSource(Source source) {
    sources.put(source.getId(), source);
    nativeMapView.addSource(source);
  }

  /**
   * Retrieve a source by id
   *
   * @param id the source's id
   * @return the source if present in the current style
   */
  public Source getSource(String id) {
    Source source = sources.get(id);
    if (source == null) {
      source = nativeMapView.getSource(id);
    }
    return source;
  }

  /**
   * Tries to cast the Source to T, throws ClassCastException if it's another type.
   *
   * @param sourceId the id used to look up a layer
   * @param <T>      the generic type of a Source
   * @return the casted Source, null if another type
   */
  public <T extends Source> T getSourceAs(@NonNull String sourceId) {
    // noinspection unchecked
    if (sources.containsKey(sourceId)) {
      return (T) sources.get(sourceId);
    }
    return (T) nativeMapView.getSource(sourceId);
  }

  /**
   * Removes the source. Any references to the source become invalid and should not be used anymore
   *
   * @param sourceId the source to remove
   * @return the source handle or null if the source was not present
   */
  @Nullable
  public Source removeSource(@NonNull String sourceId) {
    sources.remove(sourceId);
    return nativeMapView.removeSource(sourceId);
  }

  /**
   * Removes the source, preserving the reference for re-use
   *
   * @param source the source to remove
   * @return the source
   */
  @Nullable
  public Source removeSource(@NonNull Source source) {
    sources.remove(source.getId());
    return nativeMapView.removeSource(source);
  }

  //
  // Layer
  //

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   */
  public void addLayer(Layer layer) {
    layers.put(layer.getId(), layer);
    nativeMapView.addLayer(layer);
  }

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   * @param below the layer id to add this layer before
   */
  public void addLayerBelow(@NonNull Layer layer, @NonNull String below) {
    layers.put(layer.getId(), layer);
    nativeMapView.addLayerBelow(layer, below);
  }

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   * @param above the layer id to add this layer above
   */
  public void addLayerAbove(@NonNull Layer layer, @NonNull String above) {
    layers.put(layer.getId(), layer);
    nativeMapView.addLayerAbove(layer, above);
  }

  /**
   * Adds the layer to the map at the specified index. The layer must be newly
   * created and not added to the map before
   *
   * @param layer the layer to add
   * @param index the index to insert the layer at
   */
  public void addLayerAt(@NonNull Layer layer, @IntRange(from = 0) int index) {
    layers.put(layer.getId(), layer);
    nativeMapView.addLayerAt(layer, index);
  }

  /**
   * Get the layer by id
   *
   * @param id the layer's id
   * @return the layer, if present in the style
   */
  public Layer getLayer(String id) {
    Layer layer = layers.get(id);
    if (layer == null) {
      layer = nativeMapView.getLayer(id);
    }
    return layer;
  }

  /**
   * Tries to cast the Layer to T, throws ClassCastException if it's another type.
   *
   * @param layerId the layer id used to look up a layer
   * @param <T>     the generic attribute of a Layer
   * @return the casted Layer, null if another type
   */
  public <T extends Layer> T getLayerAs(@NonNull String layerId) {
    // noinspection unchecked
    return (T) nativeMapView.getLayer(layerId);
  }

  /**
   * Retrieve all the layers in the style
   *
   * @return all the layers in the current style
   */
  @NonNull
  public List<Layer> getLayers() {
    return nativeMapView.getLayers();
  }

  /**
   * Removes the layer. Any references to the layer become invalid and should not be used anymore
   *
   * @param layerId the layer to remove
   * @return the removed layer or null if not found
   */
  @Nullable
  public Layer removeLayer(@NonNull String layerId) {
    layers.remove(layerId);
    return nativeMapView.removeLayer(layerId);
  }

  /**
   * Removes the layer. The reference is re-usable after this and can be re-added
   *
   * @param layer the layer to remove
   * @return the layer
   */
  @Nullable
  public Layer removeLayer(@NonNull Layer layer) {
    layers.remove(layer.getId());
    return nativeMapView.removeLayer(layer);
  }

  /**
   * Removes the layer. Any other references to the layer become invalid and should not be used anymore
   *
   * @param index the layer index
   * @return the removed layer or null if not found
   */
  @Nullable
  public Layer removeLayerAt(@IntRange(from = 0) int index) {
    // TODO what about runtime added sources?
    return nativeMapView.removeLayerAt(index);
  }

  //
  // Image
  //

  /**
   * Adds an image to be used in the map's style
   *
   * @param name  the name of the image
   * @param image the pre-multiplied Bitmap
   */
  public void addImage(@NonNull String name, @NonNull Bitmap image) {
    addImage(name, image, false);
  }

  /**
   * Adds an image to be used in the map's style
   *
   * @param name  the name of the image
   * @param image the pre-multiplied Bitmap
   * @param sdf   the flag indicating image is an SDF or template image
   */
  public void addImage(@NonNull String name, @NonNull Bitmap image, boolean sdf) {
    nativeMapView.addImage(name, image, sdf);
  }

  /**
   * Adds an images to be used in the map's style
   */
  public void addImages(@NonNull HashMap<String, Bitmap> images) {
    nativeMapView.addImages(images);
  }

  /**
   * Removes an image from the map's style
   *
   * @param name the name of the image to remove
   */
  public void removeImage(@NonNull String name) {
    nativeMapView.removeImage(name);
  }

  @Nullable
  public Bitmap getImage(@NonNull String name) {
    return nativeMapView.getImage(name);
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
  // Light
  //

  /**
   * Get the light source used to change lighting conditions on extruded fill layers.
   *
   * @return the global light source
   */
  @NonNull
  public Light getLight() {
    return nativeMapView.getLight();
  }

  //
  // Builder
  //

  public static class Builder {

    private String styleUrl;
    private String styleJson;
    private List<Source> sources = new ArrayList<>();
    // TODO allow adding below and at index
    private List<Layer> layers = new ArrayList<>();
    private TransitionOptions transitionOptions;


    /**
     * <p>
     * Loads a new map style asynchronous from the specified URL.
     * </p>
     * {@code url} can take the following forms:
     * <ul>
     * <li>{@code Style.*}: load one of the bundled styles in {@link Style}.</li>
     * <li>{@code mapbox://styles/<user>/<style>}:
     * loads the style from a <a href="https://www.mapbox.com/account/">Mapbox account.</a>
     * {@code user} is your username. {@code style} is the ID of your custom
     * style created in <a href="https://www.mapbox.com/studio">Mapbox Studio</a>.</li>
     * <li>{@code http://...} or {@code https://...}:
     * loads the style over the Internet from any web server.</li>
     * <li>{@code asset://...}:
     * loads the style from the APK {@code assets/} directory.
     * This is used to load a style bundled with your app.</li>
     * <li>{@code null}: loads the default {@link Style#MAPBOX_STREETS} style.</li>
     * </ul>
     * <p>
     * This method is asynchronous and will return before the style finishes loading.
     * If you wish to wait for the map to finish loading, listen to the {@link MapView.OnDidFinishLoadingStyleListener}
     * callback.
     * </p>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView.OnDidFailLoadingMapListener} callback
     * will be triggered.
     *
     * @param url The URL of the map style
     * @see Style
     */
    public Builder withStyleUrl(@StyleUrl String url) {
      this.styleUrl = styleUrl;
      return this;
    }

    /**
     * Loads a new map style from a json string.
     * <p>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView.OnDidFailLoadingMapListener} callback
     * will be triggered.
     * </p>
     */
    public Builder withStyleJson(String styleJson) {
      this.styleJson = styleJson;
      return this;
    }

    public Builder withSource(Source source) {
      sources.add(source);
      return this;
    }

    public Builder withLayer(Layer layer) {
      layers.add(layer);
      return this;
    }

    public Builder withTransition(TransitionOptions transition) {
      this.transitionOptions = transition;
      return this;
    }

    Style build(NativeMapView nativeMapView) {
      Style style = new Style(nativeMapView);
      for (Source source : sources) {
        style.addSource(source);
      }

      for (Layer layer : layers) {
        style.addLayer(layer);
      }

      if (transitionOptions != null) {
        style.setTransition(transitionOptions);
      }
      return style;
    }

  }

  //
  // Style URL constants
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
