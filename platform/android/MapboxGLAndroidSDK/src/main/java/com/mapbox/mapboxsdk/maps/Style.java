package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringDef;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * TODO
 */
public class Style {

  private final NativeMapView nativeMapView;
  private final HashMap<String, Source> sources = new HashMap<>();
  private final HashMap<String, Layer> layers = new HashMap<>();
  private final OnStyleLoaded onStyleLoaded;
  private final Builder builder;
  private boolean styleLoaded;

  /**
   * Private constructor to build a style object.
   *
   * @param builder the builder used for creating this style
   * @param nativeMapView the map object used to load this style
   * @param styleLoaded the callback used to notify about finish style loading
   */
  private Style(Builder builder, NativeMapView nativeMapView, OnStyleLoaded styleLoaded) {
    this.builder = builder;
    this.nativeMapView = nativeMapView;
    this.onStyleLoaded = styleLoaded;
  }

  /**
   * Returns the current style url.
   *
   * @return the style url
   */
  @NonNull
  public String getUrl() {
    return nativeMapView.getStyleUrl();
  }

  /**
   * Returns the current style json.
   *
   * @return the style json
   */
  @NonNull
  public String getJson() {
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
   * Adds an images to be used in the map's style.
   */
  public void addImages(@NonNull HashMap<String, Bitmap> images) {
    nativeMapView.addImages(images);
  }

  /**
   * Removes an image from the map's style.
   *
   * @param name the name of the image to remove
   */
  public void removeImage(@NonNull String name) {
    nativeMapView.removeImage(name);
  }

  /**
   * Get an image from the map's style using an id.
   *
   * @param id the id of the image
   * @return the image bitmap
   */
  @Nullable
  public Bitmap getImage(@NonNull String id) {
    return nativeMapView.getImage(id);
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

  /**
   * Called when the underlying map will start loading a new style. This method will clean up this style
   * by setting the java sources and layers in a detached state and removing them from core.
   */
  public void onWillStartLoadingStyle() {
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

    sources.clear();
    layers.clear();
  }

  /**
   * Called when the underlying map has finished loading this style.
   * This method will add all components added to the builder that were defined with the 'with' prefix.
   */
  public void onDidFinishLoadingStyle() {
    if (!styleLoaded) {
      styleLoaded = true;
      for (Source source : builder.sources) {
        addSource(source);
      }

      for (Builder.LayerWrapper layerWrapper : builder.layers) {
        if (layerWrapper instanceof Builder.LayerAtWrapper) {
          addLayerAt(layerWrapper.layer, ((Builder.LayerAtWrapper) layerWrapper).index);
        } else if (layerWrapper instanceof Builder.LayerAboveWrapper) {
          addLayerAbove(layerWrapper.layer, ((Builder.LayerAboveWrapper) layerWrapper).aboveLayer);
        } else if (layerWrapper instanceof Builder.LayerBelowWrapper) {
          addLayerBelow(layerWrapper.layer, ((Builder.LayerBelowWrapper) layerWrapper).belowLayer);
        } else {
          // just add layer to map, but below annotations
          addLayerBelow(layerWrapper.layer, MapboxConstants.LAYER_ID_ANNOTATIONS);
        }
      }

      for (Builder.ImageWrapper image : builder.images) {
        addImage(image.id, image.bitmap, image.sdf);
      }

      if (builder.transitionOptions != null) {
        setTransition(builder.transitionOptions);
      }

      if (onStyleLoaded != null) {
        onStyleLoaded.onStyleLoaded(this);
      }
    }
  }

  //
  // Builder
  //

  /**
   * TODO
   */
  public static class Builder {

    private final List<Source> sources = new ArrayList<>();
    private final List<LayerWrapper> layers = new ArrayList<>();
    private final List<ImageWrapper> images = new ArrayList<>();

    private TransitionOptions transitionOptions;
    private String styleUrl;
    private String styleJson;

    /**
     * <p>
     * Will loads a new map style asynchronous from the specified URL.
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
     * @return this
     * @see Style
     */
    public Builder fromUrl(String url) {
      this.styleUrl = url;
      return this;
    }

    /**
     * Will load a new map style from a json string.
     * <p>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView.OnDidFailLoadingMapListener} callback
     * will be triggered.
     * </p>
     *
     * @return this
     */
    public Builder fromJson(String styleJson) {
      this.styleJson = styleJson;
      return this;
    }

    /**
     * Will add the source when map style has loaded.
     *
     * @param source the source to add
     * @return this
     */
    public Builder withSource(Source source) {
      sources.add(source);
      return this;
    }

    /**
     * Will add the layer when the style has loaded.
     *
     * @param layer the layer to be added
     * @return this
     */
    public Builder withLayer(Layer layer) {
      layers.add(new LayerWrapper(layer));
      return this;
    }

    /**
     * Will add the layer when the style has loaded at a specified index.
     *
     * @param layer the layer to be added
     * @return this
     */
    public Builder withLayerAt(Layer layer, int index) {
      layers.add(new LayerAtWrapper(layer, index));
      return this;
    }

    /**
     * Will add the layer when the style has loaded above a specified layer id.
     *
     * @param layer the layer to be added
     * @return this
     */
    public Builder withLayerAbove(Layer layer, String aboveLayerId) {
      layers.add(new LayerAboveWrapper(layer, aboveLayerId));
      return this;
    }

    /**
     * Will add the layer when the style has loaded below a specified layer id.
     *
     * @param layer the layer to be added
     * @return this
     */
    public Builder withLayerBelow(Layer layer, String belowLayerId) {
      layers.add(new LayerBelowWrapper(layer, belowLayerId));
      return this;
    }

    /**
     * Will add the transition when the map style has loaded.
     *
     * @param transition the transition to be added
     * @return this
     */
    public Builder withTransition(TransitionOptions transition) {
      this.transitionOptions = transition;
      return this;
    }

    /**
     * Will add the image when the map style has loaded.
     *
     * @param id    the id for the image
     * @param image the image to be added
     * @return this
     */
    public Builder withImage(String id, Bitmap image) {
      return this.withImage(id, image, false);
    }

    /**
     * Will add the image when the map style has loaded.
     *
     * @param id    the id for the image
     * @param image the image to be added
     * @return this
     */
    public Builder withImage(String id, Bitmap image, boolean sdf) {
      images.add(new ImageWrapper(id, image, sdf));
      return this;
    }

    /**
     * Build the composed style.
     *
     * @param nativeMapView the native map used for style loading
     * @param styleLoaded   the callback to be invoked when the style has loaded
     */
    void build(NativeMapView nativeMapView, OnStyleLoaded styleLoaded) {
      Style style = new Style(this, nativeMapView, styleLoaded);
      nativeMapView.setStyle(style);

      if (styleUrl != null) {
        nativeMapView.setStyleUrl(styleUrl);
      } else if (styleJson != null) {
        nativeMapView.setStyleJson(styleJson);
      } else {
        // user didn't provide a `from` component,
        // flag the style as loaded,
        // add components defined added using the `with` prefix.
        style.onDidFinishLoadingStyle();
      }
    }

    private class ImageWrapper {
      private Bitmap bitmap;
      private String id;
      private boolean sdf;

      ImageWrapper(String id, Bitmap bitmap, boolean sdf) {
        this.id = id;
        this.bitmap = bitmap;
        this.sdf = sdf;
      }

      public Bitmap getBitmap() {
        return bitmap;
      }

      public String getId() {
        return id;
      }

      public boolean isSdf() {
        return sdf;
      }
    }

    private class LayerWrapper {
      private Layer layer;

      LayerWrapper(Layer layer) {
        this.layer = layer;
      }
    }

    private class LayerAboveWrapper extends LayerWrapper {

      private String aboveLayer;

      LayerAboveWrapper(Layer layer, String aboveLayer) {
        super(layer);
        this.aboveLayer = aboveLayer;
      }
    }

    private class LayerBelowWrapper extends LayerWrapper {

      private String belowLayer;

      LayerBelowWrapper(Layer layer, String belowLayer) {
        super(layer);
        this.belowLayer = belowLayer;
      }
    }

    private class LayerAtWrapper extends LayerWrapper {

      private int index;

      LayerAtWrapper(Layer layer, int index) {
        super(layer);
        this.index = index;
      }
    }
  }

  /**
   * Callback to be invoked when a style has finished loading.
   */
  public interface OnStyleLoaded {
    /**
     * Invoked when a style has finished loading.
     *
     * @param style the style that has finished loading
     */
    void onStyleLoaded(Style style);
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
