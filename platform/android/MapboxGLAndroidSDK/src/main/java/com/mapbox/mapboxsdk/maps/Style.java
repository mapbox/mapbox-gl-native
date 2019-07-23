package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.os.AsyncTask;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringDef;
import android.util.DisplayMetrics;
import android.util.Pair;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * The proxy object for current map style.
 * <p>
 * To create new instances of this object, create a new instance using a {@link Builder} and load the style with
 * MapboxMap. This object is returned from {@link MapboxMap#getStyle()} once the style
 * has been loaded by underlying map.
 * </p>
 */
@SuppressWarnings("unchecked")
public class Style {

  private final NativeMap nativeMap;
  private final HashMap<String, Source> sources = new HashMap<>();
  private final HashMap<String, Layer> layers = new HashMap<>();
  private final HashMap<String, Bitmap> images = new HashMap<>();
  private final Builder builder;
  private boolean fullyLoaded;

  /**
   * Private constructor to build a style object.
   *
   * @param builder   the builder used for creating this style
   * @param nativeMap the map object used to load this style
   */
  private Style(@NonNull Builder builder, @NonNull NativeMap nativeMap) {
    this.builder = builder;
    this.nativeMap = nativeMap;
  }

  /**
   * Returns the current style url.
   *
   * @return the style url
   * @deprecated use {@link #getUri()} instead
   */
  @NonNull
  @Deprecated
  public String getUrl() {
    validateState("getUrl");
    return nativeMap.getStyleUri();
  }

  /**
   * Returns the current style uri.
   *
   * @return the style uri
   */
  @NonNull
  public String getUri() {
    validateState("getUri");
    return nativeMap.getStyleUri();
  }

  /**
   * Returns the current style json.
   *
   * @return the style json
   */
  @NonNull
  public String getJson() {
    validateState("getJson");
    return nativeMap.getStyleJson();
  }

  //
  // Source
  //

  /**
   * Retrieve all the sources in the style
   *
   * @return all the sources in the current style
   */
  @NonNull
  public List<Source> getSources() {
    validateState("getSources");
    return nativeMap.getSources();
  }

  /**
   * Adds the source to the map. The source must be newly created and not added to the map before
   *
   * @param source the source to add
   */
  public void addSource(@NonNull Source source) {
    validateState("addSource");
    nativeMap.addSource(source);
    sources.put(source.getId(), source);
  }

  /**
   * Retrieve a source by id
   *
   * @param id the source's id
   * @return the source if present in the current style
   */
  @Nullable
  public Source getSource(String id) {
    validateState("getSource");
    Source source = sources.get(id);
    if (source == null) {
      source = nativeMap.getSource(id);
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
  @Nullable
  public <T extends Source> T getSourceAs(@NonNull String sourceId) {
    validateState("getSourceAs");
    // noinspection unchecked
    if (sources.containsKey(sourceId)) {
      return (T) sources.get(sourceId);
    }
    return (T) nativeMap.getSource(sourceId);
  }

  /**
   * Removes the source from the style.
   *
   * @param sourceId the source to remove
   * @return the source handle or null if the source was not present
   */
  public boolean removeSource(@NonNull String sourceId) {
    validateState("removeSource");
    sources.remove(sourceId);
    return nativeMap.removeSource(sourceId);
  }

  /**
   * Removes the source, preserving the reference for re-use
   *
   * @param source the source to remove
   * @return the source
   */
  public boolean removeSource(@NonNull Source source) {
    validateState("removeSource");
    sources.remove(source.getId());
    return nativeMap.removeSource(source);
  }

  //
  // Layer
  //

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   */
  public void addLayer(@NonNull Layer layer) {
    validateState("addLayer");
    nativeMap.addLayer(layer);
    layers.put(layer.getId(), layer);
  }

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   * @param below the layer id to add this layer before
   */
  public void addLayerBelow(@NonNull Layer layer, @NonNull String below) {
    validateState("addLayerBelow");
    nativeMap.addLayerBelow(layer, below);
    layers.put(layer.getId(), layer);
  }

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   * @param above the layer id to add this layer above
   */
  public void addLayerAbove(@NonNull Layer layer, @NonNull String above) {
    validateState("addLayerAbove");
    nativeMap.addLayerAbove(layer, above);
    layers.put(layer.getId(), layer);
  }

  /**
   * Adds the layer to the map at the specified index. The layer must be newly
   * created and not added to the map before
   *
   * @param layer the layer to add
   * @param index the index to insert the layer at
   */
  public void addLayerAt(@NonNull Layer layer, @IntRange(from = 0) int index) {
    validateState("addLayerAbove");
    nativeMap.addLayerAt(layer, index);
    layers.put(layer.getId(), layer);
  }

  /**
   * Get the layer by id
   *
   * @param id the layer's id
   * @return the layer, if present in the style
   */
  @Nullable
  public Layer getLayer(@NonNull String id) {
    validateState("getLayer");
    Layer layer = layers.get(id);
    if (layer == null) {
      layer = nativeMap.getLayer(id);
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
  @Nullable
  public <T extends Layer> T getLayerAs(@NonNull String layerId) {
    validateState("getLayerAs");
    // noinspection unchecked
    return (T) nativeMap.getLayer(layerId);
  }

  /**
   * Retrieve all the layers in the style
   *
   * @return all the layers in the current style
   */
  @NonNull
  public List<Layer> getLayers() {
    validateState("getLayers");
    return nativeMap.getLayers();
  }

  /**
   * Removes the layer. Any references to the layer become invalid and should not be used anymore
   *
   * @param layerId the layer to remove
   * @return the removed layer or null if not found
   */
  public boolean removeLayer(@NonNull String layerId) {
    validateState("removeLayer");
    layers.remove(layerId);
    return nativeMap.removeLayer(layerId);
  }

  /**
   * Removes the layer. The reference is re-usable after this and can be re-added
   *
   * @param layer the layer to remove
   * @return the layer
   */
  public boolean removeLayer(@NonNull Layer layer) {
    validateState("removeLayer");
    layers.remove(layer.getId());
    return nativeMap.removeLayer(layer);
  }

  /**
   * Removes the layer. Any other references to the layer become invalid and should not be used anymore
   *
   * @param index the layer index
   * @return the removed layer or null if not found
   */
  public boolean removeLayerAt(@IntRange(from = 0) int index) {
    validateState("removeLayerAt");
    return nativeMap.removeLayerAt(index);
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
   * Adds an drawable to be converted into a bitmap to be used in the map's style
   *
   * @param name     the name of the image
   * @param drawable the drawable instance to convert
   */
  public void addImage(@NonNull String name, @NonNull Drawable drawable) {
    Bitmap bitmap = BitmapUtils.getBitmapFromDrawable(drawable);
    if (bitmap == null) {
      throw new IllegalArgumentException("Provided drawable couldn't be converted to a Bitmap.");
    }
    addImage(name, bitmap, false);
  }

  /**
   * Adds an image to be used in the map's style
   *
   * @param name   the name of the image
   * @param bitmap the pre-multiplied Bitmap
   * @param sdf    the flag indicating image is an SDF or template image
   */
  public void addImage(@NonNull final String name, @NonNull Bitmap bitmap, boolean sdf) {
    validateState("addImage");
    nativeMap.addImages(new Image[]{toImage(new Builder.ImageWrapper(name, bitmap, sdf))});
  }

  /**
   * Adds an image asynchronously, to be used in the map's style.
   *
   * @param name  the name of the image
   * @param image the pre-multiplied Bitmap
   */
  public void addImageAsync(@NonNull String name, @NonNull Bitmap image) {
    addImageAsync(name, image, false);
  }

  /**
   * Adds an drawable asynchronously, to be converted into a bitmap to be used in the map's style.
   *
   * @param name     the name of the image
   * @param drawable the drawable instance to convert
   */
  public void addImageAsync(@NonNull String name, @NonNull Drawable drawable) {
    Bitmap bitmap = BitmapUtils.getBitmapFromDrawable(drawable);
    if (bitmap == null) {
      throw new IllegalArgumentException("Provided drawable couldn't be converted to a Bitmap.");
    }
    addImageAsync(name, bitmap, false);
  }

  /**
   * Adds an image asynchronously, to be used in the map's style.
   *
   * @param name   the name of the image
   * @param bitmap the pre-multiplied Bitmap
   * @param sdf    the flag indicating image is an SDF or template image
   */
  public void addImageAsync(@NonNull final String name, @NonNull Bitmap bitmap, boolean sdf) {
    validateState("addImage");
    new BitmapImageConversionTask(nativeMap).execute(new Builder.ImageWrapper(name, bitmap, sdf));
  }

  /**
   * Adds images to be used in the map's style.
   *
   * @param images the map of images to add
   */
  public void addImages(@NonNull HashMap<String, Bitmap> images) {
    addImages(images, false);
  }

  /**
   * Adds images to be used in the map's style.
   *
   * @param images the map of images to add
   * @param sdf    the flag indicating image is an SDF or template image
   */
  public void addImages(@NonNull HashMap<String, Bitmap> images, boolean sdf) {
    validateState("addImage");
    Image[] convertedImages = new Image[images.size()];
    int index = 0;
    for (Builder.ImageWrapper imageWrapper : Builder.ImageWrapper.convertToImageArray(images, sdf)) {
      convertedImages[index] = toImage(imageWrapper);
      index++;
    }

    nativeMap.addImages(convertedImages);
  }

  /**
   * Adds images asynchronously, to be used in the map's style.
   *
   * @param images the map of images to add
   */
  public void addImagesAsync(@NonNull HashMap<String, Bitmap> images) {
    addImagesAsync(images, false);
  }

  /**
   * Adds images asynchronously, to be used in the map's style.
   *
   * @param images the map of images to add
   * @param sdf    the flag indicating image is an SDF or template image
   */
  public void addImagesAsync(@NonNull HashMap<String, Bitmap> images, boolean sdf) {
    validateState("addImages");
    new BitmapImageConversionTask(nativeMap).execute(Builder.ImageWrapper.convertToImageArray(images, sdf));
  }

  /**
   * Removes an image from the map's style.
   *
   * @param name the name of the image to remove
   */
  public void removeImage(@NonNull String name) {
    validateState("removeImage");
    nativeMap.removeImage(name);
  }

  /**
   * Get an image from the map's style using an id.
   *
   * @param id the id of the image
   * @return the image bitmap
   */
  @Nullable
  public Bitmap getImage(@NonNull String id) {
    validateState("getImage");
    return nativeMap.getImage(id);
  }

  //
  // Transition
  //

  /**
   * <p>
   * Set the transition options for style changes.
   * </p>
   * If not set, any changes take effect without animation, besides symbols,
   * which will fade in/out with a default duration after symbol collision detection.
   * <p>
   * To disable symbols fade in/out animation,
   * pass transition options with {@link TransitionOptions#enablePlacementTransitions} equal to false.
   * <p>
   * Both {@link TransitionOptions#duration} and {@link TransitionOptions#delay}
   * will also change the behavior of the symbols fade in/out animation if the placement transition is enabled.
   *
   * @param transitionOptions the transition options
   */
  public void setTransition(@NonNull TransitionOptions transitionOptions) {
    validateState("setTransition");
    nativeMap.setTransitionOptions(transitionOptions);
  }

  /**
   * <p>
   * Get the transition options for style changes.
   * </p>
   * By default, any changes take effect without animation, besides symbols,
   * which will fade in/out with a default duration after symbol collision detection.
   * <p>
   * To disable symbols fade in/out animation,
   * pass transition options with {@link TransitionOptions#enablePlacementTransitions} equal to false
   * into {@link #setTransition(TransitionOptions)}.
   * <p>
   * Both {@link TransitionOptions#duration} and {@link TransitionOptions#delay}
   * will also change the behavior of the symbols fade in/out animation if the placement transition is enabled.
   *
   * @return TransitionOptions the transition options
   */
  @NonNull
  public TransitionOptions getTransition() {
    validateState("getTransition");
    return nativeMap.getTransitionOptions();
  }

  //
  // Light
  //

  /**
   * Get the light source used to change lighting conditions on extruded fill layers.
   *
   * @return the global light source
   */
  @Nullable
  public Light getLight() {
    validateState("getLight");
    return nativeMap.getLight();
  }

  //
  // State
  //

  /**
   * Called when the underlying map will start loading a new style or the map is destroyed.
   * This method will clean up this style by setting the java sources and layers
   * in a detached state and removing them from core.
   */
  void clear() {
    fullyLoaded = false;
    for (Layer layer : layers.values()) {
      if (layer != null) {
        layer.setDetached();
        nativeMap.removeLayer(layer);
      }
    }

    for (Source source : sources.values()) {
      if (source != null) {
        source.setDetached();
        nativeMap.removeSource(source);
      }
    }

    for (Map.Entry<String, Bitmap> bitmapEntry : images.entrySet()) {
      nativeMap.removeImage(bitmapEntry.getKey());
      bitmapEntry.getValue().recycle();
    }

    sources.clear();
    layers.clear();
    images.clear();
  }

  /**
   * Called when the underlying map has finished loading this style.
   * This method will add all components added to the builder that were defined with the 'with' prefix.
   */
  void onDidFinishLoadingStyle() {
    if (!fullyLoaded) {
      fullyLoaded = true;
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
    }
  }

  /**
   * Returns true if the style is fully loaded. Returns false if style hasn't been fully loaded or a new style is
   * underway of being loaded.
   *
   * @return True if fully loaded, false otherwise
   */
  public boolean isFullyLoaded() {
    return fullyLoaded;
  }

  /**
   * Validates the style state, throw an IllegalArgumentException on invalid state.
   *
   * @param methodCall the calling method name
   */
  private void validateState(String methodCall) {
    if (!fullyLoaded) {
      throw new IllegalStateException(
        String.format("Calling %s when a newer style is loading/has loaded.", methodCall)
      );
    }
  }

  //
  // Builder
  //

  /**
   * Builder for composing a style object.
   */
  public static class Builder {

    private final List<Source> sources = new ArrayList<>();
    private final List<LayerWrapper> layers = new ArrayList<>();
    private final List<ImageWrapper> images = new ArrayList<>();

    private TransitionOptions transitionOptions;
    private String styleUri;
    private String styleJson;

    /**
     * <p>
     * Will loads a new map style asynchronous from the specified URL.
     * </p>
     * {@code url} can take the following forms:
     * <ul>
     * <li>{@code Style#StyleUrl}: load one of the bundled styles in {@link Style}.</li>
     * <li>{@code mapbox://styles/<user>/<style>}:
     * loads the style from a <a href="https://www.mapbox.com/account/">Mapbox account.</a>
     * {@code user} is your username. {@code style} is the ID of your custom
     * style created in <a href="https://www.mapbox.com/studio">Mapbox Studio</a>.</li>
     * <li>{@code http://...} or {@code https://...}:
     * loads the style over the Internet from any web server.</li>
     * <li>{@code asset://...}:
     * loads the style from the APK {@code assets/} directory.
     * This is used to load a style bundled with your app.</li>
     * <li>{@code file://...}:
     * loads the style from a file path. This is used to load a style from disk.
     * </li>
     * </li>
     * <li>{@code null}: loads the default {@link Style#MAPBOX_STREETS} style.</li>
     * </ul>
     * <p>
     * This method is asynchronous and will return before the style finishes loading.
     * If you wish to wait for the map to finish loading, listen to the {@link MapView.OnDidFinishLoadingStyleListener}
     * callback or provide an {@link OnStyleLoaded} callback when setting the style on MapboxMap.
     * </p>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView.OnDidFailLoadingMapListener} callback
     * will be triggered.
     *
     * @param url The URL of the map style
     * @return this
     * @see Style
     * @deprecated use {@link #fromUri(String)} instead
     */
    @Deprecated
    @NonNull
    public Builder fromUrl(@NonNull String url) {
      this.styleUri = url;
      return this;
    }

    /**
     * <p>
     * Will loads a new map style asynchronous from the specified URI.
     * </p>
     * {@code uri} can take the following forms:
     * <ul>
     * <li>{@code Style#StyleUrl}: load one of the bundled styles in {@link Style}.</li>
     * <li>{@code mapbox://styles/<user>/<style>}:
     * loads the style from a <a href="https://www.mapbox.com/account/">Mapbox account.</a>
     * {@code user} is your username. {@code style} is the ID of your custom
     * style created in <a href="https://www.mapbox.com/studio">Mapbox Studio</a>.</li>
     * <li>{@code http://...} or {@code https://...}:
     * loads the style over the Internet from any web server.</li>
     * <li>{@code asset://...}:
     * loads the style from the APK {@code assets/} directory.
     * This is used to load a style bundled with your app.</li>
     * <li>{@code file://...}:
     * loads the style from a file path. This is used to load a style from disk.
     * </li>
     * </li>
     * <li>{@code null}: loads the default {@link Style#MAPBOX_STREETS} style.</li>
     * </ul>
     * <p>
     * This method is asynchronous and will return before the style finishes loading.
     * If you wish to wait for the map to finish loading, listen to the {@link MapView.OnDidFinishLoadingStyleListener}
     * callback or use {@link MapboxMap#setStyle(String, OnStyleLoaded)} instead.
     * </p>
     * If the style fails to load or an invalid style URI is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView.OnDidFailLoadingMapListener} callback
     * will be triggered.
     *
     * @param uri The URI of the map style
     * @return this
     * @see Style
     */
    @NonNull
    public Builder fromUri(@NonNull String uri) {
      this.styleUri = uri;
      return this;
    }

    /**
     * Will load a new map style from a json string.
     * <p>
     * If the style fails to load or an invalid style URI is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView.OnDidFailLoadingMapListener} callback
     * will be triggered.
     * </p>
     *
     * @return this
     */
    @NonNull
    public Builder fromJson(@NonNull String styleJson) {
      this.styleJson = styleJson;
      return this;
    }

    /**
     * Will add the source when map style has loaded.
     *
     * @param source the source to add
     * @return this
     */
    @NonNull
    public Builder withSource(@NonNull Source source) {
      sources.add(source);
      return this;
    }

    /**
     * Will add the sources when map style has loaded.
     *
     * @param sources the sources to add
     * @return this
     */
    @NonNull
    public Builder withSources(@NonNull Source... sources) {
      this.sources.addAll(Arrays.asList(sources));
      return this;
    }

    /**
     * Will add the layer when the style has loaded.
     *
     * @param layer the layer to be added
     * @return this
     */
    @NonNull
    public Builder withLayer(@NonNull Layer layer) {
      layers.add(new LayerWrapper(layer));
      return this;
    }

    /**
     * Will add the layers when the style has loaded.
     *
     * @param layers the layers to be added
     * @return this
     */
    @NonNull
    public Builder withLayers(@NonNull Layer... layers) {
      for (Layer layer : layers) {
        this.layers.add(new LayerWrapper(layer));
      }
      return this;
    }

    /**
     * Will add the layer when the style has loaded at a specified index.
     *
     * @param layer the layer to be added
     * @return this
     */
    @NonNull
    public Builder withLayerAt(@NonNull Layer layer, int index) {
      layers.add(new LayerAtWrapper(layer, index));
      return this;
    }

    /**
     * Will add the layer when the style has loaded above a specified layer id.
     *
     * @param layer the layer to be added
     * @return this
     */
    @NonNull
    public Builder withLayerAbove(@NonNull Layer layer, @NonNull String aboveLayerId) {
      layers.add(new LayerAboveWrapper(layer, aboveLayerId));
      return this;
    }

    /**
     * Will add the layer when the style has loaded below a specified layer id.
     *
     * @param layer the layer to be added
     * @return this
     */
    @NonNull
    public Builder withLayerBelow(@NonNull Layer layer, @NonNull String belowLayerId) {
      layers.add(new LayerBelowWrapper(layer, belowLayerId));
      return this;
    }

    /**
     * Will add the transition when the map style has loaded.
     *
     * @param transition the transition to be added
     * @return this
     */
    @NonNull
    public Builder withTransition(@NonNull TransitionOptions transition) {
      this.transitionOptions = transition;
      return this;
    }

    /**
     * Will add the drawable as image when the map style has loaded.
     *
     * @param id       the id for the image
     * @param drawable the drawable to be converted and added
     * @return this
     */
    @NonNull
    public Builder withImage(@NonNull String id, @NonNull Drawable drawable) {
      Bitmap bitmap = BitmapUtils.getBitmapFromDrawable(drawable);
      if (bitmap == null) {
        throw new IllegalArgumentException("Provided drawable couldn't be converted to a Bitmap.");
      }
      return this.withImage(id, bitmap, false);
    }

    /**
     * Will add the drawables as images when the map style has loaded.
     *
     * @param values pairs, where first is the id for te image and second is the drawable
     * @return this
     */
    @NonNull
    public Builder withDrawableImages(@NonNull Pair<String, Drawable>... values) {
      return this.withDrawableImages(false, values);
    }

    /**
     * Will add the image when the map style has loaded.
     *
     * @param id    the id for the image
     * @param image the image to be added
     * @return this
     */
    @NonNull
    public Builder withImage(@NonNull String id, @NonNull Bitmap image) {
      return this.withImage(id, image, false);
    }

    /**
     * Will add the images when the map style has loaded.
     *
     * @param values pairs, where first is the id for te image and second is the bitmap
     * @return this
     */
    @NonNull
    public Builder withBitmapImages(@NonNull Pair<String, Bitmap>... values) {
      for (Pair<String, Bitmap> value : values) {
        this.withImage(value.first, value.second, false);
      }
      return this;
    }

    /**
     * Will add the drawable as image when the map style has loaded.
     *
     * @param id       the id for the image
     * @param drawable the drawable to be converted and added
     * @param sdf      the flag indicating image is an SDF or template image
     * @return this
     */
    @NonNull
    public Builder withImage(@NonNull String id, @NonNull Drawable drawable, boolean sdf) {
      Bitmap bitmap = BitmapUtils.getBitmapFromDrawable(drawable);
      if (bitmap == null) {
        throw new IllegalArgumentException("Provided drawable couldn't be converted to a Bitmap.");
      }
      return this.withImage(id, bitmap, sdf);
    }

    /**
     * Will add the drawables as images when the map style has loaded.
     *
     * @param sdf    the flag indicating image is an SDF or template image
     * @param values pairs, where first is the id for te image and second is the drawable
     * @return this
     */
    @NonNull
    public Builder withDrawableImages(boolean sdf, @NonNull Pair<String, Drawable>... values) {
      for (Pair<String, Drawable> value : values) {
        Bitmap bitmap = BitmapUtils.getBitmapFromDrawable(value.second);
        if (bitmap == null) {
          throw new IllegalArgumentException("Provided drawable couldn't be converted to a Bitmap.");
        }
        this.withImage(value.first, bitmap, sdf);
      }
      return this;
    }

    /**
     * Will add the image when the map style has loaded.
     *
     * @param id    the id for the image
     * @param image the image to be added
     * @param sdf   the flag indicating image is an SDF or template image
     * @return this
     */
    @NonNull
    public Builder withImage(@NonNull String id, @NonNull Bitmap image, boolean sdf) {
      images.add(new ImageWrapper(id, image, sdf));
      return this;
    }

    /**
     * Will add the images when the map style has loaded.
     *
     * @param sdf    the flag indicating image is an SDF or template image
     * @param values pairs, where first is the id for te image and second is the bitmap
     * @return this
     */
    @NonNull
    public Builder withBitmapImages(boolean sdf, @NonNull Pair<String, Bitmap>... values) {
      for (Pair<String, Bitmap> value : values) {
        this.withImage(value.first, value.second, sdf);
      }
      return this;
    }

    String getUri() {
      return styleUri;
    }

    String getJson() {
      return styleJson;
    }

    List<Source> getSources() {
      return sources;
    }

    List<LayerWrapper> getLayers() {
      return layers;
    }

    List<ImageWrapper> getImages() {
      return images;
    }

    TransitionOptions getTransitionOptions() {
      return transitionOptions;
    }

    /**
     * Build the composed style.
     */
    Style build(@NonNull NativeMap nativeMap) {
      return new Style(this, nativeMap);
    }

    static class ImageWrapper {
      Bitmap bitmap;
      String id;
      boolean sdf;

      ImageWrapper(String id, Bitmap bitmap, boolean sdf) {
        this.id = id;
        this.bitmap = bitmap;
        this.sdf = sdf;
      }

      static ImageWrapper[] convertToImageArray(HashMap<String, Bitmap> bitmapHashMap, boolean sdf) {
        ImageWrapper[] images = new ImageWrapper[bitmapHashMap.size()];
        List<String> keyList = new ArrayList<>(bitmapHashMap.keySet());
        for (int i = 0; i < bitmapHashMap.size(); i++) {
          String id = keyList.get(i);
          images[i] = new ImageWrapper(id, bitmapHashMap.get(id), sdf);
        }
        return images;
      }
    }

    class LayerWrapper {
      Layer layer;

      LayerWrapper(Layer layer) {
        this.layer = layer;
      }
    }

    class LayerAboveWrapper extends LayerWrapper {
      String aboveLayer;

      LayerAboveWrapper(Layer layer, String aboveLayer) {
        super(layer);
        this.aboveLayer = aboveLayer;
      }
    }

    class LayerBelowWrapper extends LayerWrapper {
      String belowLayer;

      LayerBelowWrapper(Layer layer, String belowLayer) {
        super(layer);
        this.belowLayer = belowLayer;
      }
    }

    class LayerAtWrapper extends LayerWrapper {
      int index;

      LayerAtWrapper(Layer layer, int index) {
        super(layer);
        this.index = index;
      }
    }
  }

  private static Image toImage(Builder.ImageWrapper imageWrapper) {
    Bitmap bitmap = imageWrapper.bitmap;
    if (bitmap.getConfig() != Bitmap.Config.ARGB_8888) {
      bitmap = bitmap.copy(Bitmap.Config.ARGB_8888, false);
    }

    ByteBuffer buffer = ByteBuffer.allocate(bitmap.getByteCount());
    bitmap.copyPixelsToBuffer(buffer);
    float pixelRatio = (float) bitmap.getDensity() / DisplayMetrics.DENSITY_DEFAULT;

    return new Image(buffer.array(), pixelRatio, imageWrapper.id,
      bitmap.getWidth(), bitmap.getHeight(), imageWrapper.sdf
    );
  }

  private static class BitmapImageConversionTask extends AsyncTask<Builder.ImageWrapper, Void, Image[]> {

    private WeakReference<NativeMap> nativeMap;

    BitmapImageConversionTask(NativeMap nativeMap) {
      this.nativeMap = new WeakReference<>(nativeMap);
    }

    @NonNull
    @Override
    protected Image[] doInBackground(Builder.ImageWrapper... params) {
      List<Image> images = new ArrayList<>();
      for (Builder.ImageWrapper param : params) {
        images.add(toImage(param));
      }
      return images.toArray(new Image[images.size()]);
    }

    @Override
    protected void onPostExecute(@NonNull Image[] images) {
      super.onPostExecute(images);
      NativeMap nativeMap = this.nativeMap.get();
      if (nativeMap != null && !nativeMap.isDestroyed()) {
        nativeMap.addImages(images);
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
    void onStyleLoaded(@NonNull Style style);
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
  public static final String MAPBOX_STREETS = "mapbox://styles/mapbox/streets-v11";

  /**
   * Outdoors: A general-purpose style tailored to outdoor activities. Using this constant means
   * your map style will always use the latest version and may change as we improve the style.
   */
  public static final String OUTDOORS = "mapbox://styles/mapbox/outdoors-v11";

  /**
   * Light: Subtle light backdrop for data visualizations. Using this constant means your map
   * style will always use the latest version and may change as we improve the style.
   */
  public static final String LIGHT = "mapbox://styles/mapbox/light-v10";

  /**
   * Dark: Subtle dark backdrop for data visualizations. Using this constant means your map style
   * will always use the latest version and may change as we improve the style.
   */
  public static final String DARK = "mapbox://styles/mapbox/dark-v10";

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
  public static final String SATELLITE_STREETS = "mapbox://styles/mapbox/satellite-streets-v11";

  /**
   * Traffic Day: Color-coded roads based on live traffic congestion data. Traffic data is currently
   * available in
   * <a href="https://www.mapbox.com/help/how-directions-work/#traffic-data">these select
   * countries</a>. Using this constant means your map style will always use the latest version and
   * may change as we improve the style.
   */
  public static final String TRAFFIC_DAY = "mapbox://styles/mapbox/traffic-day-v2";

  /**
   * Traffic Night: Color-coded roads based on live traffic congestion data, designed to maximize
   * legibility in low-light situations. Traffic data is currently available in
   * <a href="https://www.mapbox.com/help/how-directions-work/#traffic-data">these select
   * countries</a>. Using this constant means your map style will always use the latest version and
   * may change as we improve the style.
   */
  public static final String TRAFFIC_NIGHT = "mapbox://styles/mapbox/traffic-night-v2";
}
