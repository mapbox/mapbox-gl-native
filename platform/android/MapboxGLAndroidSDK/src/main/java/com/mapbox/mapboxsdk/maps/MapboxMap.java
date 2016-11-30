package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;
import android.location.Location;
import android.os.SystemClock;
import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.util.LongSparseArray;
import android.support.v4.util.Pools;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.MapboxAccountManager;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.BaseMarkerViewOptions;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.InfoWindow;
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
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.NoSuchLayerException;
import com.mapbox.mapboxsdk.style.sources.NoSuchSourceException;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.services.commons.geojson.Feature;

import java.lang.reflect.ParameterizedType;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

/**
 * The general class to interact with in the Android Mapbox SDK. It exposes the entry point for all
 * methods related to the MapView. You cannot instantiate {@link MapboxMap} object directly, rather,
 * you must obtain one from the getMapAsync() method on a MapFragment or MapView that you have
 * added to your application.
 * <p>
 * Note: Similar to a View object, a MapboxMap should only be read and modified from the main thread.
 * </p>
 */
public class MapboxMap {
    private static final String TAG = MapboxMap.class.getSimpleName();

    private MapView mapView;
    private UiSettings uiSettings;
    private TrackingSettings trackingSettings;
    private MyLocationViewSettings myLocationViewSettings;
    private Projection projection;
    private CameraPosition cameraPosition;
    private boolean invalidCameraPosition;
    private LongSparseArray<Annotation> annotations;

    private List<Marker> selectedMarkers;
    private MarkerViewManager markerViewManager;

    private List<InfoWindow> infoWindows;
    private MapboxMap.InfoWindowAdapter infoWindowAdapter;

    private boolean myLocationEnabled;
    private boolean allowConcurrentMultipleInfoWindows;

    private MapboxMap.OnMapClickListener onMapClickListener;
    private MapboxMap.OnMapLongClickListener onMapLongClickListener;
    private MapboxMap.OnMarkerClickListener onMarkerClickListener;
    private MapboxMap.OnInfoWindowClickListener onInfoWindowClickListener;
    private MapboxMap.OnInfoWindowLongClickListener onInfoWindowLongClickListener;
    private MapboxMap.OnInfoWindowCloseListener onInfoWindowCloseListener;
    private MapboxMap.OnFlingListener onFlingListener;
    private MapboxMap.OnScrollListener onScrollListener;
    private MapboxMap.OnMyLocationTrackingModeChangeListener onMyLocationTrackingModeChangeListener;
    private MapboxMap.OnMyBearingTrackingModeChangeListener onMyBearingTrackingModeChangeListener;
    private MapboxMap.OnFpsChangedListener onFpsChangedListener;
    private MapboxMap.OnCameraChangeListener onCameraChangeListener;

    private double maxZoomLevel = -1;
    private double minZoomLevel = -1;

    MapboxMap(@NonNull MapView mapView) {
        this.mapView = mapView;
        this.mapView.addOnMapChangedListener(new MapChangeCameraPositionListener());
        uiSettings = new UiSettings(mapView);
        trackingSettings = new TrackingSettings(this.mapView, uiSettings);
        projection = new Projection(mapView);
        annotations = new LongSparseArray<>();
        selectedMarkers = new ArrayList<>();
        infoWindows = new ArrayList<>();
        markerViewManager = new MarkerViewManager(this, mapView);
    }

    // Style

    /**
     * Returns a layer that conforms to {@link Layer} if any layer with the given identifier was
     * found.
     * <p>
     * Layer identifiers are not guaranteed to exist across styles or different versions of the
     * same style. Applications that use this API must set the style URL to an explicitly versioned
     * style either by passing in the URL String using {@link MapboxMapOptions#styleUrl(String)} or
     * setting through XML using {@link com.mapbox.mapboxsdk.R.attr#style_url}. This approach
     * also avoids layer identifier name changes that will occur in the default style’s layers over
     * time. These default styles can be found in the {@link Style} class.
     * </p>
     *
     * @param layerId a String matching the layer ID found within the current map style. This String
     *                is case sensitive.
     * @return a {@link Layer}, null if the layer doesn't exist.
     */
    @Nullable
    @UiThread
    public Layer getLayer(@NonNull String layerId) {
        return getMapView().getNativeMapView().getLayer(layerId);
    }

    /**
     * Tries to cast the Layer to T, returns null if it's another type.
     * <p>
     * Layer identifiers are not guaranteed to exist across styles or different versions of the
     * same style. Applications that use this API must set the style URL to an explicitly versioned
     * style either by passing in the URL String using {@link MapboxMapOptions#styleUrl(String)} or
     * setting through XML using {@link com.mapbox.mapboxsdk.R.attr#style_url}. This approach
     * also avoids layer identifier name changes that will occur in the default style’s layers over
     * time. These default styles can be found in the {@link Style} class.
     * </p>
     *
     * @param layerId a String matching the layer ID found within the current map style. This String
     *                is case sensitive.
     * @param <T>     the generic attribute of a {@link Layer}.
     * @return the casted {@link Layer}, null if another type.
     */
    @Nullable
    @UiThread
    public <T extends Layer> T getLayerAs(@NonNull String layerId) {
        try {
            //noinspection unchecked
            return (T) getMapView().getNativeMapView().getLayer(layerId);
        } catch (ClassCastException e) {
            Log.e(TAG, String.format("Layer: %s is a different type: %s", layerId, e.getMessage()));
            return null;
        }
    }

    /**
     * Adds a new layer on top of existing layers. The layer must be newly created and not added to the map before
     *
     * @param layer a {@link Layer}.
     */
    @UiThread
    public void addLayer(@NonNull Layer layer) {
        addLayer(layer, null);
    }

    /**
     * Inserts a new layer below another layer. The layer must be newly created and not added to the map before
     * <p>
     * Layer identifiers are not guaranteed to exist across styles or different versions of the
     * same style. Applications that use this API must set the style URL to an explicitly versioned
     * style either by passing in the URL String using {@link MapboxMapOptions#styleUrl(String)} or
     * setting through XML using {@link com.mapbox.mapboxsdk.R.attr#style_url}. This approach
     * also avoids layer identifier name changes that will occur in the default style’s layers over
     * time. These default styles can be found in the {@link Style} class.
     * </p>
     *
     * @param layer  a {@link Layer} to be added to map style.
     * @param before a String layer identifier that's already found in the map view style. If the
     *               layer is not found in the current map style the layer will be added to the top
     *               of the map.
     */
    @UiThread
    public void addLayer(@NonNull Layer layer, String before) {
        getMapView().getNativeMapView().addLayer(layer, before);
    }

    /**
     * Removes a layer from the map style. Any references to the layer become invalid and should not
     * be used anymore
     * <p>
     * Layer identifiers are not guaranteed to exist across styles or different versions of the
     * same style. Applications that use this API must set the style URL to an explicitly versioned
     * style either by passing in the URL String using {@link MapboxMapOptions#styleUrl(String)} or
     * setting through XML using {@link com.mapbox.mapboxsdk.R.attr#style_url}. This approach
     * also avoids layer identifier name changes that will occur in the default style’s layers over
     * time. These default styles can be found in the {@link Style} class.
     * </p>
     *
     * @param layerId a String matching the layer ID found within the current map style. This
     *                String is case sensitive. Any references to the layer become invalid and should
     *                not be used anymore
     * @throws NoSuchLayerException If the layer doesn't exist in the current style, this exception
     *                              will be thrown.
     */
    @UiThread
    public void removeLayer(@NonNull String layerId) throws NoSuchLayerException {
        getMapView().getNativeMapView().removeLayer(layerId);
    }

