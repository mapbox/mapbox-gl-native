package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.Bundle;
import android.support.annotation.FloatRange;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.Size;
import android.support.annotation.UiThread;
import android.support.v4.util.Pools;
import android.text.TextUtils;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.android.gestures.RotateGestureDetector;
import com.mapbox.android.gestures.ShoveGestureDetector;
import com.mapbox.android.gestures.StandardScaleGestureDetector;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Geometry;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.BaseMarkerViewOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.util.HashMap;
import java.util.List;

/**
 * The general class to interact with in the Android Mapbox SDK. It exposes the entry point for all
 * methods related to the MapView. You cannot instantiate {@link MapboxMap} object directly, rather,
 * you must obtain one from the getMapAsync() method on a MapFragment or MapView that you have
 * added to your application.
 * <p>
 * Note: Similar to a View object, a MapboxMap should only be read and modified from the main thread.
 * </p>
 */
@UiThread
public final class MapboxMap {

  private static final String TAG = "Mbgl-MapboxMap";

  private final NativeMapView nativeMapView;

  private final UiSettings uiSettings;
  private final Projection projection;
  private final Transform transform;
  private final AnnotationManager annotationManager;
  private final CameraChangeDispatcher cameraChangeDispatcher;

  private final OnGesturesManagerInteractionListener onGesturesManagerInteractionListener;

  private MapboxMap.OnFpsChangedListener onFpsChangedListener;

  MapboxMap(NativeMapView map, Transform transform, UiSettings ui, Projection projection,
            OnGesturesManagerInteractionListener listener, AnnotationManager annotations,
            CameraChangeDispatcher cameraChangeDispatcher) {
    this.nativeMapView = map;
    this.uiSettings = ui;
    this.projection = projection;
    this.annotationManager = annotations.bind(this);
    this.transform = transform;
    this.onGesturesManagerInteractionListener = listener;
    this.cameraChangeDispatcher = cameraChangeDispatcher;
  }

  void initialise(@NonNull Context context, @NonNull MapboxMapOptions options) {
    transform.initialise(this, options);
    uiSettings.initialise(context, options);

    // Map configuration
    setDebugActive(options.getDebugActive());
    setApiBaseUrl(options);
    setStyleUrl(options);
    setStyleJson(options);
    setPrefetchesTiles(options);
  }

  /**
   * Called when the hosting Activity/Fragment onStart() method is called.
   */
  void onStart() {
    nativeMapView.update();
    if (TextUtils.isEmpty(nativeMapView.getStyleUrl()) && TextUtils.isEmpty(nativeMapView.getStyleJson())) {
      // if user hasn't loaded a Style yet
      nativeMapView.setStyleUrl(Style.MAPBOX_STREETS);
    }
  }

  /**
   * Called when the hosting Activity/Fragment onStop() method is called.
   */
  void onStop() {
  }

  /**
   * Called when the hosting Activity/Fragment is going to be destroyed and map state needs to be saved.
   *
   * @param outState the bundle to save the state to.
   */
  void onSaveInstanceState(@NonNull Bundle outState) {
    outState.putParcelable(MapboxConstants.STATE_CAMERA_POSITION, transform.getCameraPosition());
    outState.putBoolean(MapboxConstants.STATE_DEBUG_ACTIVE, nativeMapView.getDebug());
    outState.putString(MapboxConstants.STATE_STYLE_URL, nativeMapView.getStyleUrl());
    uiSettings.onSaveInstanceState(outState);
  }

  /**
   * Called when the hosting Activity/Fragment is recreated and map state needs to be restored.
   *
   * @param savedInstanceState the bundle containing the saved state
   */
  void onRestoreInstanceState(@NonNull Bundle savedInstanceState) {
    final CameraPosition cameraPosition = savedInstanceState.getParcelable(MapboxConstants.STATE_CAMERA_POSITION);

    uiSettings.onRestoreInstanceState(savedInstanceState);

    if (cameraPosition != null) {
      moveCamera(CameraUpdateFactory.newCameraPosition(
        new CameraPosition.Builder(cameraPosition).build())
      );
    }

    nativeMapView.setDebug(savedInstanceState.getBoolean(MapboxConstants.STATE_DEBUG_ACTIVE));

    final String styleUrl = savedInstanceState.getString(MapboxConstants.STATE_STYLE_URL);
    if (!TextUtils.isEmpty(styleUrl)) {
      nativeMapView.setStyleUrl(savedInstanceState.getString(MapboxConstants.STATE_STYLE_URL));
    }
  }

  /**
   * Called before the OnMapReadyCallback is invoked.
   */
  void onPreMapReady() {
    invalidateCameraPosition();
    annotationManager.reloadMarkers();
    annotationManager.adjustTopOffsetPixels(this);
  }

  /**
   * Called when the OnMapReadyCallback has finished executing.
   * <p>
   * Invalidation of the camera position is required to update the added components in
   * OnMapReadyCallback with the correct transformation.
   * </p>
   */
  void onPostMapReady() {
    invalidateCameraPosition();
  }

  /**
   * Called when the region is changing or has changed.
   */
  void onUpdateRegionChange() {
    annotationManager.update();
  }

  /**
   * Called when the map frame is fully rendered.
   */
  void onUpdateFullyRendered() {
    CameraPosition cameraPosition = transform.invalidateCameraPosition();
    if (cameraPosition != null) {
      uiSettings.update(cameraPosition);
    }
  }

  // Style

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
   * Sets tile pre-fetching from MapboxOptions.
   *
   * @param options the options object
   */
  private void setPrefetchesTiles(@NonNull MapboxMapOptions options) {
    setPrefetchesTiles(options.getPrefetchesTiles());
  }

  /**
   * Enable or disable tile pre-fetching. Pre-fetching makes sure that a low-resolution
   * tile is rendered as soon as possible at the expense of a little bandwidth.
   *
   * @param enable true to enable
   */
  public void setPrefetchesTiles(boolean enable) {
    nativeMapView.setPrefetchesTiles(enable);
  }

  /**
   * Check whether tile pre-fetching is enabled or not.
   *
   * @return true if enabled
   * @see MapboxMap#setPrefetchesTiles(boolean)
   */
  public boolean getPrefetchesTiles() {
    return nativeMapView.getPrefetchesTiles();
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
      Logger.e(TAG, String.format("Layer: %s is a different type: ", layerId), exception);
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
  @NonNull
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
      Logger.e(TAG, String.format("Source: %s is a different type: ", sourceId), exception);
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
   * Removes the source, preserving the reference for re-use
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
  // MinZoom
  //

  /**
   * <p>
   * Sets the minimum zoom level the map can be displayed at.
   * </p>
   *
   * @param minZoom The new minimum zoom level.
   */
  public void setMinZoomPreference(
    @FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double minZoom) {
    transform.setMinZoom(minZoom);
  }

  /**
   * <p>
   * Gets the minimum zoom level the map can be displayed at.
   * </p>
   *
   * @return The minimum zoom level.
   */
  public double getMinZoomLevel() {
    return transform.getMinZoom();
  }

  //
  // MaxZoom
  //

  /**
   * <p>
   * Sets the maximum zoom level the map can be displayed at.
   * </p>
   * <p>
   * The default maximum zoomn level is 22. The upper bound for this value is 25.5.
   * </p>
   *
   * @param maxZoom The new maximum zoom level.
   */
  public void setMaxZoomPreference(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
    to = MapboxConstants.MAXIMUM_ZOOM) double maxZoom) {
    transform.setMaxZoom(maxZoom);
  }

  /**
   * <p>
   * Gets the maximum zoom level the map can be displayed at.
   * </p>
   *
   * @return The maximum zoom level.
   */
  public double getMaxZoomLevel() {
    return transform.getMaxZoom();
  }

  //
  // UiSettings
  //

  /**
   * Gets the user interface settings for the map.
   *
   * @return the UiSettings associated with this map
   */
  @NonNull
  public UiSettings getUiSettings() {
    return uiSettings;
  }

  //
  // Projection
  //

  /**
   * Get the Projection object that you can use to convert between screen coordinates and latitude/longitude
   * coordinates.
   *
   * @return the Projection associated with this map
   */
  @NonNull
  public Projection getProjection() {
    return projection;
  }

  //
  // Light
  //

  /**
   * Get the global light source used to change lighting conditions on extruded fill layers.
   *
   * @return the global light source
   */
  @NonNull
  public Light getLight() {
    return nativeMapView.getLight();
  }

  //
  // Camera API
  //

  /**
   * Cancels ongoing animations.
   * <p>
   * This invokes the {@link CancelableCallback} for ongoing camera updates.
   * </p>
   */
  public void cancelTransitions() {
    transform.cancelTransitions();
  }

  /**
   * Gets the current position of the camera.
   * The CameraPosition returned is a snapshot of the current position, and will not automatically update when the
   * camera moves.
   *
   * @return The current position of the Camera.
   */
  @NonNull
  public final CameraPosition getCameraPosition() {
    return transform.getCameraPosition();
  }

