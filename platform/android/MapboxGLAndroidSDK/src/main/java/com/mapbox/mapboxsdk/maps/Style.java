package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringDef;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.HashMap;
import java.util.List;

import timber.log.Timber;

public class Style {

  private long nativePtr;

  // todo remove NativeMapView facade
  private NativeMapView nativeMapView;

  private String url;
  private String json;

  // todo make private
  Style() {
    initialize();
  }

  // todo remove
  public void bind(NativeMapView nativeMapView) {
    this.nativeMapView = nativeMapView;
  }

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
   * <li>{@code null}: loads the default
   * {@link com.mapbox.mapboxsdk.constants.StyleConstants#MAPBOX_STREETS} style.</li>
   * </ul>
   * <p>
   * This method is asynchronous and will return before the style finishes loading.
   * If you wish to wait for the map to finish loading, listen for the {@link MapView#DID_FINISH_LOADING_MAP} event
   * or use the {@link MapboxMap#setStyle(Style, MapboxMap.OnStyleLoadedListener)} method instead.
   * </p>
   * If the style fails to load or an invalid style URL is set, the map view will become blank.
   * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
   * emitted.
   *
   * @param url The URL of the map style
   * @see Style
   */
  public static Style fromUrl(String url) {
    Style style = new Style();
    style.setUrl(url);
    return style;
  }

  /**
   * Loads a new map style from a json string.
   * <p>
   * If the style fails to load or an invalid style URL is set, the map view will become blank.
   * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
   * sent.
   * </p>
   */
  public static Style fromJson(String json) {
    Style style = new Style();
    style.setJson(json);
    return style;
  }

  private void setJson(String json) {
    this.json = json;
  }

  private void setUrl(String url) {
    this.url = url;
  }

  Style load() {
    if (url != null) {
      nativeMapView.setStyleUrl(url);
    } else if (json != null) {
      nativeMapView.setStyleJson(json);
    } else {
      throw new IllegalStateException();
    }
    return this;
  }

  //todo implement
  public String getName() {
    return "";
  }

  // Todo implement
  public CameraPosition getDefaultCameraPosition() {
    return null;
  }

  /**
   * <p>
   * Get the animation duration for style changes.
   * </p>
   * The default value is zero, so any changes take effect without animation.
   *
   * @return Duration in milliseconds
   */
  public long getTransitionDuration() {
    return nativeMapView.getTransitionDuration();
  }

  /**
   * Set the animation duration for style changes.
   *
   * @param durationMs Duration in milliseconds
   */
  public void setTransitionDuration(long durationMs) {
    nativeMapView.setTransitionDuration(durationMs);
  }

  /**
   * <p>
   * Get the animation delay for style changes.
   * </p>
   * The default value is zero, so any changes begin to animate immediately.
   *
   * @return Delay in milliseconds
   */
  public long getTransitionDelay() {
    return nativeMapView.getTransitionDelay();
  }

  /**
   * Set the animation delay for style changes.
   *
   * @param delayMs Delay in milliseconds
   */
  public void setTransitionDelay(long delayMs) {
    nativeMapView.setTransitionDelay(delayMs);
  }

  /**
   * Get the global light source used to change lighting conditions on extruded fill layers.
   *
   * @return the global light source
   */
  @Nullable
  public Light getLight() {
    return nativeMapView.getLight();
  }

  /**
   * Retrieve all the layers in the style
   *
   * @return all the layers in the current style
   */
  public List<Layer> getLayers() {
    return nativeMapView.getLayers();
  }

  /**
   * Get the layer by id
   *
   * @param layerId the layer's id
   * @return the layer, if present in the style
   */
  @Nullable
  public Layer getLayer(@NonNull String layerId) {
    return nativeMapView.getLayer(layerId);
  }

  /**
   * Tries to cast the Layer to T, returns null if it's another type.
   *
   * @param layerId the layer id used to look up a layer
   * @param <T>     the generic attribute of a Layer
   * @return the casted Layer, null if another type
   */
  @Nullable
  public <T extends Layer> T getLayerAs(@NonNull String layerId) {
    try {
      // noinspection unchecked
      return (T) nativeMapView.getLayer(layerId);
    } catch (ClassCastException exception) {
      Timber.e(exception, "Layer: %s is a different type: ", layerId);
      return null;
    }
  }

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   */
  public void addLayer(@NonNull Layer layer) {
    nativeMapView.addLayer(layer);
  }

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   * @param below the layer id to add this layer before
   */
  public void addLayerBelow(@NonNull Layer layer, @NonNull String below) {
    nativeMapView.addLayerBelow(layer, below);
  }

  /**
   * Adds the layer to the map. The layer must be newly created and not added to the map before
   *
   * @param layer the layer to add
   * @param above the layer id to add this layer above
   */
  public void addLayerAbove(@NonNull Layer layer, @NonNull String above) {
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
    nativeMapView.addLayerAt(layer, index);
  }

  /**
   * Removes the layer. Any references to the layer become invalid and should not be used anymore
   *
   * @param layerId the layer to remove
   * @return the removed layer or null if not found
   */
  @Nullable
  public Layer removeLayer(@NonNull String layerId) {
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
    return nativeMapView.removeLayerAt(index);
  }

  /**
   * Retrieve all the sources in the style
   *
   * @return all the sources in the current style
   */
  public List<Source> getSources() {
    return nativeMapView.getSources();
  }

  /**
   * Retrieve a source by id
   *
   * @param sourceId the source's id
   * @return the source if present in the current style
   */
  @Nullable
  public Source getSource(@NonNull String sourceId) {
    return nativeMapView.getSource(sourceId);
  }

  /**
   * Tries to cast the Source to T, returns null if it's another type.
   *
   * @param sourceId the id used to look up a layer
   * @param <T>      the generic type of a Source
   * @return the casted Source, null if another type
   */
  @Nullable
  public <T extends Source> T getSourceAs(@NonNull String sourceId) {
    try {
      // noinspection unchecked
      return (T) nativeMapView.getSource(sourceId);
    } catch (ClassCastException exception) {
      Timber.e(exception, "Source: %s is a different type: ", sourceId);
      return null;
    }
  }

  /**
   * Adds the source to the map. The source must be newly created and not added to the map before
   *
   * @param source the source to add
   */
  public void addSource(@NonNull Source source) {
    nativeMapView.addSource(source);
  }

  /**
   * Removes the source. Any references to the source become invalid and should not be used anymore
   *
   * @param sourceId the source to remove
   * @return the source handle or null if the source was not present
   */
  @Nullable
  public Source removeSource(@NonNull String sourceId) {
    return nativeMapView.removeSource(sourceId);
  }

  /**
   * Removes the source, preserving the reverence for re-use
   *
   * @param source the source to remove
   * @return the source
   */
  @Nullable
  public Source removeSource(@NonNull Source source) {
    return nativeMapView.removeSource(source);
  }

  /**
   * Adds an image to be used in the map's style
   *
   * @param name  the name of the image
   * @param image the pre-multiplied Bitmap
   */
  public void addImage(@NonNull String name, @NonNull Bitmap image) {
    nativeMapView.addImage(name, image);
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
  public void removeImage(String name) {
    nativeMapView.removeImage(name);
  }

  //TODO add javadoc
  public Bitmap getImage(@NonNull String name) {
    return nativeMapView.getImage(name);
  }


  /**
   * Returns the map style url currently displayed in the map view.
   *
   * @return The URL of the map style
   */
  public String getUrl() {
    return url;
  }

  /**
   * Returns the map style json currently displayed in the map view.
   *
   * @return The json of the map style
   */
  public String getJson() {
    return json;
  }

  //
  // Native methods
  //

  private native void initialize();

  @Override
  protected native void finalize() throws Throwable;

  //
  // Constants
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