    /**
     * Removes the layer. The reference is re-usable after this and can be re-added
     *
     * @param layer the layer to remove
     * @throws NoSuchLayerException
     */
    @UiThread
    public void removeLayer(@NonNull Layer layer) throws NoSuchLayerException {
        getMapView().getNativeMapView().removeLayer(layer);
    }

    /**
     * Returns a {@link Source} if any source with the given identifier was found.
     * <p>
     * Source identifiers are not guaranteed to exist across styles or different versions of the
     * same style. Applications that use this API must set the style URL to an explicitly versioned
     * style either by passing in the URL String using {@link MapboxMapOptions#styleUrl(String)} or
     * setting through XML using {@link com.mapbox.mapboxsdk.R.attr#style_url}. This approach
     * also avoids source identifier name changes that will occur in the default style’s source over
     * time. These default styles can be found in the {@link Style} class.
     * </p>
     *
     * @param sourceId a String matching the source ID found within the current map style. This
     *                 String is case sensitive.
     * @return a {@link Source} object, null if the layer doesn't exist.
     */
    @Nullable
    @UiThread
    public Source getSource(@NonNull String sourceId) {
        return getMapView().getNativeMapView().getSource(sourceId);
    }

    /**
     * Tries to cast the Source to T, returns null if it's another type.
     * <p>
     * Source identifiers are not guaranteed to exist across styles or different versions of the
     * same style. Applications that use this API must set the style URL to an explicitly versioned
     * style either by passing in the URL String using {@link MapboxMapOptions#styleUrl(String)} or
     * setting through XML using {@link com.mapbox.mapboxsdk.R.attr#style_url}. This approach
     * also avoids source identifier name changes that will occur in the default style’s source over
     * time. These default styles can be found in the {@link Style} class.
     * </p>
     *
     * @param sourceId the id used to look up a layer.
     * @param <T>      the generic type of a Source.
     * @return the casted Source, null if another type.
     */
    @Nullable
    @UiThread
    public <T extends Source> T getSourceAs(@NonNull String sourceId) {
        try {
            //noinspection unchecked
            return (T) getMapView().getNativeMapView().getSource(sourceId);
        } catch (ClassCastException e) {
            Log.e(TAG, String.format("Source: %s is a different type: %s", sourceId, e.getMessage()));
            return null;
        }
    }

    /**
     * Adds the source to the map. The source must be newly created and not added to the map before
     *
     * @param source the {@link Source} to add to the map view.
     */
    @UiThread
    public void addSource(@NonNull Source source) {
        getMapView().getNativeMapView().addSource(source);
    }

    /**
     * Removes a source from the map style. Any references to the source become invalid and should not be used anymore
     * <p>
     * Source identifiers are not guaranteed to exist across styles or different versions of the
     * same style. Applications that use this API must set the style URL to an explicitly versioned
     * style either by passing in the URL String using {@link MapboxMapOptions#styleUrl(String)} or
     * setting through XML using {@link com.mapbox.mapboxsdk.R.attr#style_url}. This approach
     * also avoids source identifier name changes that will occur in the default style’s source over
     * time. These default styles can be found in the {@link Style} class.
     * </p>
     *
     * @param sourceId a String identifier representing the source to remove.
     * @throws NoSuchSourceException the source trying to be removed doesn't exist in the map.
     */
    @UiThread
    public void removeSource(@NonNull String sourceId) throws NoSuchSourceException {
        getMapView().getNativeMapView().removeSource(sourceId);
    }

    /**
     * Removes the source, preserving the reverence for re-use
     *
     * @param source the source to remove
     * @throws NoSuchSourceException
     */
    @UiThread
    public void removeSource(@NonNull Source source) throws NoSuchSourceException {
        getMapView().getNativeMapView().removeSource(source);
    }

    /**
     * Add an image to be used int hte map's style
     *
     * @param name  the name of the image.
     * @param image the pre-multiplied Bitmap.
     */
    @UiThread
    public void addImage(@NonNull String name, @NonNull Bitmap image) {
        getMapView().getNativeMapView().addImage(name, image);
    }

    /**
     * Removes an image from the map style
     *
     * @param name the name of the image to remove.
     */
    @UiThread
    public void removeImage(String name) {
        getMapView().getNativeMapView().removeImage(name);
    }

    //
    // MinZoom
    //

    /**
     * Sets the minimum zoom level the map can be displayed at.
     *
     * @param minZoom The new minimum zoom level.
     */
    @UiThread
    public void setMinZoom(
            @FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double minZoom) {
        if ((minZoom < MapboxConstants.MINIMUM_ZOOM) || (minZoom > MapboxConstants.MAXIMUM_ZOOM)) {
            Log.e(MapboxConstants.TAG, "Not setting minZoom, value is in unsupported range: " + minZoom);
            return;
        }
        minZoomLevel = minZoom;
        mapView.setMinZoom(minZoom);
    }

    /**
     * Gets the maximum zoom level the map can be displayed at.
     *
     * @return The minimum zoom level.
     */
    @UiThread
    public double getMinZoom() {
        if (minZoomLevel == -1) {
            return minZoomLevel = mapView.getMinZoom();
        }
        return minZoomLevel;
    }

    //
    // MaxZoom
    //

    /**
     * Sets the maximum zoom level the map can be displayed at.
     *
     * @param maxZoom The new maximum zoom level.
     */
    @UiThread
    public void setMaxZoom(
            @FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double maxZoom) {
        if ((maxZoom < MapboxConstants.MINIMUM_ZOOM) || (maxZoom > MapboxConstants.MAXIMUM_ZOOM)) {
            Log.e(MapboxConstants.TAG, "Not setting maxZoom, value is in unsupported range: " + maxZoom);
            return;
        }
        maxZoomLevel = maxZoom;
        mapView.setMaxZoom(maxZoom);
    }

    /**
     * Gets the maximum zoom level the map can be displayed at.
     *
     * @return The maximum zoom level.
     */
    @UiThread
    public double getMaxZoom() {
        if (maxZoomLevel == -1) {
            return maxZoomLevel = mapView.getMaxZoom();
        }
        return maxZoomLevel;
    }

    //
    // UiSettings
    //

    /**
     * Gets the user interface settings for the map.
     *
     * @return the UiSettings associated with this map
     */
    public UiSettings getUiSettings() {
        return uiSettings;
    }

    //
    // TrackingSettings
    //

    /**
     * Gets the tracking interface settings for the map.
     *
     * @return the TrackingSettings asssociated with this map
     */
    public TrackingSettings getTrackingSettings() {
        return trackingSettings;
    }

    //
    // MyLocationViewSettings
    //