  /**
   * Repositions the camera according to the cameraPosition.
   * The move is instantaneous, and a subsequent getCameraPosition() will reflect the new position.
   * See CameraUpdateFactory for a set of updates.
   *
   * @param cameraPosition the camera position to set
   */
  public void setCameraPosition(@NonNull CameraPosition cameraPosition) {
    moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), null);
  }

  /**
   * Repositions the camera according to the instructions defined in the update.
   * The move is instantaneous, and a subsequent getCameraPosition() will reflect the new position.
   * See CameraUpdateFactory for a set of updates.
   *
   * @param update The change that should be applied to the camera.
   */
  public final void moveCamera(@NonNull CameraUpdate update) {
    moveCamera(update, null);
  }

  /**
   * Repositions the camera according to the instructions defined in the update.
   * The move is instantaneous, and a subsequent getCameraPosition() will reflect the new position.
   * See CameraUpdateFactory for a set of updates.
   *
   * @param update   The change that should be applied to the camera
   * @param callback the callback to be invoked when an animation finishes or is canceled
   */
  public final void moveCamera(@NonNull final CameraUpdate update,
                               @Nullable final MapboxMap.CancelableCallback callback) {
    transform.moveCamera(MapboxMap.this, update, callback);
  }

  /**
   * Gradually move the camera by the default duration, zoom will not be affected unless specified
   * within {@link CameraUpdate}. If {@link #getCameraPosition()} is called during the animation,
   * it will return the current location of the camera in flight.
   *
   * @param update The change that should be applied to the camera.
   * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
   */
  public final void easeCamera(CameraUpdate update) {
    easeCamera(update, MapboxConstants.ANIMATION_DURATION);
  }

  /**
   * Gradually move the camera by a specified duration in milliseconds, zoom will not be affected
   * unless specified within {@link CameraUpdate}. If {@link #getCameraPosition()} is called
   * during the animation, it will return the current location of the camera in flight.
   *
   * @param update     The change that should be applied to the camera.
   * @param durationMs The duration of the animation in milliseconds. This must be strictly
   *                   positive, otherwise an IllegalArgumentException will be thrown.
   * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
   */
  public final void easeCamera(@NonNull CameraUpdate update, int durationMs) {
    easeCamera(update, durationMs, null);
  }

  /**
   * Gradually move the camera by a specified duration in milliseconds, zoom will not be affected
   * unless specified within {@link CameraUpdate}. A callback can be used to be notified when
   * easing the camera stops. If {@link #getCameraPosition()} is called during the animation, it
   * will return the current location of the camera in flight.
   * <p>
   * Note that this will cancel location tracking mode if enabled.
   * </p>
   *
   * @param update     The change that should be applied to the camera.
   * @param durationMs The duration of the animation in milliseconds. This must be strictly
   *                   positive, otherwise an IllegalArgumentException will be thrown.
   * @param callback   An optional callback to be notified from the main thread when the animation
   *                   stops. If the animation stops due to its natural completion, the callback
   *                   will be notified with onFinish(). If the animation stops due to interruption
   *                   by a later camera movement or a user gesture, onCancel() will be called.
   *                   Do not update or ease the camera from within onCancel().
   * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
   */
  public final void easeCamera(@NonNull CameraUpdate update, int durationMs,
                               @Nullable final MapboxMap.CancelableCallback callback) {
    easeCamera(update, durationMs, true, callback);
  }

  /**
   * Gradually move the camera by a specified duration in milliseconds, zoom will not be affected
   * unless specified within {@link CameraUpdate}. A callback can be used to be notified when
   * easing the camera stops. If {@link #getCameraPosition()} is called during the animation, it
   * will return the current location of the camera in flight.
   * <p>
   * Note that this will cancel location tracking mode if enabled.
   * </p>
   *
   * @param update             The change that should be applied to the camera.
   * @param durationMs         The duration of the animation in milliseconds. This must be strictly
   *                           positive, otherwise an IllegalArgumentException will be thrown.
   * @param easingInterpolator True for easing interpolator, false for linear.
   */
  public final void easeCamera(@NonNull CameraUpdate update, int durationMs, boolean easingInterpolator) {
    easeCamera(update, durationMs, easingInterpolator, null);
  }

  /**
   * Gradually move the camera by a specified duration in milliseconds, zoom will not be affected
   * unless specified within {@link CameraUpdate}. A callback can be used to be notified when
   * easing the camera stops. If {@link #getCameraPosition()} is called during the animation, it
   * will return the current location of the camera in flight.
   *
   * @param update             The change that should be applied to the camera.
   * @param durationMs         The duration of the animation in milliseconds. This must be strictly
   *                           positive, otherwise an IllegalArgumentException will be thrown.
   * @param easingInterpolator True for easing interpolator, false for linear.
   * @param callback           An optional callback to be notified from the main thread when the animation
   *                           stops. If the animation stops due to its natural completion, the callback
   *                           will be notified with onFinish(). If the animation stops due to interruption
   *                           by a later camera movement or a user gesture, onCancel() will be called.
   *                           Do not update or ease the camera from within onCancel().
   */
  public final void easeCamera(@NonNull final CameraUpdate update,
                               final int durationMs,
                               final boolean easingInterpolator,
                               @Nullable final MapboxMap.CancelableCallback callback) {
    easeCamera(update, durationMs, easingInterpolator, callback, false);
  }

  /**
   * Gradually move the camera by a specified duration in milliseconds, zoom will not be affected
   * unless specified within {@link CameraUpdate}. A callback can be used to be notified when
   * easing the camera stops. If {@link #getCameraPosition()} is called during the animation, it
   * will return the current location of the camera in flight.
   *
   * @param update             The change that should be applied to the camera.
   * @param durationMs         The duration of the animation in milliseconds. This must be strictly
   *                           positive, otherwise an IllegalArgumentException will be thrown.
   * @param easingInterpolator True for easing interpolator, false for linear.
   * @param callback           An optional callback to be notified from the main thread when the animation
   *                           stops. If the animation stops due to its natural completion, the callback
   *                           will be notified with onFinish(). If the animation stops due to interruption
   *                           by a later camera movement or a user gesture, onCancel() will be called.
   *                           Do not update or ease the camera from within onCancel().
   * @param isDismissable      true will allow animated camera changes dismiss a tracking mode.
   */
  public final void easeCamera(@NonNull final CameraUpdate update, final int durationMs,
                               final boolean easingInterpolator, @Nullable final MapboxMap.CancelableCallback callback,
                               final boolean isDismissable) {

    if (durationMs <= 0) {
      throw new IllegalArgumentException("Null duration passed into easeCamera");
    }
    transform.easeCamera(MapboxMap.this, update, durationMs, easingInterpolator, callback, isDismissable);
  }

  /**
   * Animate the camera to a new location defined within {@link CameraUpdate} using a transition
   * animation that evokes powered flight. The animation will last the default amount of time.
   * During the animation, a call to {@link #getCameraPosition()} returns an intermediate location
   * of the camera in flight.
   *
   * @param update The change that should be applied to the camera.
   * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
   */
  public final void animateCamera(@NonNull CameraUpdate update) {
    animateCamera(update, MapboxConstants.ANIMATION_DURATION, null);
  }

  /**
   * Animate the camera to a new location defined within {@link CameraUpdate} using a transition
   * animation that evokes powered flight. The animation will last the default amount of time. A
   * callback can be used to be notified when animating the camera stops. During the animation, a
   * call to {@link #getCameraPosition()} returns an intermediate location of the camera in flight.
   *
   * @param update   The change that should be applied to the camera.
   * @param callback The callback to invoke from the main thread when the animation stops. If the
   *                 animation completes normally, onFinish() is called; otherwise, onCancel() is
   *                 called. Do not update or animate the camera from within onCancel().
   * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
   */
  public final void animateCamera(@NonNull CameraUpdate update, @Nullable MapboxMap.CancelableCallback callback) {
    animateCamera(update, MapboxConstants.ANIMATION_DURATION, callback);
  }

  /**
   * Animate the camera to a new location defined within {@link CameraUpdate} using a transition
   * animation that evokes powered flight. The animation will last a specified amount of time
   * given in milliseconds. During the animation, a call to {@link #getCameraPosition()} returns
   * an intermediate location of the camera in flight.
   *
   * @param update     The change that should be applied to the camera.
   * @param durationMs The duration of the animation in milliseconds. This must be strictly
   *                   positive, otherwise an IllegalArgumentException will be thrown.
   * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
   */
  public final void animateCamera(@NonNull CameraUpdate update, int durationMs) {
    animateCamera(update, durationMs, null);
  }

  /**
   * Animate the camera to a new location defined within {@link CameraUpdate} using a transition
   * animation that evokes powered flight. The animation will last a specified amount of time
   * given in milliseconds. A callback can be used to be notified when animating the camera stops.
   * During the animation, a call to {@link #getCameraPosition()} returns an intermediate location
   * of the camera in flight.
   *
   * @param update     The change that should be applied to the camera.
   * @param durationMs The duration of the animation in milliseconds. This must be strictly
   *                   positive, otherwise an IllegalArgumentException will be thrown.
   * @param callback   An optional callback to be notified from the main thread when the animation
   *                   stops. If the animation stops due to its natural completion, the callback
   *                   will be notified with onFinish(). If the animation stops due to interruption
   *                   by a later camera movement or a user gesture, onCancel() will be called.
   *                   Do not update or animate the camera from within onCancel(). If a callback
   *                   isn't required, leave it as null.
   * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
   */
  public final void animateCamera(@NonNull final CameraUpdate update, final int durationMs,
                                  @Nullable final MapboxMap.CancelableCallback callback) {
    if (durationMs <= 0) {
      throw new IllegalArgumentException("Null duration passed into animateCamera");
    }

    transform.animateCamera(MapboxMap.this, update, durationMs, callback);
  }

  /**
   * Invalidates the current camera position by reconstructing it from mbgl
   */
  private void invalidateCameraPosition() {
    CameraPosition cameraPosition = transform.invalidateCameraPosition();
    if (cameraPosition != null) {
      transform.updateCameraPosition(cameraPosition);
    }
  }

  //
  //  Reset North
  //

  /**
   * Resets the map view to face north.
   */
  public void resetNorth() {
    transform.resetNorth();
  }

  /**
   * Transform the map bearing given a bearing, focal point coordinates, and a duration.
   *
   * @param bearing  The bearing of the Map to be transformed to
   * @param focalX   The x coordinate of the focal point
   * @param focalY   The y coordinate of the focal point
   * @param duration The duration of the transformation
   */
  public void setFocalBearing(double bearing, float focalX, float focalY, long duration) {
    transform.setBearing(bearing, focalX, focalY, duration);
  }

  /**
   * Returns the measured height of the Map.
   *
   * @return the height of the map
   */
  public float getHeight() {
    return nativeMapView.getHeight();
  }

  /**
   * Returns the measured width of the Map.
   *
   * @return the width of the map
   */
  public float getWidth() {
    return nativeMapView.getWidth();
  }

  //
  // Debug
  //

  /**
   * Returns whether the map debug information is currently shown.
   *
   * @return If true, map debug information is currently shown.
   */
  public boolean isDebugActive() {
    return nativeMapView.getDebug();
  }

  /**
   * <p>
   * Changes whether the map debug information is shown.
   * </p>
   * The default value is false.
   *
   * @param debugActive If true, map debug information is shown.
   */
  public void setDebugActive(boolean debugActive) {
    nativeMapView.setDebug(debugActive);
  }

  /**
   * <p>
   * Cycles through the map debug options.
   * </p>
   * The value of isDebugActive reflects whether there are
   * any map debug options enabled or disabled.
   *
   * @see #isDebugActive()
   */
  public void cycleDebugOptions() {
    nativeMapView.cycleDebugOptions();
  }

  //
  // API endpoint config
  //

  private void setApiBaseUrl(@NonNull MapboxMapOptions options) {
    String apiBaseUrl = options.getApiBaseUrl();
    if (!TextUtils.isEmpty(apiBaseUrl)) {
      nativeMapView.setApiBaseUrl(apiBaseUrl);
    }
  }

  //
  // Styling
  //

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
   * If you wish to wait for the map to finish loading, listen for the {@link MapView#DID_FINISH_LOADING_MAP} event
   * or use the {@link #setStyleUrl(String, OnStyleLoadedListener)} method instead.
   * </p>
   * If the style fails to load or an invalid style URL is set, the map view will become blank.
   * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
   * emitted.
   *
   * @param url The URL of the map style
   * @see Style
   */
  public void setStyleUrl(@NonNull String url) {
    setStyleUrl(url, null);
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
   * <li>{@code null}: loads the default {@link Style#MAPBOX_STREETS} style.</li>
   * </ul>
   * <p>
   * If the style fails to load or an invalid style URL is set, the map view will become blank.
   * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
   * emitted.
   * <p>
   *
   * @param url      The URL of the map style
   * @param callback The callback that is invoked when the style has loaded.
   * @see Style
   */
  public void setStyleUrl(@NonNull final String url, @Nullable final OnStyleLoadedListener callback) {
    if (callback != null) {
      nativeMapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
        @Override
        public void onMapChanged(@MapView.MapChange int change) {
          if (change == MapView.DID_FINISH_LOADING_STYLE) {
            callback.onStyleLoaded(url);
            nativeMapView.removeOnMapChangedListener(this);
          }
        }
      });
    }
    nativeMapView.setStyleUrl(url);
  }

  /**
   * <p>
   * Loads a new map style from the specified bundled style.
   * </p>
   * <p>
   * This method is asynchronous and will return before the style finishes loading.
   * If you wish to wait for the map to finish loading, listen for the {@link MapView#DID_FINISH_LOADING_MAP} event
   * or use the {@link #setStyle(String, OnStyleLoadedListener)} method instead.
   * </p>
   * If the style fails to load or an invalid style URL is set, the map view will become blank.
   * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
   * sent.
   *
   * @param style The bundled style.
   * @see Style
   */
  public void setStyle(@Style.StyleUrl String style) {
    setStyleUrl(style);
  }

  /**
   * <p>
   * Loads a new map style from the specified bundled style.
   * </p>
   * If the style fails to load or an invalid style URL is set, the map view will become blank.
   * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
   * sent.
   *
   * @param style    The bundled style.
   * @param callback The callback to be invoked when the style has finished loading
   * @see Style
   */
  public void setStyle(@Style.StyleUrl String style, @Nullable OnStyleLoadedListener callback) {
    setStyleUrl(style, callback);
  }

  /**
   * Loads a new map style from MapboxMapOptions if available.
   *
   * @param options the object containing the style url
   */
  private void setStyleUrl(@NonNull MapboxMapOptions options) {
    String style = options.getStyleUrl();
    if (!TextUtils.isEmpty(style)) {
      setStyleUrl(style, null);
    }
  }

  /**
   * Returns the map style url currently displayed in the map view.
   *
   * @return The URL of the map style
   */
  @Nullable
  public String getStyleUrl() {
    return nativeMapView.getStyleUrl();
  }

  /**
   * Loads a new map style from a json string.
   * <p>
   * If the style fails to load or an invalid style URL is set, the map view will become blank.
   * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
   * sent.
   * </p>
   */
  public void setStyleJson(@NonNull String styleJson) {
    nativeMapView.setStyleJson(styleJson);
  }

  /**
   * Loads a new map style json from MapboxMapOptions if available.
   *
   * @param options the object containing the style json
   */
  private void setStyleJson(@NonNull MapboxMapOptions options) {
    String styleJson = options.getStyleJson();
    if (!TextUtils.isEmpty(styleJson)) {
      setStyleJson(styleJson);
    }
  }

  /**
   * Returns the map style json currently displayed in the map view.
   *
   * @return The json of the map style
   */
  @NonNull
  public String getStyleJson() {
    return nativeMapView.getStyleJson();
  }

  //
  // Annotations
  //

  /**
   * <p>
   * Adds a marker to this map.
   * </p>
   * The marker's icon is rendered on the map at the location {@code Marker.position}.
   * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
   *
   * @param markerOptions A marker options object that defines how to render the marker
   * @return The {@code Marker} that was added to the map
   */
  @NonNull
  public Marker addMarker(@NonNull MarkerOptions markerOptions) {
    return annotationManager.addMarker(markerOptions, this);
  }

  /**
   * <p>
   * Adds a marker to this map.
   * </p>
   * The marker's icon is rendered on the map at the location {@code Marker.position}.
   * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
   *
   * @param markerOptions A marker options object that defines how to render the marker
   * @return The {@code Marker} that was added to the map
   */
  @NonNull
  public Marker addMarker(@NonNull BaseMarkerOptions markerOptions) {
    return annotationManager.addMarker(markerOptions, this);
  }

  /**
   * <p>
   * Adds a marker to this map.
   * </p>
   * The marker's icon is rendered on the map at the location {@code Marker.position}.
   * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
   *
   * @param markerOptions A marker options object that defines how to render the marker
   * @return The {@code Marker} that was added to the map
   * @deprecated Use a {@link com.mapbox.mapboxsdk.style.layers.SymbolLayer} instead. An example of converting Android
   * SDK views to be used as a symbol see https://github
   * .com/mapbox/mapbox-gl-native/blob/68f32bc104422207c64da8d90e8411b138d87f04/platform/android
   * /MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity/style/SymbolGeneratorActivity.java
   */
  @NonNull
  @Deprecated
  public MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions) {
    return annotationManager.addMarker(markerOptions, this, null);
  }

  /**
   * <p>
   * Adds a marker to this map.
   * </p>
   * The marker's icon is rendered on the map at the location {@code Marker.position}.
   * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
   *
   * @param markerOptions             A marker options object that defines how to render the marker
   * @param onMarkerViewAddedListener Callback invoked when the View has been added to the map
   * @return The {@code Marker} that was added to the map
   * @deprecated Use a {@link com.mapbox.mapboxsdk.style.layers.SymbolLayer} instead. An example of converting Android
   * SDK views to be used as a symbol see https://github
   * .com/mapbox/mapbox-gl-native/blob/68f32bc104422207c64da8d90e8411b138d87f04/platform/android
   * /MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity/style/SymbolGeneratorActivity.java
   */
  @Deprecated
  @NonNull
  public MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions,
                              final MarkerViewManager.OnMarkerViewAddedListener onMarkerViewAddedListener) {
    return annotationManager.addMarker(markerOptions, this, onMarkerViewAddedListener);
  }

  /**
   * Adds multiple markersViews to this map.
   * <p>
   * The marker's icon is rendered on the map at the location {@code Marker.position}.
   * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
   * </p>
   *
   * @param markerViewOptions A list of markerView options objects that defines how to render the markers
   * @return A list of the {@code MarkerView}s that were added to the map
   * @deprecated Use a {@link com.mapbox.mapboxsdk.style.layers.SymbolLayer} instead. An example of converting Android
   * SDK views to be used as a symbol see https://github
   * .com/mapbox/mapbox-gl-native/blob/68f32bc104422207c64da8d90e8411b138d87f04/platform/android
   * /MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity/style/SymbolGeneratorActivity.java
   */
  @NonNull
  @Deprecated
  public List<MarkerView> addMarkerViews(@NonNull List<? extends
    BaseMarkerViewOptions> markerViewOptions) {
    return annotationManager.addMarkerViews(markerViewOptions, this);
  }

  /**
   * Returns markerViews found inside of a rectangle on this map.
   *
   * @param rect the rectangular area on the map to query for markerViews
   * @return A list of the markerViews that were found in the rectangle
   * @deprecated Use a {@link com.mapbox.mapboxsdk.style.layers.SymbolLayer} instead. An example of converting Android
   * SDK views to be used as a symbol see https://github
   * .com/mapbox/mapbox-gl-native/blob/68f32bc104422207c64da8d90e8411b138d87f04/platform/android
   * /MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity/style/SymbolGeneratorActivity.java
   */
  @NonNull
  @Deprecated
  public List<MarkerView> getMarkerViewsInRect(@NonNull RectF rect) {
    return annotationManager.getMarkerViewsInRect(rect);
  }

  /**
   * <p>
   * Adds multiple markers to this map.
   * </p>
   * The marker's icon is rendered on the map at the location {@code Marker.position}.
   * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
   *
   * @param markerOptionsList A list of marker options objects that defines how to render the markers
   * @return A list of the {@code Marker}s that were added to the map
   */
  @NonNull
  public List<Marker> addMarkers(@NonNull List<? extends
    BaseMarkerOptions> markerOptionsList) {
    return annotationManager.addMarkers(markerOptionsList, this);
  }

  /**
   * <p>
   * Updates a marker on this map. Does nothing if the marker isn't already added.
   * </p>
   *
   * @param updatedMarker An updated marker object
   */
  public void updateMarker(@NonNull Marker updatedMarker) {
    annotationManager.updateMarker(updatedMarker, this);
  }

  /**
   * Adds a polyline to this map.
   *
   * @param polylineOptions A polyline options object that defines how to render the polyline
   * @return The {@code Polyine} that was added to the map
   */
  @NonNull
  public Polyline addPolyline(@NonNull PolylineOptions polylineOptions) {
    return annotationManager.addPolyline(polylineOptions, this);
  }

  /**
   * Adds multiple polylines to this map.
   *
   * @param polylineOptionsList A list of polyline options objects that defines how to render the polylines.
   * @return A list of the {@code Polyline}s that were added to the map.
   */
  @NonNull
  public List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList) {
    return annotationManager.addPolylines(polylineOptionsList, this);
  }

  /**
   * Update a polyline on this map.
   *
   * @param polyline An updated polyline object.
   */
  public void updatePolyline(@NonNull Polyline polyline) {
    annotationManager.updatePolyline(polyline);
  }

  /**
   * Adds a polygon to this map.
   *
   * @param polygonOptions A polygon options object that defines how to render the polygon.
   * @return The {@code Polygon} that was added to the map.
   */
  @NonNull
  public Polygon addPolygon(@NonNull PolygonOptions polygonOptions) {
    return annotationManager.addPolygon(polygonOptions, this);
  }

  /**
   * Adds multiple polygons to this map.
   *
   * @param polygonOptionsList A list of polygon options objects that defines how to render the polygons
   * @return A list of the {@code Polygon}s that were added to the map
   */
  @NonNull
  public List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList) {
    return annotationManager.addPolygons(polygonOptionsList, this);
  }

  /**
   * Update a polygon on this map.
   *
   * @param polygon An updated polygon object
   */
  public void updatePolygon(@NonNull Polygon polygon) {
    annotationManager.updatePolygon(polygon);
  }

  /**
   * <p>
   * Convenience method for removing a Marker from the map.
   * </p>
   * Calls removeAnnotation() internally.
   *
   * @param marker Marker to remove
   */
  public void removeMarker(@NonNull Marker marker) {
    annotationManager.removeAnnotation(marker);
  }

  /**
   * <p>
   * Convenience method for removing a Polyline from the map.
   * </p>
   * Calls removeAnnotation() internally.
   *
   * @param polyline Polyline to remove
   */
  public void removePolyline(@NonNull Polyline polyline) {
    annotationManager.removeAnnotation(polyline);
  }

  /**
   * <p>
   * Convenience method for removing a Polygon from the map.
   * </p>
   * Calls removeAnnotation() internally.
   *
   * @param polygon Polygon to remove
   */
  public void removePolygon(@NonNull Polygon polygon) {
    annotationManager.removeAnnotation(polygon);
  }

  /**
   * Removes an annotation from the map.
   *
   * @param annotation The annotation object to remove.
   */
  public void removeAnnotation(@NonNull Annotation annotation) {
    annotationManager.removeAnnotation(annotation);
  }

  /**
   * Removes an annotation from the map
   *
   * @param id The identifier associated to the annotation to be removed
   */
  public void removeAnnotation(long id) {
    annotationManager.removeAnnotation(id);
  }

  /**
   * Removes multiple annotations from the map.
   *
   * @param annotationList A list of annotation objects to remove.
   */
  public void removeAnnotations(@NonNull List<? extends Annotation> annotationList) {
    annotationManager.removeAnnotations(annotationList);
  }

  /**
   * Removes all annotations from the map.
   */
  public void removeAnnotations() {
    annotationManager.removeAnnotations();
  }

  /**
   * Removes all markers, polylines, polygons, overlays, etc from the map.
   */
  public void clear() {
    annotationManager.removeAnnotations();
  }

  /**
   * Return a annotation based on its id.
   *
   * @param id the id used to look up an annotation
   * @return An annotation with a matched id, null is returned if no match was found
   */
  @Nullable
  public Annotation getAnnotation(long id) {
    return annotationManager.getAnnotation(id);
  }

  /**
   * Returns a list of all the annotations on the map.
   *
   * @return A list of all the annotation objects. The returned object is a copy so modifying this
   * list will not update the map
   */
  @NonNull
  public List<Annotation> getAnnotations() {
    return annotationManager.getAnnotations();
  }

  /**
   * Returns a list of all the markers on the map.
   *
   * @return A list of all the markers objects. The returned object is a copy so modifying this
   * list will not update the map.
   */
  @NonNull
  public List<Marker> getMarkers() {
    return annotationManager.getMarkers();
  }

  /**
   * Returns a list of all the polygons on the map.
   *
   * @return A list of all the polygon objects. The returned object is a copy so modifying this
   * list will not update the map.
   */
  @NonNull
  public List<Polygon> getPolygons() {
    return annotationManager.getPolygons();
  }

  /**
   * Returns a list of all the polylines on the map.
   *
   * @return A list of all the polylines objects. The returned object is a copy so modifying this
   * list will not update the map.
   */
  @NonNull
  public List<Polyline> getPolylines() {
    return annotationManager.getPolylines();
  }

  /**
   * Sets a callback that's invoked when the user clicks on a marker.
   *
   * @param listener The callback that's invoked when the user clicks on a marker.
   *                 To unset the callback, use null.
   */
  public void setOnMarkerClickListener(@Nullable OnMarkerClickListener listener) {
    annotationManager.setOnMarkerClickListener(listener);
  }

  /**
   * Sets a callback that's invoked when the user clicks on a polygon.
   *
   * @param listener The callback that's invoked when the user clicks on a polygon.
   *                 To unset the callback, use null.
   */
  public void setOnPolygonClickListener(@Nullable OnPolygonClickListener listener) {
    annotationManager.setOnPolygonClickListener(listener);
  }

  /**
   * Sets a callback that's invoked when the user clicks on a polyline.
   *
   * @param listener The callback that's invoked when the user clicks on a polyline.
   *                 To unset the callback, use null.
   */
  public void setOnPolylineClickListener(@Nullable OnPolylineClickListener listener) {
    annotationManager.setOnPolylineClickListener(listener);
  }

  /**
   * <p>
   * Selects a marker. The selected marker will have it's info window opened.
   * Any other open info windows will be closed unless isAllowConcurrentMultipleOpenInfoWindows()
   * is true.
   * </p>
   * Selecting an already selected marker will have no effect.
   *
   * @param marker The marker to select.
   */
  public void selectMarker(@NonNull Marker marker) {
    if (marker == null) {
      Logger.w(TAG, "marker was null, so just returning");
      return;
    }
    annotationManager.selectMarker(marker);
  }

  /**
   * Deselects any currently selected marker. All markers will have it's info window closed.
   */
  public void deselectMarkers() {
    annotationManager.deselectMarkers();
  }

  /**
   * Deselects a currently selected marker. The selected marker will have it's info window closed.
   *
   * @param marker the marker to deselect
   */
  public void deselectMarker(@NonNull Marker marker) {
    annotationManager.deselectMarker(marker);
  }

  /**
   * Gets the currently selected marker.
   *
   * @return The currently selected marker.
   */
  @NonNull
  public List<Marker> getSelectedMarkers() {
    return annotationManager.getSelectedMarkers();
  }

  /**
   * Get the MarkerViewManager associated to the MapView.
   *
   * @return the associated MarkerViewManager
   */
  @NonNull
  public MarkerViewManager getMarkerViewManager() {
    return annotationManager.getMarkerViewManager();
  }

  //
  // InfoWindow
  //

  /**
   * <p>
   * Sets a custom renderer for the contents of info window.
   * </p>
   * When set your callback is invoked when an info window is about to be shown. By returning
   * a custom {@link View}, the default info window will be replaced.
   *
   * @param infoWindowAdapter The callback to be invoked when an info window will be shown.
   *                          To unset the callback, use null.
   */
  public void setInfoWindowAdapter(@Nullable InfoWindowAdapter infoWindowAdapter) {
    annotationManager.getInfoWindowManager().setInfoWindowAdapter(infoWindowAdapter);
  }

  /**
   * Gets the callback to be invoked when an info window will be shown.
   *
   * @return The callback to be invoked when an info window will be shown.
   */
  @Nullable
  public InfoWindowAdapter getInfoWindowAdapter() {
    return annotationManager.getInfoWindowManager().getInfoWindowAdapter();
  }

  /**
   * Changes whether the map allows concurrent multiple infowindows to be shown.
   *
   * @param allow If true, map allows concurrent multiple infowindows to be shown.
   */
  public void setAllowConcurrentMultipleOpenInfoWindows(boolean allow) {
    annotationManager.getInfoWindowManager().setAllowConcurrentMultipleOpenInfoWindows(allow);
  }

  /**
   * Returns whether the map allows concurrent multiple infowindows to be shown.
   *
   * @return If true, map allows concurrent multiple infowindows to be shown.
   */
  public boolean isAllowConcurrentMultipleOpenInfoWindows() {
    return annotationManager.getInfoWindowManager().isAllowConcurrentMultipleOpenInfoWindows();
  }

  //
  // LatLngBounds
  //

  /**
   * Sets a LatLngBounds that constraints map transformations to this bounds.
   * <p>
   * Set to null to clear current bounds, newly set bounds will override previously set bounds.
   * </p>
   *
   * @param latLngBounds the bounds to constrain the map with
   */
  public void setLatLngBoundsForCameraTarget(@Nullable LatLngBounds latLngBounds) {
    nativeMapView.setLatLngBounds(latLngBounds);
  }


  /**
   * Get a camera position that fits a provided bounds and the current camera tilt and bearing.
   *
   * @param latLngBounds the bounds to set the map with
   * @return the camera position that fits the bounds
   */
  @NonNull
  public CameraPosition getCameraForLatLngBounds(@NonNull LatLngBounds latLngBounds) {
    // we use current camera tilt value to provide expected transformations as #11993
    return getCameraForLatLngBounds(latLngBounds, new int[] {0, 0, 0, 0});
  }


  /**
   * Get a camera position that fits a provided bounds and padding and the current camera tilt and bearing.
   *
   * @param latLngBounds the bounds to set the map with
   * @param padding      the padding to apply to the bounds
   * @return the camera position that fits the bounds and padding
   */
  @NonNull
  public CameraPosition getCameraForLatLngBounds(@NonNull LatLngBounds latLngBounds,
                                                 @NonNull @Size(value = 4) int[] padding) {
    // we use current camera tilt/bearing value to provide expected transformations as #11993
    return getCameraForLatLngBounds(latLngBounds, padding, transform.getRawBearing(), transform.getTilt());
  }


  /**
   * Get a camera position that fits a provided bounds, bearing and tilt.
   *
   * @param latLngBounds the bounds to set the map with
   * @param bearing      the bearing to transform the camera position with
   * @param tilt         to transform the camera position with
   * @return the camera position that fits the bounds and given bearing and tilt
   */
  @NonNull
  public CameraPosition getCameraForLatLngBounds(@NonNull LatLngBounds latLngBounds,
                                                 @FloatRange(from = MapboxConstants.MINIMUM_DIRECTION,
                                                   to = MapboxConstants.MAXIMUM_DIRECTION) double bearing,
                                                 @FloatRange(from = MapboxConstants.MINIMUM_TILT,
                                                   to = MapboxConstants.MAXIMUM_TILT) double tilt) {
    return getCameraForLatLngBounds(latLngBounds, new int[] {0, 0, 0, 0}, bearing, tilt);
  }


  /**
   * Get a camera position that fits a provided bounds, padding, bearing and tilt.
   *
   * @param latLngBounds the bounds to set the map with
   * @param padding      the padding to apply to the bounds
   * @param bearing      the bearing to transform the camera position with
   * @param tilt         to transform the camera position with
   * @return the camera position that fits the bounds, bearing and tilt
   */
  @NonNull
  public CameraPosition getCameraForLatLngBounds(@NonNull LatLngBounds latLngBounds,
                                                 @NonNull @Size(value = 4) int[] padding,
                                                 @FloatRange(from = MapboxConstants.MINIMUM_DIRECTION,
                                                   to = MapboxConstants.MAXIMUM_DIRECTION) double bearing,
                                                 @FloatRange(from = MapboxConstants.MINIMUM_TILT,
                                                   to = MapboxConstants.MAXIMUM_TILT) double tilt) {
    return nativeMapView.getCameraForLatLngBounds(latLngBounds, padding, bearing, tilt);
  }

  /**
   * Get a camera position that fits a provided shape.
   *
   * @param geometry the geometry to wraps the map with
   * @return the camera position that fits the geometry inside
   */
  @NonNull
  public CameraPosition getCameraForGeometry(@NonNull Geometry geometry) {
    // we use current camera tilt value to provide expected transformations as #11993
    return getCameraForGeometry(geometry, new int[] {0, 0, 0, 0});
  }

  /**
   * Get a camera position that fits a provided shape and padding.
   *
   * @param geometry the geometry to wraps the map with
   * @param padding  the padding to apply to the bounds
   * @return the camera position that fits the geometry inside and padding
   */
  @NonNull
  public CameraPosition getCameraForGeometry(@NonNull Geometry geometry,
                                             @NonNull @Size(value = 4) int[] padding) {
    // we use current camera tilt/bearing value to provide expected transformations as #11993
    return getCameraForGeometry(geometry, padding, transform.getBearing(), transform.getTilt());
  }

  /**
   * Get a camera position that fits a provided shape with a given bearing and tilt.
   *
   * @param geometry the geometry to wraps the map with
   * @param bearing  the bearing at which to compute the geometry's bounds
   * @param tilt     the tilt at which to compute the geometry's bounds
   * @return the camera position that the geometry inside with bearing and tilt
   */
  @NonNull
  public CameraPosition getCameraForGeometry(@NonNull Geometry geometry,
                                             @FloatRange(from = MapboxConstants.MINIMUM_DIRECTION,
                                               to = MapboxConstants.MAXIMUM_DIRECTION) double bearing,
                                             @FloatRange(from = MapboxConstants.MINIMUM_TILT,
                                               to = MapboxConstants.MAXIMUM_TILT) double tilt) {
    return getCameraForGeometry(geometry, new int[] {0, 0, 0, 0}, bearing, tilt);
  }

  /**
   * Get a camera position that fits a provided shape with a given padding, bearing and tilt.
   *
   * @param geometry the geometry to wraps the map with
   * @param padding  the padding to apply to the bounds
   * @param bearing  the bearing at which to compute the geometry's bounds
   * @param tilt     the tilt at which to compute the geometry's bounds
   * @return the camera position that fits the geometry inside with padding, bearing and tilt
   */
  @NonNull
  public CameraPosition getCameraForGeometry(@NonNull Geometry geometry,
                                             @NonNull @Size(value = 4) int[] padding,
                                             @FloatRange(from = MapboxConstants.MINIMUM_DIRECTION,
                                               to = MapboxConstants.MAXIMUM_DIRECTION) double bearing,
                                             @FloatRange(from = MapboxConstants.MINIMUM_TILT,
                                               to = MapboxConstants.MAXIMUM_TILT) double tilt) {
    return nativeMapView.getCameraForGeometry(geometry, padding, bearing, tilt);
  }

  /**
   * Get a camera position that fits a provided shape with a given bearing and padding.
   *
   * @param geometry the geometry to wraps the map with
   * @param bearing  the bearing at which to compute the geometry's bounds
   * @param padding  the padding to apply to the bounds
   * @return the camera position that fits the geometry inside with padding and bearing
   * @deprecated use Mapbox{@link #getCameraForGeometry(Geometry, int[], double, double)} instead
   */
  @NonNull
  @Deprecated
  public CameraPosition getCameraForGeometry(Geometry geometry, double bearing, int[] padding) {
    return getCameraForGeometry(geometry, padding, bearing, transform.getTilt());
  }

  //
  // Padding
  //

  /**
   * <p>
   * Sets the distance from the edges of the map view&#x27;s frame to the edges of the map
   * view&#x27s logical viewport.
   * </p>
   * <p>
   * When the value of this property is equal to {0,0,0,0}, viewport
   * properties such as &#x27;centerCoordinate&#x27; assume a viewport that matches the map
   * view&#x27;s frame. Otherwise, those properties are inset, excluding part of the
   * frame from the viewport. For instance, if the only the top edge is inset, the
   * map center is effectively shifted downward.
   * </p>
   *
   * @param left   The left margin in pixels.
   * @param top    The top margin in pixels.
   * @param right  The right margin in pixels.
   * @param bottom The bottom margin in pixels.
   */
  public void setPadding(int left, int top, int right, int bottom) {
    projection.setContentPadding(new int[] {left, top, right, bottom});
    uiSettings.invalidate();
  }

  /**
   * Returns the current configured content padding on map view.
   *
   * @return An array with length 4 in the LTRB order.
   */
  @NonNull
  public int[] getPadding() {
    return projection.getContentPadding();
  }

  //
  // Map events
  //

  /**
   * Sets a callback that's invoked on every change in camera position.
   *
   * @param listener The callback that's invoked on every camera change position.
   *                 To unset the callback, use null.
   */
  @Deprecated
  public void setOnCameraChangeListener(@Nullable OnCameraChangeListener listener) {
    transform.setOnCameraChangeListener(listener);
  }

  /**
   * Sets a callback that is invoked when camera movement has ended.
   *
   * @param listener the listener to notify
   * @deprecated use {@link #addOnCameraIdleListener(OnCameraIdleListener)}
   * and {@link #removeOnCameraIdleListener(OnCameraIdleListener)} instead
   */
  @Deprecated
  public void setOnCameraIdleListener(@Nullable OnCameraIdleListener listener) {
    cameraChangeDispatcher.setOnCameraIdleListener(listener);
  }

  /**
   * Adds a callback that is invoked when camera movement has ended.
   *
   * @param listener the listener to notify
   */
  public void addOnCameraIdleListener(@NonNull OnCameraIdleListener listener) {
    cameraChangeDispatcher.addOnCameraIdleListener(listener);
  }

  /**
   * Removes a callback that is invoked when camera movement has ended.
   *
   * @param listener the listener to remove
   */
  public void removeOnCameraIdleListener(@NonNull OnCameraIdleListener listener) {
    cameraChangeDispatcher.removeOnCameraIdleListener(listener);
  }

  /**
   * Sets a callback that is invoked when camera movement was cancelled.
   *
   * @param listener the listener to notify
   * @deprecated use {@link #addOnCameraMoveCancelListener(OnCameraMoveCanceledListener)} and
   * {@link #removeOnCameraMoveCancelListener(OnCameraMoveCanceledListener)} instead
   */
  @Deprecated
  public void setOnCameraMoveCancelListener(@Nullable OnCameraMoveCanceledListener listener) {
    cameraChangeDispatcher.setOnCameraMoveCanceledListener(listener);
  }

  /**
   * Adds a callback that is invoked when camera movement was cancelled.
   *
   * @param listener the listener to notify
   */
  public void addOnCameraMoveCancelListener(@NonNull OnCameraMoveCanceledListener listener) {
    cameraChangeDispatcher.addOnCameraMoveCancelListener(listener);
  }

  /**
   * Removes a callback that is invoked when camera movement was cancelled.
   *
   * @param listener the listener to remove
   */
  public void removeOnCameraMoveCancelListener(@NonNull OnCameraMoveCanceledListener listener) {
    cameraChangeDispatcher.removeOnCameraMoveCancelListener(listener);
  }

  /**
   * Sets a callback that is invoked when camera movement has started.
   *
   * @param listener the listener to notify
   * @deprecated use {@link #addOnCameraMoveStartedListener(OnCameraMoveStartedListener)} and
   * {@link #removeOnCameraMoveStartedListener(OnCameraMoveStartedListener)} instead
   */
  @Deprecated
  public void setOnCameraMoveStartedListener(@Nullable OnCameraMoveStartedListener listener) {
    cameraChangeDispatcher.setOnCameraMoveStartedListener(listener);
  }

  /**
   * Adds a callback that is invoked when camera movement has started.
   *
   * @param listener the listener to notify
   */
  public void addOnCameraMoveStartedListener(@NonNull OnCameraMoveStartedListener listener) {
    cameraChangeDispatcher.addOnCameraMoveStartedListener(listener);
  }

  /**
   * Removes a callback that is invoked when camera movement has started.
   *
   * @param listener the listener to remove
   */
  public void removeOnCameraMoveStartedListener(@NonNull OnCameraMoveStartedListener listener) {
    cameraChangeDispatcher.removeOnCameraMoveStartedListener(listener);
  }

  /**
   * Sets a callback that is invoked when camera position changes.
   *
   * @param listener the listener to notify
   * @deprecated use {@link #addOnCameraMoveListener(OnCameraMoveListener)} and
   * {@link #removeOnCameraMoveListener(OnCameraMoveListener)}instead
   */
  @Deprecated
  public void setOnCameraMoveListener(@Nullable OnCameraMoveListener listener) {
    cameraChangeDispatcher.setOnCameraMoveListener(listener);
  }

  /**
   * Adds a callback that is invoked when camera position changes.
   *
   * @param listener the listener to notify
   */
  public void addOnCameraMoveListener(@NonNull OnCameraMoveListener listener) {
    cameraChangeDispatcher.addOnCameraMoveListener(listener);
  }

  /**
   * Removes a callback that is invoked when camera position changes.
   *
   * @param listener the listener to remove
   */
  public void removeOnCameraMoveListener(@NonNull OnCameraMoveListener listener) {
    cameraChangeDispatcher.removeOnCameraMoveListener(listener);
  }

  /**
   * Sets a callback that's invoked on every frame rendered to the map view.
   *
   * @param listener The callback that's invoked on every frame rendered to the map view.
   *                 To unset the callback, use null.
   */
  public void setOnFpsChangedListener(@Nullable OnFpsChangedListener listener) {
    onFpsChangedListener = listener;
    nativeMapView.setOnFpsChangedListener(listener);
  }

  // used by MapView
  @Nullable
  OnFpsChangedListener getOnFpsChangedListener() {
    return onFpsChangedListener;
  }

  /**
   * Sets a callback that's invoked when the map is scrolled.
   *
   * @param listener The callback that's invoked when the map is scrolled.
   *                 To unset the callback, use null.
   * @deprecated Use {@link #addOnScrollListener(OnScrollListener)} instead.
   */
  @Deprecated
  public void setOnScrollListener(@Nullable OnScrollListener listener) {
    onGesturesManagerInteractionListener.onSetScrollListener(listener);
  }

  /**
   * Adds a callback that's invoked when the map is scrolled.
   *
   * @param listener The callback that's invoked when the map is scrolled.
   */
  public void addOnScrollListener(@NonNull OnScrollListener listener) {
    onGesturesManagerInteractionListener.onAddScrollListener(listener);
  }

  /**
   * Removes a callback that's invoked when the map is scrolled.
   *
   * @param listener The callback that's invoked when the map is scrolled.
   */
  public void removeOnScrollListener(@NonNull OnScrollListener listener) {
    onGesturesManagerInteractionListener.onRemoveScrollListener(listener);
  }

  /**
   * Sets a callback that's invoked when the map is flinged.
   *
   * @param listener The callback that's invoked when the map is flinged.
   *                 To unset the callback, use null.
   * @deprecated Use {@link #addOnFlingListener(OnFlingListener)} instead.
   */
  @Deprecated
  public void setOnFlingListener(@Nullable OnFlingListener listener) {
    onGesturesManagerInteractionListener.onSetFlingListener(listener);
  }

  /**
   * Adds a callback that's invoked when the map is flinged.
   *
   * @param listener The callback that's invoked when the map is flinged.
   */
  public void addOnFlingListener(@NonNull OnFlingListener listener) {
    onGesturesManagerInteractionListener.onAddFlingListener(listener);
  }

  /**
   * Removes a callback that's invoked when the map is flinged.
   *
   * @param listener The callback that's invoked when the map is flinged.
   */
  public void removeOnFlingListener(@NonNull OnFlingListener listener) {
    onGesturesManagerInteractionListener.onRemoveFlingListener(listener);
  }

  /**
   * Adds a callback that's invoked when the map is moved.
   *
   * @param listener The callback that's invoked when the map is moved.
   */
  public void addOnMoveListener(@NonNull OnMoveListener listener) {
    onGesturesManagerInteractionListener.onAddMoveListener(listener);
  }

  /**
   * Removes a callback that's invoked when the map is moved.
   *
   * @param listener The callback that's invoked when the map is moved.
   */
  public void removeOnMoveListener(@NonNull OnMoveListener listener) {
    onGesturesManagerInteractionListener.onRemoveMoveListener(listener);
  }

  /**
   * Adds a callback that's invoked when the map is rotated.
   *
   * @param listener The callback that's invoked when the map is rotated.
   */
  public void addOnRotateListener(@NonNull OnRotateListener listener) {
    onGesturesManagerInteractionListener.onAddRotateListener(listener);
  }

  /**
   * Removes a callback that's invoked when the map is rotated.
   *
   * @param listener The callback that's invoked when the map is rotated.
   */
  public void removeOnRotateListener(@NonNull OnRotateListener listener) {
    onGesturesManagerInteractionListener.onRemoveRotateListener(listener);
  }

  /**
   * Adds a callback that's invoked when the map is scaled.
   *
   * @param listener The callback that's invoked when the map is scaled.
   */
  public void addOnScaleListener(@NonNull OnScaleListener listener) {
    onGesturesManagerInteractionListener.onAddScaleListener(listener);
  }

  /**
   * Removes a callback that's invoked when the map is scaled.
   *
   * @param listener The callback that's invoked when the map is scaled.
   */
  public void removeOnScaleListener(@NonNull OnScaleListener listener) {
    onGesturesManagerInteractionListener.onRemoveScaleListener(listener);
  }

  /**
   * Adds a callback that's invoked when the map is tilted.
   *
   * @param listener The callback that's invoked when the map is tilted.
   */
  public void addOnShoveListener(@NonNull OnShoveListener listener) {
    onGesturesManagerInteractionListener.onAddShoveListener(listener);
  }

  /**
   * Remove a callback that's invoked when the map is tilted.
   *
   * @param listener The callback that's invoked when the map is tilted.
   */
  public void removeOnShoveListener(@NonNull OnShoveListener listener) {
    onGesturesManagerInteractionListener.onRemoveShoveListener(listener);
  }

  /**
   * Sets a custom {@link AndroidGesturesManager} to handle {@link android.view.MotionEvent}s
   * registered by the {@link MapView}.
   *
   * @param androidGesturesManager       Gestures manager that interprets gestures based on the motion events.
   * @param attachDefaultListeners       If true, pre-defined listeners will be attach
   *                                     to change map based on {@link AndroidGesturesManager} callbacks.
   * @param setDefaultMutuallyExclusives If true, pre-defined mutually exclusive gesture sets
   *                                     will be added to the passed gestures manager.
   * @see <a href="https://github.com/mapbox/mapbox-gestures-android">mapbox-gestures-android library</a>
   */
  public void setGesturesManager(@NonNull AndroidGesturesManager androidGesturesManager, boolean attachDefaultListeners,
                                 boolean setDefaultMutuallyExclusives) {
    onGesturesManagerInteractionListener.setGesturesManager(
      androidGesturesManager, attachDefaultListeners, setDefaultMutuallyExclusives);
  }

  /**
   * Get current {@link AndroidGesturesManager} that handles {@link android.view.MotionEvent}s
   * registered by the {@link MapView}
   *
   * @return Current gestures manager.
   */
  @NonNull
  public AndroidGesturesManager getGesturesManager() {
    return onGesturesManagerInteractionListener.getGesturesManager();
  }

  /**
   * Interrupts any ongoing gesture velocity animations.
   */
  public void cancelAllVelocityAnimations() {
    onGesturesManagerInteractionListener.cancelAllVelocityAnimations();
  }

  /**
   * Sets a callback that's invoked when the user clicks on the map view.
   *
   * @param listener The callback that's invoked when the user clicks on the map view.
   *                 To unset the callback, use null.
   * @deprecated Use {@link #addOnMapClickListener(OnMapClickListener)} instead.
   */
  @Deprecated
  public void setOnMapClickListener(@Nullable OnMapClickListener listener) {
    onGesturesManagerInteractionListener.onSetMapClickListener(listener);
  }

  /**
   * Adds a callback that's invoked when the user clicks on the map view.
   *
   * @param listener The callback that's invoked when the user clicks on the map view.
   */
  public void addOnMapClickListener(@NonNull OnMapClickListener listener) {
    onGesturesManagerInteractionListener.onAddMapClickListener(listener);
  }

  /**
   * Removes a callback that's invoked when the user clicks on the map view.
   *
   * @param listener The callback that's invoked when the user clicks on the map view.
   */
  public void removeOnMapClickListener(@NonNull OnMapClickListener listener) {
    onGesturesManagerInteractionListener.onRemoveMapClickListener(listener);
  }

  /**
   * Sets a callback that's invoked when the user long clicks on the map view.
   *
   * @param listener The callback that's invoked when the user long clicks on the map view.
   *                 To unset the callback, use null.
   * @deprecated Use {@link #addOnMapLongClickListener(OnMapLongClickListener)} instead.
   */
  @Deprecated
  public void setOnMapLongClickListener(@Nullable OnMapLongClickListener listener) {
    onGesturesManagerInteractionListener.onSetMapLongClickListener(listener);
  }

  /**
   * Adds a callback that's invoked when the user long clicks on the map view.
   *
   * @param listener The callback that's invoked when the user long clicks on the map view.
   */
  public void addOnMapLongClickListener(@NonNull OnMapLongClickListener listener) {
    onGesturesManagerInteractionListener.onAddMapLongClickListener(listener);
  }

  /**
   * Removes a callback that's invoked when the user long clicks on the map view.
   *
   * @param listener The callback that's invoked when the user long clicks on the map view.
   */
  public void removeOnMapLongClickListener(@NonNull OnMapLongClickListener listener) {
    onGesturesManagerInteractionListener.onRemoveMapLongClickListener(listener);
  }

  /**
   * Sets a callback that's invoked when the user clicks on an info window.
   *
   * @param listener The callback that's invoked when the user clicks on an info window.
   *                 To unset the callback, use null.
   */
  public void setOnInfoWindowClickListener(@Nullable OnInfoWindowClickListener listener) {
    annotationManager.getInfoWindowManager().setOnInfoWindowClickListener(listener);
  }

  /**
   * Return the InfoWindow click listener
   *
   * @return Current active InfoWindow Click Listener
   */
  @Nullable
  public OnInfoWindowClickListener getOnInfoWindowClickListener() {
    return annotationManager.getInfoWindowManager().getOnInfoWindowClickListener();
  }

  /**
   * Sets a callback that's invoked when a marker's info window is long pressed.
   *
   * @param listener The callback that's invoked when a marker's info window is long pressed. To unset the callback,
   *                 use null.
   */
  public void setOnInfoWindowLongClickListener(@Nullable OnInfoWindowLongClickListener
                                                 listener) {
    annotationManager.getInfoWindowManager().setOnInfoWindowLongClickListener(listener);
  }

  /**
   * Return the InfoWindow long click listener
   *
   * @return Current active InfoWindow long Click Listener
   */
  @Nullable
  public OnInfoWindowLongClickListener getOnInfoWindowLongClickListener() {
    return annotationManager.getInfoWindowManager().getOnInfoWindowLongClickListener();
  }

  /**
   * Set an callback to be invoked when an InfoWindow closes.
   *
   * @param listener callback invoked when an InfoWindow closes
   */
  public void setOnInfoWindowCloseListener(@Nullable OnInfoWindowCloseListener listener) {
    annotationManager.getInfoWindowManager().setOnInfoWindowCloseListener(listener);
  }

  /**
   * Return the InfoWindow close listener
   *
   * @return Current active InfoWindow Close Listener
   */
  @Nullable
  public OnInfoWindowCloseListener getOnInfoWindowCloseListener() {
    return annotationManager.getInfoWindowManager().getOnInfoWindowCloseListener();
  }

  //
  // Invalidate
  //

  /**
   * Takes a snapshot of the map.
   *
   * @param callback Callback method invoked when the snapshot is taken.
   */
  public void snapshot(@NonNull SnapshotReadyCallback callback) {
    nativeMapView.addSnapshotCallback(callback);
  }

  /**
   * Queries the map for rendered features
   *
   * @param coordinates the point to query
   * @param layerIds    optionally - only query these layers
   * @return the list of feature
   */
  @NonNull
  public List<Feature> queryRenderedFeatures(@NonNull PointF coordinates, @Nullable String...
    layerIds) {
    return nativeMapView.queryRenderedFeatures(coordinates, layerIds, null);
  }

  /**
   * Queries the map for rendered features
   *
   * @param coordinates the point to query
   * @param filter      filters the returned features with an expression
   * @param layerIds    optionally - only query these layers
   * @return the list of feature
   */
  @NonNull
  public List<Feature> queryRenderedFeatures(@NonNull PointF coordinates,
                                             @Nullable Expression filter,
                                             @Nullable String... layerIds) {
    return nativeMapView.queryRenderedFeatures(coordinates, layerIds, filter);
  }

  /**
   * Queries the map for rendered features
   *
   * @param coordinates the box to query
   * @param layerIds    optionally - only query these layers
   * @return the list of feature
   */
  @NonNull
  public List<Feature> queryRenderedFeatures(@NonNull RectF coordinates,
                                             @Nullable String... layerIds) {
    return nativeMapView.queryRenderedFeatures(coordinates, layerIds, null);
  }

  /**
   * Queries the map for rendered features
   *
   * @param coordinates the box to query
   * @param filter      filters the returned features with an expression
   * @param layerIds    optionally - only query these layers
   * @return the list of feature
   */
  @NonNull
  public List<Feature> queryRenderedFeatures(@NonNull RectF coordinates,
                                             @Nullable Expression filter,
                                             @Nullable String... layerIds) {
    return nativeMapView.queryRenderedFeatures(coordinates, layerIds, filter);
  }

  //
  // Interfaces
  //

  /**
   * Interface definition for a callback to be invoked when the map is flinged.
   *
   * @see MapboxMap#setOnFlingListener(OnFlingListener)
   */
  public interface OnFlingListener {
    /**
     * Called when the map is flinged.
     */
    void onFling();
  }

  /**
   * Interface definition for a callback to be invoked when the map is scrolled.
   *
   * @see MapboxMap#setOnScrollListener(OnScrollListener)
   * @deprecated Use {@link OnMoveListener} instead.
   */
  @Deprecated
  public interface OnScrollListener {
    /**
     * Called when the map is scrolled.
     */
    void onScroll();
  }

  /**
   * Interface definition for a callback to be invoked when the map is moved.
   *
   * @see MapboxMap#addOnMoveListener(OnMoveListener)
   */
  public interface OnMoveListener {
    void onMoveBegin(@NonNull MoveGestureDetector detector);

    void onMove(@NonNull MoveGestureDetector detector);

    void onMoveEnd(@NonNull MoveGestureDetector detector);
  }

  /**
   * Interface definition for a callback to be invoked when the map is rotated.
   *
   * @see MapboxMap#addOnRotateListener(OnRotateListener)
   */
  public interface OnRotateListener {
    void onRotateBegin(@NonNull RotateGestureDetector detector);

    void onRotate(@NonNull RotateGestureDetector detector);

    void onRotateEnd(@NonNull RotateGestureDetector detector);
  }

  /**
   * Interface definition for a callback to be invoked when the map is scaled.
   *
   * @see MapboxMap#addOnScaleListener(OnScaleListener)
   */
  public interface OnScaleListener {
    void onScaleBegin(@NonNull StandardScaleGestureDetector detector);

    void onScale(@NonNull StandardScaleGestureDetector detector);

    void onScaleEnd(@NonNull StandardScaleGestureDetector detector);
  }

  /**
   * Interface definition for a callback to be invoked when the map is tilted.
   *
   * @see MapboxMap#addOnShoveListener(OnShoveListener)
   */
  public interface OnShoveListener {
    void onShoveBegin(@NonNull ShoveGestureDetector detector);

    void onShove(@NonNull ShoveGestureDetector detector);

    void onShoveEnd(@NonNull ShoveGestureDetector detector);
  }

  /**
   * Interface definition for a callback to be invoked when the camera changes position.
   *
   * @deprecated Replaced by {@link MapboxMap.OnCameraMoveStartedListener}, {@link MapboxMap.OnCameraMoveListener} and
   * {@link MapboxMap.OnCameraIdleListener}. The order in which the deprecated onCameraChange method will be called in
   * relation to the methods in the new camera change listeners is undefined.
   */
  @Deprecated
  public interface OnCameraChangeListener {
    /**
     * Called after the camera position has changed. During an animation,
     * this listener may not be notified of intermediate camera positions.
     * It is always called for the final position in the animation.
     *
     * @param position The CameraPosition at the end of the last camera change.
     */
    void onCameraChange(CameraPosition position);
  }

  /**
   * Interface definition for a callback to be invoked for when the camera motion starts.
   */
  public interface OnCameraMoveStartedListener {
    int REASON_API_GESTURE = 1;
    int REASON_DEVELOPER_ANIMATION = 2;
    int REASON_API_ANIMATION = 3;

    /**
     * Called when the camera starts moving after it has been idle or when the reason for camera motion has changed.
     *
     * @param reason the reason for the camera change
     */
    void onCameraMoveStarted(int reason);
  }

  /**
   * Interface definition for a callback to be invoked for when the camera changes position.
   */
  public interface OnCameraMoveListener {
    /**
     * Called repeatedly as the camera continues to move after an onCameraMoveStarted call.
     * This may be called as often as once every frame and should not perform expensive operations.
     */
    void onCameraMove();
  }

  /**
   * Interface definition for a callback to be invoked for when the camera's motion has been stopped or when the camera
   * starts moving for a new reason.
   */
  public interface OnCameraMoveCanceledListener {
    /**
     * Called when the developer explicitly calls the cancelTransitions() method or if the reason for camera motion has
     * changed before the onCameraIdle had a chance to fire after the previous animation.
     * Do not update or animate the camera from within this method.
     */
    void onCameraMoveCanceled();
  }

  /**
   * Interface definition for a callback to be invoked for when camera movement has ended.
   */
  public interface OnCameraIdleListener {
    /**
     * Called when camera movement has ended.
     */
    void onCameraIdle();
  }

  /**
   * Interface definition for a callback to be invoked for when the compass is animating.
   */
  public interface OnCompassAnimationListener {
    /**
     * Called repeatedly as the compass continues to move after clicking on it.
     */
    void onCompassAnimation();

    /**
     * Called when compass animation has ended.
     */
    void onCompassAnimationFinished();
  }

  /**
   * Interface definition for a callback to be invoked when a frame is rendered to the map view.
   *
   * @see MapboxMap#setOnFpsChangedListener(OnFpsChangedListener)
   */
  public interface OnFpsChangedListener {
    /**
     * Called for every frame rendered to the map view.
     *
     * @param fps The average number of frames rendered over the last second.
     */
    void onFpsChanged(double fps);
  }

  /**
   * Interface definition for a callback to be invoked when a user registers an listener that is
   * related to touch and click events.
   */
  interface OnGesturesManagerInteractionListener {
    void onSetMapClickListener(OnMapClickListener listener);

    void onAddMapClickListener(OnMapClickListener listener);

    void onRemoveMapClickListener(OnMapClickListener listener);

    void onSetMapLongClickListener(OnMapLongClickListener listener);

    void onAddMapLongClickListener(OnMapLongClickListener listener);

    void onRemoveMapLongClickListener(OnMapLongClickListener listener);

    void onSetScrollListener(OnScrollListener listener);

    void onAddScrollListener(OnScrollListener listener);

    void onRemoveScrollListener(OnScrollListener listener);

    void onSetFlingListener(OnFlingListener listener);

    void onAddFlingListener(OnFlingListener listener);

    void onRemoveFlingListener(OnFlingListener listener);

    void onAddMoveListener(OnMoveListener listener);

    void onRemoveMoveListener(OnMoveListener listener);

    void onAddRotateListener(OnRotateListener listener);

    void onRemoveRotateListener(OnRotateListener listener);

    void onAddScaleListener(OnScaleListener listener);

    void onRemoveScaleListener(OnScaleListener listener);

    void onAddShoveListener(OnShoveListener listener);

    void onRemoveShoveListener(OnShoveListener listener);

    AndroidGesturesManager getGesturesManager();

    void setGesturesManager(AndroidGesturesManager gesturesManager, boolean attachDefaultListeners,
                            boolean setDefaultMutuallyExclusives);

    void cancelAllVelocityAnimations();
  }

  /**
   * Interface definition for a callback to be invoked when the user clicks on the map view.
   *
   * @see MapboxMap#setOnMapClickListener(OnMapClickListener)
   */
  public interface OnMapClickListener {
    /**
     * Called when the user clicks on the map view.
     *
     * @param point The projected map coordinate the user clicked on.
     */
    void onMapClick(@NonNull LatLng point);
  }

  /**
   * Interface definition for a callback to be invoked when the user long clicks on the map view.
   *
   * @see MapboxMap#setOnMapLongClickListener(OnMapLongClickListener)
   */
  public interface OnMapLongClickListener {
    /**
     * Called when the user long clicks on the map view.
     *
     * @param point The projected map coordinate the user long clicked on.
     */
    void onMapLongClick(@NonNull LatLng point);
  }

  /**
   * Interface definition for a callback to be invoked when the user clicks on a marker.
   *
   * @see MapboxMap#setOnMarkerClickListener(OnMarkerClickListener)
   */
  public interface OnMarkerClickListener {
    /**
     * Called when the user clicks on a marker.
     *
     * @param marker The marker the user clicked on.
     * @return If true the listener has consumed the event and the info window will not be shown.
     */
    boolean onMarkerClick(@NonNull Marker marker);
  }

  /**
   * Interface definition for a callback to be invoked when the user clicks on a polygon.
   *
   * @see MapboxMap#setOnPolygonClickListener(OnPolygonClickListener)
   */
  public interface OnPolygonClickListener {
    /**
     * Called when the user clicks on a polygon.
     *
     * @param polygon The polygon the user clicked on.
     */
    void onPolygonClick(@NonNull Polygon polygon);
  }

  /**
   * Interface definition for a callback to be invoked when the user clicks on a polyline.
   *
   * @see MapboxMap#setOnPolylineClickListener(OnPolylineClickListener)
   */
  public interface OnPolylineClickListener {
    /**
     * Called when the user clicks on a polyline.
     *
     * @param polyline The polyline the user clicked on.
     */
    void onPolylineClick(@NonNull Polyline polyline);
  }

  /**
   * Interface definition for a callback to be invoked when the user clicks on an info window.
   *
   * @see MapboxMap#setOnInfoWindowClickListener(OnInfoWindowClickListener)
   */
  public interface OnInfoWindowClickListener {
    /**
     * Called when the user clicks on an info window.
     *
     * @param marker The marker of the info window the user clicked on.
     * @return If true the listener has consumed the event and the info window will not be closed.
     */
    boolean onInfoWindowClick(@NonNull Marker marker);
  }

  /**
   * Interface definition for a callback to be invoked when the user long presses on a marker's info window.
   *
   * @see MapboxMap#setOnInfoWindowClickListener(OnInfoWindowClickListener)
   */
  public interface OnInfoWindowLongClickListener {

    /**
     * Called when the user makes a long-press gesture on the marker's info window.
     *
     * @param marker The marker were the info window is attached to
     */
    void onInfoWindowLongClick(@NonNull Marker marker);
  }

  /**
   * Interface definition for a callback to be invoked when a marker's info window is closed.
   *
   * @see MapboxMap#setOnInfoWindowCloseListener(OnInfoWindowCloseListener)
   */
  public interface OnInfoWindowCloseListener {

    /**
     * Called when the marker's info window is closed.
     *
     * @param marker The marker of the info window that was closed.
     */
    void onInfoWindowClose(@NonNull Marker marker);
  }

  /**
   * Interface definition for a callback to be invoked when an info window will be shown.
   *
   * @see MapboxMap#setInfoWindowAdapter(InfoWindowAdapter)
   */
  public interface InfoWindowAdapter {
    /**
     * Called when an info window will be shown as a result of a marker click.
     *
     * @param marker The marker the user clicked on.
     * @return View to be shown as a info window. If null is returned the default
     * info window will be shown.
     */
    @Nullable
    View getInfoWindow(@NonNull Marker marker);
  }

  /**
   * Interface definition for a callback to be invoked when an MarkerView will be shown.
   *
   * @param <U> the instance type of MarkerView
   * @deprecated Use a {@link com.mapbox.mapboxsdk.style.layers.SymbolLayer} instead. An example of converting Android
   * SDK views to be used as a symbol see https://github
   * .com/mapbox/mapbox-gl-native/blob/68f32bc104422207c64da8d90e8411b138d87f04/platform/android
   * /MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity/style/SymbolGeneratorActivity.java
   */
  @Deprecated
  public abstract static class MarkerViewAdapter<U extends MarkerView> {

    private Context context;
    private final Class<U> persistentClass;
    private final Pools.SimplePool<View> viewReusePool;

    /**
     * Create an instance of MarkerViewAdapter.
     *
     * @param context the context associated to a MapView
     */
    public MarkerViewAdapter(Context context, Class<U> persistentClass) {
      this.context = context;
      this.persistentClass = persistentClass;
      viewReusePool = new Pools.SimplePool<>(10000);
    }

    /**
     * Called when an MarkerView will be added to the MapView.
     *
     * @param marker      the model representing the MarkerView
     * @param convertView the reusable view
     * @param parent      the parent ViewGroup of the convertview
     * @return the View that is adapted to the contents of MarkerView
     */
    @Nullable
    public abstract View getView(@NonNull U marker, @Nullable View convertView, @NonNull ViewGroup parent);

    /**
     * Called when an MarkerView is removed from the MapView or the View object is going to be reused.
     * <p>
     * This method should be used to reset an animated view back to it's original state for view reuse.
     * </p>
     * <p>
     * Returning true indicates you want to the view reuse to be handled automatically.
     * Returning false indicates you want to perform an animation and you are required calling
     * {@link #releaseView(View)} yourself.
     * </p>
     *
     * @param marker      the model representing the MarkerView
     * @param convertView the reusable view
     * @return true if you want reuse to occur automatically, false if you want to manage this yourself.
     */
    public boolean prepareViewForReuse(@NonNull MarkerView marker, @NonNull View convertView) {
      return true;
    }

    /**
     * Called when a MarkerView is selected from the MapView.
     * <p>
     * Returning true from this method indicates you want to move the MarkerView to the selected state.
     * Returning false indicates you want to animate the View first an manually select the MarkerView when appropriate.
     * </p>
     *
     * @param marker                   the model representing the MarkerView
     * @param convertView              the reusable view
     * @param reselectionFromRecycling indicates if the onSelect callback is the initial selection
     *                                 callback or that selection occurs due to recreation of selected marker
     * @return true if you want to select the Marker immediately, false if you want to manage this yourself.
     */
    public boolean onSelect(@NonNull U marker, @NonNull View convertView, boolean reselectionFromRecycling) {
      return true;
    }

    /**
     * Called when a MarkerView is deselected from the MapView.
     *
     * @param marker      the model representing the MarkerView
     * @param convertView the reusable view
     */
    public void onDeselect(@NonNull U marker, @NonNull View convertView) {
    }

    /**
     * Returns the generic type of the used MarkerView.
     *
     * @return the generic type
     */
    public final Class<U> getMarkerClass() {
      return persistentClass;
    }

    /**
     * Returns the pool used to store reusable Views.
     *
     * @return the pool associated to this adapter
     */
    public final Pools.SimplePool<View> getViewReusePool() {
      return viewReusePool;
    }

    /**
     * Returns the context associated to the hosting MapView.
     *
     * @return the context used
     */
    public final Context getContext() {
      return context;
    }

    /**
     * Release a View to the ViewPool.
     *
     * @param view the view to be released
     */
    public final void releaseView(View view) {
      view.setVisibility(View.GONE);
      viewReusePool.release(view);
    }
  }

  /**
   * Interface definition for a callback to be invoked when the user clicks on a MarkerView.
   */
  public interface OnMarkerViewClickListener {

    /**
     * Called when the user clicks on a MarkerView.
     *
     * @param marker  the MarkerView associated to the clicked View
     * @param view    the clicked View
     * @param adapter the adapter used to adapt the MarkerView to the View
     * @return If true the listener has consumed the event and the info window will not be shown
     */
    boolean onMarkerClick(@NonNull Marker marker, @NonNull View view, @NonNull MarkerViewAdapter adapter);
  }

  /**
   * Interface definition for a callback to be invoked when a task is complete or cancelled.
   */
  public interface CancelableCallback {
    /**
     * Invoked when a task is cancelled.
     */
    void onCancel();

    /**
     * Invoked when a task is complete.
     */
    void onFinish();
  }

  /**
   * Interface definition for a callback to be invoked when the snapshot has been taken.
   */
  public interface SnapshotReadyCallback {
    /**
     * Invoked when the snapshot has been taken.
     *
     * @param snapshot the snapshot bitmap
     */
    void onSnapshotReady(@NonNull Bitmap snapshot);
  }

  /**
   * Interface definition for a callback to be invoked when the style has finished loading.
   */
  public interface OnStyleLoadedListener {
    /**
     * Invoked when the style has finished loading
     *
     * @param style the style that has been loaded
     */
    void onStyleLoaded(@NonNull String style);
  }

  //
  // Used for instrumentation testing
  //
  @NonNull
  Transform getTransform() {
    return transform;
  }
}