    /**
     * Gets the settings of the user location for the map.
     *
     * @return the MyLocationViewSettings associated with this map
     */
    public MyLocationViewSettings getMyLocationViewSettings() {
        if (myLocationViewSettings == null) {
            myLocationViewSettings = new MyLocationViewSettings(mapView, mapView.getUserLocationView());
        }
        return myLocationViewSettings;
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
    public Projection getProjection() {
        return projection;
    }

    //
    // Camera API
    //

    /**
     * Gets the current position of the camera.
     * The CameraPosition returned is a snapshot of the current position, and will not automatically update when the
     * camera moves.
     *
     * @return The current position of the Camera.
     */
    public final CameraPosition getCameraPosition() {
        invalidCameraPosition = invalidCameraPosition || cameraPosition == null;
        if (invalidCameraPosition) {
            invalidateCameraPosition();
        }
        return cameraPosition;
    }

    /**
     * Repositions the camera according to the cameraPosition.
     * The move is instantaneous, and a subsequent getCameraPosition() will reflect the new position.
     * See CameraUpdateFactory for a set of updates.
     *
     * @param cameraPosition the camera position to set
     */
    public void setCameraPosition(@NonNull CameraPosition cameraPosition) {
        moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
    }

    /**
     * Repositions the camera according to the instructions defined in the update.
     * The move is instantaneous, and a subsequent getCameraPosition() will reflect the new position.
     * See CameraUpdateFactory for a set of updates.
     *
     * @param update The change that should be applied to the camera.
     */
    @UiThread
    public final void moveCamera(CameraUpdate update) {
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
    @UiThread
    public final void moveCamera(final CameraUpdate update, final MapboxMap.CancelableCallback callback) {
        mapView.post(new Runnable() {
            @Override
            public void run() {
                cameraPosition = update.getCameraPosition(MapboxMap.this);
                mapView.resetTrackingModesIfRequired(cameraPosition);
                mapView.jumpTo(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
                if (callback != null) {
                    callback.onFinish();
                }

                if (onCameraChangeListener != null) {
                    onCameraChangeListener.onCameraChange(cameraPosition);
                }
            }
        });
    }

    /**
     * Gradually move the camera by the default duration, zoom will not be affected unless specified
     * within {@link CameraUpdate}. If {@link #getCameraPosition()} is called during the animation,
     * it will return the current location of the camera in flight.
     *
     * @param update The change that should be applied to the camera.
     * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
     */
    @UiThread
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
    @UiThread
    public final void easeCamera(CameraUpdate update, int durationMs) {
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
    @UiThread
    public final void easeCamera(CameraUpdate update, int durationMs, final MapboxMap.CancelableCallback callback) {
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
     * @param easingInterpolator The rate of change of the camera animation. If false, the easing
     *                           animation will be linear.
     * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
     */
    @UiThread
    public final void easeCamera(CameraUpdate update, int durationMs, boolean easingInterpolator) {
        easeCamera(update, durationMs, easingInterpolator, null);
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
     * @param easingInterpolator The rate of change of the camera animation. If false, the easing
     *                           animation will be linear.
     * @param callback           An optional callback to be notified from the main thread when the
     *                           animation stops. If the animation stops due to its natural
     *                           completion, the callback will be notified with onFinish(). If the
     *                           animation stops due to interruption by a later camera movement or a
     *                           user gesture, onCancel() will be called. Do not update or ease the
     *                           camera from within onCancel().
     * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
     */
    @UiThread
    public final void easeCamera(
            CameraUpdate update, int durationMs, boolean easingInterpolator, final MapboxMap.CancelableCallback callback) {
        // dismiss tracking, moving camera is equal to a gesture
        easeCamera(update, durationMs, easingInterpolator, true, callback);
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
     * @param easingInterpolator The rate of change of the camera animation. If false, the easing
     *                           animation will be linear.
     * @param resetTrackingMode  Dismiss tracking, moving camera is equal to a gesture.
     * @param callback           An optional callback to be notified from the main thread when the
     *                           animation stops. If the animation stops due to its natural
     *                           completion, the callback will be notified with onFinish(). If the
     *                           animation stops due to interruption by a later camera movement or a
     *                           user gesture, onCancel() will be called. Do not update or ease the
     *                           camera from within onCancel().
     * @see com.mapbox.mapboxsdk.camera.CameraUpdateFactory for a set of updates.
     */
    @UiThread
    public final void easeCamera(final CameraUpdate update, final int durationMs, final boolean easingInterpolator, final boolean resetTrackingMode, final MapboxMap.CancelableCallback callback) {
        mapView.post(new Runnable() {
            @Override
            public void run() {
                // dismiss tracking, moving camera is equal to a gesture
                cameraPosition = update.getCameraPosition(MapboxMap.this);
                if (resetTrackingMode) {
                    mapView.resetTrackingModesIfRequired(cameraPosition);
                }

                mapView.easeTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt,
                        cameraPosition.zoom, easingInterpolator, new CancelableCallback() {
                            @Override
                            public void onCancel() {
                                if (callback != null) {
                                    callback.onCancel();
                                }
                                invalidateCameraPosition();
                            }

                            @Override
                            public void onFinish() {
                                if (callback != null) {
                                    callback.onFinish();
                                }
                                invalidateCameraPosition();
                            }
                        });
            }
        });
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
    @UiThread
    public final void animateCamera(CameraUpdate update) {
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
    @UiThread
    public final void animateCamera(CameraUpdate update, MapboxMap.CancelableCallback callback) {
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
    @UiThread
    public final void animateCamera(CameraUpdate update, int durationMs) {
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
    @UiThread
    public final void animateCamera(final CameraUpdate update, final int durationMs, final MapboxMap.CancelableCallback callback) {
        mapView.post(new Runnable() {
            @Override
            public void run() {
                cameraPosition = update.getCameraPosition(MapboxMap.this);
                mapView.resetTrackingModesIfRequired(cameraPosition);
                mapView.flyTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt,
                        cameraPosition.zoom, new CancelableCallback() {
                            @Override
                            public void onCancel() {
                                if (callback != null) {
                                    callback.onCancel();
                                }
                                invalidateCameraPosition();
                            }

                            @Override
                            public void onFinish() {
                                if (onCameraChangeListener != null) {
                                    onCameraChangeListener.onCameraChange(cameraPosition);
                                }

                                if (callback != null) {
                                    callback.onFinish();
                                }
                                invalidateCameraPosition();
                            }
                        });
            }
        });
    }

    /**
     * Converts milliseconds to nanoseconds
     *
     * @param durationMs The time in milliseconds
     * @return time in nanoseconds
     */
    private long getDurationNano(long durationMs) {
        return durationMs > 0 ? TimeUnit.NANOSECONDS.convert(durationMs, TimeUnit.MILLISECONDS) : 0;
    }

    /**
     * Invalidates the current camera position by reconstructing it from mbgl
     */
    private void invalidateCameraPosition() {
        if (invalidCameraPosition) {
            invalidCameraPosition = false;

            CameraPosition cameraPosition = mapView.invalidateCameraPosition();
            if (cameraPosition != null) {
                this.cameraPosition = cameraPosition;
            }

            if (onCameraChangeListener != null) {
                onCameraChangeListener.onCameraChange(this.cameraPosition);
            }
        }
    }

    //
    //  Reset North
    //

    /**
     * Resets the map view to face north.
     */
    public void resetNorth() {
        mapView.resetNorth();
    }

    //
    // Debug
    //

    /**
     * Returns whether the map debug information is currently shown.
     *
     * @return If true, map debug information is currently shown.
     */
    @UiThread
    public boolean isDebugActive() {
        return mapView.isDebugActive();
    }

    /**
     * <p>
     * Changes whether the map debug information is shown.
     * </p>
     * The default value is false.
     *
     * @param debugActive If true, map debug information is shown.
     */
    @UiThread
    public void setDebugActive(boolean debugActive) {
        mapView.setDebugActive(debugActive);
    }

    /**
     * <p>
     * Cycles through the map debug options.
     * </p>
     * The value of {@link MapView#isDebugActive()} reflects whether there are
     * any map debug options enabled or disabled.
     *
     * @see MapView#isDebugActive()
     */
    @UiThread
    public void cycleDebugOptions() {
        mapView.cycleDebugOptions();
    }

    //
    // Styling
    //

    /**
     * <p>
     * Loads a new map style from the specified URL.
     * </p>
     * {@code url} can take the following forms:
     * <ul>
     * <li>{@code Style.*}: load one of the bundled styles in {@link Style}.</li>
     * <li>{@code mapbox://styles/<user>/<style>}:
     * retrieves the style from a <a href="https://www.mapbox.com/account/">Mapbox account.</a>
     * {@code user} is your username. {@code style} is the ID of your custom
     * style created in <a href="https://www.mapbox.com/studio">Mapbox Studio</a>.</li>
     * <li>{@code http://...} or {@code https://...}:
     * retrieves the style over the Internet from any web server.</li>
     * <li>{@code asset://...}:
     * reads the style from the APK {@code assets/} directory.
     * This is used to load a style bundled with your app.</li>
     * <li>{@code null}: loads the default {@link Style#MAPBOX_STREETS} style.</li>
     * </ul>
     * <p>
     * This method is asynchronous and will return immediately before the style finishes loading.
     * If you wish to wait for the map to finish loading listen for the {@link MapView#DID_FINISH_LOADING_MAP} event.
     * </p>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
     * sent.
     *
     * @param url The URL of the map style
     * @see Style
     */
    @UiThread
    public void setStyleUrl(@NonNull String url) {
        mapView.setStyleUrl(url);
    }

    /**
     * <p>
     * Loads a new map style from the specified bundled style.
     * </p>
     * <p>
     * This method is asynchronous and will return immediately before the style finishes loading.
     * If you wish to wait for the map to finish loading listen for the {@link MapView#DID_FINISH_LOADING_MAP} event.
     * </p>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be
     * sent.
     *
     * @param style The bundled style. Accepts one of the values from {@link Style}.
     * @see Style
     * @deprecated use {@link #setStyleUrl(String)} instead with versioned url methods from {@link Style}
     */
    @UiThread
    @Deprecated
    public void setStyle(@Style.StyleUrl String style) {
        setStyleUrl(style);
    }

    /**
     * <p>
     * Returns the map style currently displayed in the map view.
     * </p>
     * If the default style is currently displayed, a URL will be returned instead of null.
     *
     * @return The URL of the map style.
     */
    @UiThread
    @NonNull
    public String getStyleUrl() {
        return mapView.getStyleUrl();
    }

    //
    // Access token
    //

    /**
     * <p>
     * DEPRECATED @see MapboxAccountManager#start(String)
     * </p>
     * <p>
     * Sets the current Mapbox access token used to load map styles and tiles.
     * </p>
     *
     * @param accessToken Your public Mapbox access token.
     * @see MapView#setAccessToken(String)
     * @deprecated As of release 4.1.0, replaced by {@link com.mapbox.mapboxsdk.MapboxAccountManager#start(Context, String)}
     */
    @Deprecated
    @UiThread
    public void setAccessToken(@NonNull String accessToken) {
        mapView.setAccessToken(accessToken);
    }

    /**
     * <p>
     * DEPRECATED @see MapboxAccountManager#getAccessToken()
     * </p>
     * <p>
     * Returns the current Mapbox access token used to load map styles and tiles.
     * </p>
     *
     * @return The current Mapbox access token.
     * @deprecated As of release 4.1.0, replaced by {@link MapboxAccountManager#getAccessToken()}
     */
    @Deprecated
    @UiThread
    @Nullable
    public String getAccessToken() {
        return mapView.getAccessToken();
    }

    //
    // Annotations
    //

    void setTilt(double tilt) {
        mapView.setTilt(tilt);
    }

    /**
     * <p>
     * Adds a marker to this map.
     * </p>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptions A marker options object that defines how to render the marker.
     * @return The {@code Marker} that was added to the map.
     */
    @UiThread
    @NonNull
    public Marker addMarker(@NonNull MarkerOptions markerOptions) {
        return addMarker((BaseMarkerOptions) markerOptions);
    }

    /**
     * <p>
     * Adds a marker to this map.
     * </p>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptions A marker options object that defines how to render the marker.
     * @return The {@code Marker} that was added to the map.
     */
    @UiThread
    @NonNull
    public Marker addMarker(@NonNull BaseMarkerOptions markerOptions) {
        Marker marker = prepareMarker(markerOptions);
        long id = mapView.addMarker(marker);
        marker.setMapboxMap(this);
        marker.setId(id);
        annotations.put(id, marker);
        return marker;
    }

    /**
     * <p>
     * Adds a marker to this map.
     * </p>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptions A marker options object that defines how to render the marker.
     * @return The {@code Marker} that was added to the map.
     */
    @UiThread
    @NonNull
    public MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions) {
       return addMarker(markerOptions, null);
    }

    /**
     * <p>
     * Adds a marker to this map.
     * </p>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptions A marker options object that defines how to render the marker.
     * @param onMarkerViewAddedListener Callback invoked when the View has been added to the map.
     * @return The {@code Marker} that was added to the map.
     */
    @UiThread
    @NonNull
    public MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions, final MarkerViewManager.OnMarkerViewAddedListener onMarkerViewAddedListener) {
        final MarkerView marker = prepareViewMarker(markerOptions);

        // listen for a render event, so we can invalidate MarkerViews
        mapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
            @Override
            public void onMapChanged(@MapView.MapChange int change) {
                if (change == MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED) {
                    markerViewManager.invalidateViewMarkersInVisibleRegion();
                    if(onMarkerViewAddedListener !=null && markerViewManager.getView(marker)!=null){
                        // checking if view is also found in current viewport.
                        onMarkerViewAddedListener.onViewAdded(marker);
                    }
                    mapView.removeOnMapChangedListener(this);
                }
            }
        });

        // add marker to map
        marker.setMapboxMap(this);
        long id = mapView.addMarker(marker);
        marker.setId(id);
        annotations.put(id, marker);
        return marker;
    }

    /**
     * <p>
     * Adds multiple {@link MarkerView}s to this map.
     * </p>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerViewOptions A list of {@link com.mapbox.mapboxsdk.annotations.MarkerViewOptions}
     *                          objects that defines how to render the markers.
     * @return A list made up of {@link MarkerView} that were added to the map.
     */
    @UiThread
    @NonNull
    public List<MarkerView> addMarkerViews(@NonNull List<? extends BaseMarkerViewOptions> markerViewOptions) {
        List<MarkerView> markers = new ArrayList<>();
        for (BaseMarkerViewOptions markerViewOption : markerViewOptions) {
            if (markerViewOptions.indexOf(markerViewOption) == markerViewOptions.size() - 1) {
                // only invalidate marker views with last item in list
                // listen for a render event, so we can invalidate MarkerViews
                mapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
                    @Override
                    public void onMapChanged(@MapView.MapChange int change) {
                        if (change == MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED) {
                            markerViewManager.invalidateViewMarkersInVisibleRegion();
                            mapView.removeOnMapChangedListener(this);
                        }
                    }
                });
            }
            // add marker to map
            MarkerView marker = prepareViewMarker(markerViewOption);
            marker.setMapboxMap(this);
            long id = mapView.addMarker(marker);
            marker.setId(id);
            annotations.put(id, marker);
            markers.add(marker);
        }
        return markers;
    }

    /**
     * <p>
     * Adds multiple markers to this map.
     * </p>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptionsList A list of marker options objects that defines how to render the markers.
     * @return A list of the {@code Marker}s that were added to the map.
     */
    @UiThread
    @NonNull
    public List<Marker> addMarkers(@NonNull List<? extends BaseMarkerOptions> markerOptionsList) {
        int count = markerOptionsList.size();
        List<Marker> markers = new ArrayList<>(count);
        if (count > 0) {
            BaseMarkerOptions markerOptions;
            Marker marker;
            for (int i = 0; i < count; i++) {
                markerOptions = markerOptionsList.get(i);
                marker = prepareMarker(markerOptions);
                markers.add(marker);
            }

            if (markers.size() > 0) {
                long[] ids = mapView.addMarkers(markers);

                // if unittests or markers are correctly added to map
                if (ids == null || ids.length == markers.size()) {
                    long id = 0;
                    Marker m;
                    for (int i = 0; i < markers.size(); i++) {
                        m = markers.get(i);
                        m.setMapboxMap(this);
                        if (ids != null) {
                            id = ids[i];
                        } else {
                            //unit test
                            id++;
                        }
                        m.setId(id);
                        annotations.put(id, m);
                    }
                }
            }
        }
        return markers;
    }

    /**
     * <p>
     * Updates a marker on this map. Does nothing if the marker is already added.
     * </p>
     *
     * @param updatedMarker An updated marker object.
     */
    @UiThread
    public void updateMarker(@NonNull Marker updatedMarker) {
        mapView.updateMarker(updatedMarker);

        int index = annotations.indexOfKey(updatedMarker.getId());
        if (index > -1) {
            annotations.setValueAt(index, updatedMarker);
        }
    }

    /**
     * Update a polygon on this map.
     *
     * @param polygon An updated polygon object.
     */
    @UiThread
    public void updatePolygon(Polygon polygon) {
        mapView.updatePolygon(polygon);

        int index = annotations.indexOfKey(polygon.getId());
        if (index > -1) {
            annotations.setValueAt(index, polygon);
        }
    }

    /**
     * Update a polyline on this map.
     *
     * @param polyline An updated polyline object.
     */
    @UiThread
    public void updatePolyline(Polyline polyline) {
        mapView.updatePolyline(polyline);

        int index = annotations.indexOfKey(polyline.getId());
        if (index > -1) {
            annotations.setValueAt(index, polyline);
        }
    }

    /**
     * Adds a polyline to this map.
     *
     * @param polylineOptions A polyline options object that defines how to render the polyline.
     * @return The {@code Polyine} that was added to the map.
     */
    @UiThread
    @NonNull
    public Polyline addPolyline(@NonNull PolylineOptions polylineOptions) {
        Polyline polyline = polylineOptions.getPolyline();
        if (!polyline.getPoints().isEmpty()) {
            long id = mapView.addPolyline(polyline);
            polyline.setMapboxMap(this);
            polyline.setId(id);
            annotations.put(id, polyline);
        }
        return polyline;
    }

    /**
     * Adds multiple polylines to this map.
     *
     * @param polylineOptionsList A list of polyline options objects that defines how to render the polylines.
     * @return A list of the {@code Polyline}s that were added to the map.
     */
    @UiThread
    @NonNull
    public List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList) {
        int count = polylineOptionsList.size();
        Polyline polyline;
        List<Polyline> polylines = new ArrayList<>(count);

        if (count > 0) {
            for (PolylineOptions options : polylineOptionsList) {
                polyline = options.getPolyline();
                if (!polyline.getPoints().isEmpty()) {
                    polylines.add(polyline);
                }
            }

            long[] ids = mapView.addPolylines(polylines);

            // if unit tests or polylines are correctly added to map
            if (ids == null || ids.length == polylines.size()) {
                long id = 0;
                Polyline p;

                for (int i = 0; i < polylines.size(); i++) {
                    p = polylines.get(i);
                    p.setMapboxMap(this);
                    if (ids != null) {
                        id = ids[i];
                    } else {
                        // unit test
                        id++;
                    }
                    p.setId(id);
                    annotations.put(id, p);
                }
            }
        }
        return polylines;
    }

    /**
     * Adds a polygon to this map.
     *
     * @param polygonOptions A polygon options object that defines how to render the polygon.
     * @return The {@code Polygon} that was added to the map.
     */
    @UiThread
    @NonNull
    public Polygon addPolygon(@NonNull PolygonOptions polygonOptions) {
        Polygon polygon = polygonOptions.getPolygon();
        if (!polygon.getPoints().isEmpty()) {
            long id = mapView.addPolygon(polygon);
            polygon.setId(id);
            polygon.setMapboxMap(this);
            annotations.put(id, polygon);
        }
        return polygon;
    }

    /**
     * Adds multiple polygons to this map.
     *
     * @param polygonOptionsList A list of polygon options objects that defines how to render the polygons.
     * @return A list of the {@code Polygon}s that were added to the map.
     */
    @UiThread
    @NonNull
    public List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList) {
        int count = polygonOptionsList.size();

        Polygon polygon;
        List<Polygon> polygons = new ArrayList<>(count);
        if (count > 0) {
            for (PolygonOptions polygonOptions : polygonOptionsList) {
                polygon = polygonOptions.getPolygon();
                if (!polygon.getPoints().isEmpty()) {
                    polygons.add(polygon);
                }
            }

            long[] ids = mapView.addPolygons(polygons);

            // if unit tests or polygons correctly added to map
            if (ids == null || ids.length == polygons.size()) {
                long id = 0;
                for (int i = 0; i < polygons.size(); i++) {
                    polygon = polygons.get(i);
                    polygon.setMapboxMap(this);
                    if (ids != null) {
                        id = ids[i];
                    } else {
                        // unit test
                        id++;
                    }
                    polygon.setId(id);
                    annotations.put(id, polygon);
                }
            }
        }
        return polygons;
    }

    /**
     * <p>
     * Convenience method for removing a Marker from the map.
     * </p>
     * Calls removeAnnotation() internally
     *
     * @param marker Marker to remove
     */
    @UiThread
    public void removeMarker(@NonNull Marker marker) {
        removeAnnotation(marker);
    }

    /**
     * <p>
     * Convenience method for removing a Polyline from the map.
     * </p>
     * Calls removeAnnotation() internally
     *
     * @param polyline Polyline to remove
     */
    @UiThread
    public void removePolyline(@NonNull Polyline polyline) {
        removeAnnotation(polyline);
    }

    /**
     * <p>
     * Convenience method for removing a Polygon from the map.
     * </p>
     * Calls removeAnnotation() internally
     *
     * @param polygon Polygon to remove
     */
    @UiThread
    public void removePolygon(@NonNull Polygon polygon) {
        removeAnnotation(polygon);
    }

    /**
     * Removes an annotation from the map.
     *
     * @param annotation The annotation object to remove.
     */
    @UiThread
    public void removeAnnotation(@NonNull Annotation annotation) {
        if (annotation instanceof Marker) {
            Marker marker = (Marker) annotation;
            marker.hideInfoWindow();
            if (marker instanceof MarkerView) {
                markerViewManager.removeMarkerView((MarkerView) marker);
            }
        }
        long id = annotation.getId();
        mapView.removeAnnotation(id);
        annotations.remove(id);
    }

    /**
     * Removes an annotation from the map
     *
     * @param id The identifier associated to the annotation to be removed
     */
    @UiThread
    public void removeAnnotation(long id) {
        mapView.removeAnnotation(id);
        annotations.remove(id);
    }

    /**
     * Removes multiple annotations from the map.
     *
     * @param annotationList A list of annotation objects to remove.
     */
    @UiThread
    public void removeAnnotations(@NonNull List<? extends Annotation> annotationList) {
        int count = annotationList.size();
        long[] ids = new long[count];
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotationList.get(i);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                marker.hideInfoWindow();
                if (marker instanceof MarkerView) {
                    markerViewManager.removeMarkerView((MarkerView) marker);
                }
            }
            ids[i] = annotationList.get(i).getId();
        }
        mapView.removeAnnotations(ids);
        for (long id : ids) {
            annotations.remove(id);
        }
    }

    /**
     * Removes all annotations from the map.
     *
     * @deprecated use {@link MapboxMap#clear()} instead.
     */
    @Deprecated
    @UiThread
    public void removeAnnotations() {
        clear();
    }

    /**
     * Removes all markers, polylines, polygons, etc from the map.
     * <p>
     * This does not affect {@link com.mapbox.mapboxsdk.maps.widgets.MyLocationView}, if
     * enabled/visible on the map it will remain visible to the user. This also doesn't remove any
     * runtime styling layers or sources you might have added to the map.
     * </p>
     */
    @UiThread
    public void clear() {
        Annotation annotation;
        int count = annotations.size();
        long[] ids = new long[count];
        for (int i = 0; i < count; i++) {
            ids[i] = annotations.keyAt(i);
            annotation = annotations.get(ids[i]);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                marker.hideInfoWindow();
                if (marker instanceof MarkerView) {
                    markerViewManager.removeMarkerView((MarkerView) marker);
                }
            }
        }
        mapView.removeAnnotations(ids);
        annotations.clear();
    }

    /**
     * Return an annotation based on its id.
     *
     * @param id the id used to look up an annotation
     * @return An annotation with a matched id, null is returned if no match was found
     */
    @Nullable
    public Annotation getAnnotation(long id) {
        return annotations.get(id);
    }

    /**
     * Returns a list of all the annotations on the map.
     *
     * @return A list of all the annotation objects. The returned object is a copy so modifying this
     * list will not update the map
     */
    @NonNull
    public List<Annotation> getAnnotations() {
        List<Annotation> annotations = new ArrayList<>();
        for (int i = 0; i < this.annotations.size(); i++) {
            annotations.add(this.annotations.get(this.annotations.keyAt(i)));
        }
        return annotations;
    }

    /**
     * Returns a list of all the markers on the map.
     *
     * @return A list of all the markers objects. The returned object is a copy so modifying this
     * list will not update the map.
     */
    @NonNull
    public List<Marker> getMarkers() {
        List<Marker> markers = new ArrayList<>();
        Annotation annotation;
        for (int i = 0; i < annotations.size(); i++) {
            annotation = annotations.get(annotations.keyAt(i));
            if (annotation instanceof Marker) {
                markers.add((Marker) annotation);
            }
        }
        return markers;
    }

    /**
     * Returns a list of all the polygons on the map.
     *
     * @return A list of all the polygon objects. The returned object is a copy so modifying this
     * list will not update the map.
     */
    @NonNull
    public List<Polygon> getPolygons() {
        List<Polygon> polygons = new ArrayList<>();
        Annotation annotation;
        for (int i = 0; i < annotations.size(); i++) {
            annotation = annotations.get(annotations.keyAt(i));
            if (annotation instanceof Polygon) {
                polygons.add((Polygon) annotation);
            }
        }
        return polygons;
    }

    /**
     * Returns a list of all the polylines on the map.
     *
     * @return A list of all the polylines objects. The returned object is a copy so modifying this
     * list will not update the map.
     */
    @NonNull
    public List<Polyline> getPolylines() {
        List<Polyline> polylines = new ArrayList<>();
        Annotation annotation;
        for (int i = 0; i < annotations.size(); i++) {
            annotation = annotations.get(annotations.keyAt(i));
            if (annotation instanceof Polyline) {
                polylines.add((Polyline) annotation);
            }
        }
        return polylines;
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
    @UiThread
    public void selectMarker(@NonNull Marker marker) {
        if (marker == null) {
            Log.w(MapboxConstants.TAG, "marker was null, so just returning");
            return;
        }

        if (selectedMarkers.contains(marker)) {
            return;
        }

        // Need to deselect any currently selected annotation first
        if (!isAllowConcurrentMultipleOpenInfoWindows()) {
            deselectMarkers();
        }

        boolean handledDefaultClick = false;
        if (onMarkerClickListener != null) {
            // end developer has provided a custom click listener
            handledDefaultClick = onMarkerClickListener.onMarkerClick(marker);
        }

        if (!handledDefaultClick) {
            if (marker instanceof MarkerView) {
                markerViewManager.select((MarkerView) marker, false);
                markerViewManager.ensureInfoWindowOffset((MarkerView) marker);
            }

            if (isInfoWindowValidForMarker(marker) || getInfoWindowAdapter() != null) {
                infoWindows.add(marker.showInfoWindow(this, mapView));
            }
        }

        selectedMarkers.add(marker);
    }

    /**
     * Deselects any currently selected marker. All markers will have it's info window closed.
     */
    @UiThread
    public void deselectMarkers() {
        if (selectedMarkers.isEmpty()) {
            return;
        }

        for (Marker marker : selectedMarkers) {
            if (marker.isInfoWindowShown()) {
                marker.hideInfoWindow();
            }

            if (marker instanceof MarkerView) {
                markerViewManager.deselect((MarkerView) marker, false);
            }
        }

        // Removes all selected markers from the list
        selectedMarkers.clear();
    }

    /**
     * Deselects a currently selected marker. The selected marker will have it's info window closed.
     *
     * @param marker the marker to deselect
     */
    @UiThread
    public void deselectMarker(@NonNull Marker marker) {
        if (!selectedMarkers.contains(marker)) {
            return;
        }

        if (marker.isInfoWindowShown()) {
            marker.hideInfoWindow();
        }

        if (marker instanceof MarkerView) {
            markerViewManager.deselect((MarkerView) marker, false);
        }

        selectedMarkers.remove(marker);
    }

    /**
     * Gets the currently selected marker.
     *
     * @return The currently selected marker.
     */
    @UiThread
    public List<Marker> getSelectedMarkers() {
        return selectedMarkers;
    }

    private Marker prepareMarker(BaseMarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Icon icon = mapView.loadIconForMarker(marker);
        marker.setTopOffsetPixels(mapView.getTopOffsetPixelsForIcon(icon));
        return marker;
    }

    private MarkerView prepareViewMarker(BaseMarkerViewOptions markerViewOptions) {
        MarkerView marker = markerViewOptions.getMarker();
        mapView.loadIconForMarkerView(marker);
        return marker;
    }

    /**
     * Get the MarkerViewManager associated to the MapView.
     *
     * @return the associated MarkerViewManager
     */
    public MarkerViewManager getMarkerViewManager() {
        return markerViewManager;
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
    @UiThread
    public void setInfoWindowAdapter(@Nullable InfoWindowAdapter infoWindowAdapter) {
        this.infoWindowAdapter = infoWindowAdapter;
    }

    /**
     * Gets the callback to be invoked when an info window will be shown.
     *
     * @return The callback to be invoked when an info window will be shown.
     */
    @UiThread
    @Nullable
    public InfoWindowAdapter getInfoWindowAdapter() {
        return infoWindowAdapter;
    }

    /**
     * Changes whether the map allows concurrent multiple infowindows to be shown.
     *
     * @param allow If true, map allows concurrent multiple infowindows to be shown.
     */
    @UiThread
    public void setAllowConcurrentMultipleOpenInfoWindows(boolean allow) {
        allowConcurrentMultipleInfoWindows = allow;
    }

    /**
     * Returns whether the map allows concurrent multiple infowindows to be shown.
     *
     * @return If true, map allows concurrent multiple infowindows to be shown.
     */
    @UiThread
    public boolean isAllowConcurrentMultipleOpenInfoWindows() {
        return allowConcurrentMultipleInfoWindows;
    }

    // used by MapView
    List<InfoWindow> getInfoWindows() {
        return infoWindows;
    }

    private boolean isInfoWindowValidForMarker(@NonNull Marker marker) {
        return !TextUtils.isEmpty(marker.getTitle()) || !TextUtils.isEmpty(marker.getSnippet());
    }

    //
    // Padding
    //

    /**
     * <p>
     * Sets the distance from the edges of the map view’s frame to the edges of the map
     * view’s logical viewport.
     * </p>
     * <p>
     * When the value of this property is equal to {0,0,0,0}, viewport
     * properties such as `centerCoordinate` assume a viewport that matches the map
     * view’s frame. Otherwise, those properties are inset, excluding part of the
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
        mapView.setContentPadding(left, top, right, bottom);
        uiSettings.invalidate();
    }

    /**
     * Returns the current configured content padding on map view.
     *
     * @return An array with length 4 in the LTRB order.
     */
    public int[] getPadding() {
        return new int[]{mapView.getContentPaddingLeft(),
                mapView.getContentPaddingTop(),
                mapView.getContentPaddingRight(),
                mapView.getContentPaddingBottom()};
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
    @UiThread
    public void setOnCameraChangeListener(@Nullable OnCameraChangeListener listener) {
        onCameraChangeListener = listener;
    }

    /**
     * Sets a callback that's invoked on every frame rendered to the map view.
     *
     * @param listener The callback that's invoked on every frame rendered to the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnFpsChangedListener(@Nullable OnFpsChangedListener listener) {
        onFpsChangedListener = listener;
    }

    // used by MapView
    OnFpsChangedListener getOnFpsChangedListener() {
        return onFpsChangedListener;
    }

    /**
     * Sets a callback that's invoked when the map is scrolled.
     *
     * @param listener The callback that's invoked when the map is scrolled.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnScrollListener(@Nullable OnScrollListener listener) {
        onScrollListener = listener;
    }

    // used by MapView
    OnScrollListener getOnScrollListener() {
        return onScrollListener;
    }

    /**
     * Sets a callback that's invoked when the map is flinged.
     *
     * @param listener The callback that's invoked when the map is flinged.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnFlingListener(@Nullable OnFlingListener listener) {
        onFlingListener = listener;
    }

    // used by MapView
    OnFlingListener getOnFlingListener() {
        return onFlingListener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on the map view.
     *
     * @param listener The callback that's invoked when the user clicks on the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMapClickListener(@Nullable OnMapClickListener listener) {
        onMapClickListener = listener;
    }

    // used  by MapView
    OnMapClickListener getOnMapClickListener() {
        return onMapClickListener;
    }

    /**
     * Sets a callback that's invoked when the user long clicks on the map view.
     *
     * @param listener The callback that's invoked when the user long clicks on the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMapLongClickListener(@Nullable OnMapLongClickListener listener) {
        onMapLongClickListener = listener;
    }

    // used by MapView
    OnMapLongClickListener getOnMapLongClickListener() {
        return onMapLongClickListener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on a marker.
     *
     * @param listener The callback that's invoked when the user clicks on a marker.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMarkerClickListener(@Nullable OnMarkerClickListener listener) {
        onMarkerClickListener = listener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on an info window.
     *
     * @param listener The callback that's invoked when the user clicks on an info window.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnInfoWindowClickListener(@Nullable OnInfoWindowClickListener listener) {
        onInfoWindowClickListener = listener;
    }

    /**
     * Return the InfoWindow click listener
     *
     * @return Current active InfoWindow Click Listener
     */
    @UiThread
    public OnInfoWindowClickListener getOnInfoWindowClickListener() {
        return onInfoWindowClickListener;
    }

    /**
     * Sets a callback that's invoked when a marker's info window is long pressed.
     *
     * @param listener The callback that's invoked when a marker's info window is long pressed. To unset the callback,
     *                 use null.
     */
    @UiThread
    public void setOnInfoWindowLongClickListener(@Nullable OnInfoWindowLongClickListener listener) {
        onInfoWindowLongClickListener = listener;
    }

    /**
     * Return the InfoWindow long click listener
     *
     * @return Current active InfoWindow long Click Listener
     */
    public OnInfoWindowLongClickListener getOnInfoWindowLongClickListener() {
        return onInfoWindowLongClickListener;
    }

    /**
     * Sets a callback that's invoked when a marker's info window is closed.
     *
     * @param listener The callback that's invoked when a marker's info window is closed. To unset
     *                 the callback, use null.
     */
    public void setOnInfoWindowCloseListener(@Nullable OnInfoWindowCloseListener listener) {
        onInfoWindowCloseListener = listener;
    }

    /**
     * Return the InfoWindow close listener
     *
     * @return Current active InfoWindow Close Listener
     */
    @UiThread
    public OnInfoWindowCloseListener getOnInfoWindowCloseListener() {
        return onInfoWindowCloseListener;
    }

    //
    // User location
    //

    /**
     * Returns the status of the my-location layer.
     *
     * @return True if the my-location layer is enabled, false otherwise.
     */
    @UiThread
    public boolean isMyLocationEnabled() {
        return myLocationEnabled;
    }

    /**
     * <p>
     * Enables or disables the my-location layer.
     * While enabled, the my-location layer continuously draws an indication of a user's current
     * location and bearing.
     * </p>
     * In order to use the my-location layer feature you need to request permission for either
     * {@link android.Manifest.permission#ACCESS_COARSE_LOCATION}
     * or @link android.Manifest.permission#ACCESS_FINE_LOCATION.
     *
     * @param enabled True to enable; false to disable.
     */
    @UiThread
    public void setMyLocationEnabled(boolean enabled) {
        if (!mapView.isPermissionsAccepted()) {
            Log.e(MapboxConstants.TAG, "Could not activate user location tracking: "
                    + "user did not accept the permission or permissions were not requested.");
            return;
        }
        myLocationEnabled = enabled;
        mapView.setMyLocationEnabled(enabled);
    }

    /**
     * Returns the currently displayed user location, or null if there is no location data available.
     *
     * @return The currently displayed user location.
     */
    @UiThread
    @Nullable
    public Location getMyLocation() {
        return mapView.getMyLocation();
    }

    /**
     * Sets a callback that's invoked when the the My Location view
     * (which signifies the user's location) changes location.
     *
     * @param listener The callback that's invoked when the user clicks on a marker.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMyLocationChangeListener(@Nullable MapboxMap.OnMyLocationChangeListener listener) {
        mapView.setOnMyLocationChangeListener(listener);
    }

    /**
     * Sets a callback that's invoked when the location tracking mode changes.
     *
     * @param listener The callback that's invoked when the location tracking mode changes.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMyLocationTrackingModeChangeListener(@Nullable MapboxMap.OnMyLocationTrackingModeChangeListener listener) {
        onMyLocationTrackingModeChangeListener = listener;
    }

    // used by MapView
    MapboxMap.OnMyLocationTrackingModeChangeListener getOnMyLocationTrackingModeChangeListener() {
        return onMyLocationTrackingModeChangeListener;
    }

    /**
     * Sets a callback that's invoked when the bearing tracking mode changes.
     *
     * @param listener The callback that's invoked when the bearing tracking mode changes.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMyBearingTrackingModeChangeListener(@Nullable OnMyBearingTrackingModeChangeListener listener) {
        onMyBearingTrackingModeChangeListener = listener;
    }

    // used by MapView
    OnMyBearingTrackingModeChangeListener getOnMyBearingTrackingModeChangeListener() {
        return onMyBearingTrackingModeChangeListener;
    }

    MapView getMapView() {
        return mapView;
    }

    void setUiSettings(UiSettings uiSettings) {
        this.uiSettings = uiSettings;
    }

    void setProjection(Projection projection) {
        this.projection = projection;
    }

    //
    // Invalidate
    //

    /**
     * Triggers an invalidation of the map view.
     */
    public void invalidate() {
        mapView.invalidate();
    }

    /**
     * Takes a snapshot of the map.
     *
     * @param callback Callback method invoked when the snapshot is taken.
     * @param bitmap   A pre-allocated bitmap.
     */
    @UiThread
    public void snapshot(@NonNull SnapshotReadyCallback callback, @Nullable final Bitmap bitmap) {
        mapView.snapshot(callback, bitmap);
    }

    /**
     * Takes a snapshot of the map.
     *
     * @param callback Callback method invoked when the snapshot is taken.
     */
    @UiThread
    public void snapshot(@NonNull SnapshotReadyCallback callback) {
        mapView.snapshot(callback, null);
    }

    /**
     * Queries the map for rendered features
     *
     * @param coordinates the point to query
     * @param layerIds    optionally - only query these layers
     * @return the list of feature
     */
    @UiThread
    @NonNull
    public List<Feature> queryRenderedFeatures(@NonNull PointF coordinates, @Nullable String... layerIds) {
        return mapView.getNativeMapView().queryRenderedFeatures(coordinates, layerIds);
    }

    /**
     * Queries the map for rendered features
     *
     * @param coordinates the box to query
     * @param layerIds    optionally - only query these layers
     * @return the list of feature
     */
    @UiThread
    @NonNull
    public List<Feature> queryRenderedFeatures(@NonNull RectF coordinates, @Nullable String... layerIds) {
        return mapView.getNativeMapView().queryRenderedFeatures(coordinates, layerIds);
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
     */
    public interface OnScrollListener {
        /**
         * Called when the map is scrolled.
         */
        void onScroll();
    }

    /**
     * Interface definition for a callback to be invoked when the camera changes position.
     */
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
        void onInfoWindowLongClick(Marker marker);
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
        void onInfoWindowClose(Marker marker);
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
     */
    public abstract static class MarkerViewAdapter<U extends MarkerView> {

        private Context context;
        private final Class<U> persistentClass;
        private final Pools.SimplePool<View> viewReusePool;

        /**
         * Create an instance of MarkerViewAdapter.
         *
         * @param context the context associated to a MapView
         */
        @SuppressWarnings("unchecked")
        public MarkerViewAdapter(Context context) {
            this.context = context;
            persistentClass = (Class<U>) ((ParameterizedType) getClass().getGenericSuperclass()).getActualTypeArguments()[0];
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
         * Returning false indicates you want to perform an animation and you are required calling {@link #releaseView(View)} yourself.
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
     * Interface definition for a callback to be invoked when the the My Location view changes location.
     *
     * @see MapboxMap#setOnMyLocationChangeListener(OnMyLocationChangeListener)
     */
    public interface OnMyLocationChangeListener {
        /**
         * Called when the location of the My Location view has changed
         * (be it latitude/longitude, bearing or accuracy).
         *
         * @param location The current location of the My Location view The type of map change event.
         */
        void onMyLocationChange(@Nullable Location location);
    }

    /**
     * Interface definition for a callback to be invoked when the the My Location tracking mode changes.
     *
     * @see MapView#setMyLocationTrackingMode(int)
     */
    public interface OnMyLocationTrackingModeChangeListener {

        /**
         * Called when the tracking mode of My Location tracking has changed
         *
         * @param myLocationTrackingMode the current active location tracking mode
         */
        void onMyLocationTrackingModeChange(@MyLocationTracking.Mode int myLocationTrackingMode);
    }

    /**
     * Interface definition for a callback to be invoked when the the My Location tracking mode changes.
     *
     * @see MapView#setMyLocationTrackingMode(int)
     */
    public interface OnMyBearingTrackingModeChangeListener {

        /**
         * Called when the tracking mode of My Bearing tracking has changed
         *
         * @param myBearingTrackingMode the current active bearing tracking mode
         */
        void onMyBearingTrackingModeChange(@MyBearingTracking.Mode int myBearingTrackingMode);
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
        void onSnapshotReady(Bitmap snapshot);
    }

    private class MapChangeCameraPositionListener implements MapView.OnMapChangedListener {

        private static final long UPDATE_RATE_MS = 400;
        private long previousUpdateTimestamp = 0;

        @Override
        public void onMapChanged(@MapView.MapChange int change) {
            if (change >= MapView.REGION_WILL_CHANGE && change <= MapView.REGION_DID_CHANGE_ANIMATED) {
                invalidCameraPosition = true;
                long currentTime = SystemClock.elapsedRealtime();
                if (currentTime < previousUpdateTimestamp) {
                    return;
                }
                invalidateCameraPosition();
                previousUpdateTimestamp = currentTime + UPDATE_RATE_MS;
            }
        }
    }
}
