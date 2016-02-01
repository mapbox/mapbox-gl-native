package com.mapbox.mapboxsdk.maps;

import android.Manifest;
import android.content.Context;
import android.graphics.PointF;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresPermission;
import android.support.annotation.UiThread;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.InfoWindow;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
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
import com.mapbox.mapboxsdk.layers.CustomLayer;
import com.mapbox.mapboxsdk.utils.ApiAccess;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class MapboxMap {

    private MapView mMapView;
    private UiSettings mUiSettings;
    private CameraPosition mCurrentCameraPosition;
    private String mStyleUrl;
    private List<Marker> mSelectedMarkers;
    private List<InfoWindow> mInfoWindows;
    private MapboxMap.InfoWindowAdapter mInfoWindowAdapter;

    private boolean mMyLocationEnabled;
    private boolean mAllowConcurrentMultipleInfoWindows;

    private MapboxMap.OnMapClickListener mOnMapClickListener;
    private MapboxMap.OnMapLongClickListener mOnMapLongClickListener;
    private MapboxMap.OnMarkerClickListener mOnMarkerClickListener;
    private MapboxMap.OnInfoWindowClickListener mOnInfoWindowClickListener;
    private MapboxMap.OnFlingListener mOnFlingListener;
    private MapboxMap.OnScrollListener mOnScrollListener;
    private MapboxMap.OnMyLocationTrackingModeChangeListener mOnMyLocationTrackingModeChangeListener;
    private MapboxMap.OnMyBearingTrackingModeChangeListener mOnMyBearingTrackingModeChangeListener;
    private MapboxMap.OnFpsChangedListener mOnFpsChangedListener;

    MapboxMap(@NonNull MapView mapView) {
        mMapView = mapView;
        mUiSettings = new UiSettings(this);
        mSelectedMarkers = new ArrayList<>();
        mInfoWindows = new ArrayList<>();
    }

    //
    // UiSettings
    //

    /**
     * Gets the user interface settings for the map.
     *
     * @return
     */
    public UiSettings getUiSettings() {
        return mUiSettings;
    }

    //
    // Camera API
    //

    /**
     * Gets the current position of the camera.
     * The CameraPosition returned is a snapshot of the current position, and will not automatically update when the camera moves.
     *
     * @return The current position of the Camera.
     */
    public final CameraPosition getCameraPosition() {
        return mCurrentCameraPosition;
    }

    /**
     * Repositions the camera according to the cameraPosition.
     * The move is instantaneous, and a subsequent getCameraPosition() will reflect the new position.
     * See CameraUpdateFactory for a set of updates.
     *
     * @param cameraPosition
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
        if (update instanceof CameraUpdateFactory.PositionCameraUpdate) {
            CameraUpdateFactory.PositionCameraUpdate positionCameraUpdate = (CameraUpdateFactory.PositionCameraUpdate) update;
            CameraPosition cameraPosition = new CameraPosition.Builder(positionCameraUpdate).build();
            updateCameraPosition(cameraPosition);
            mMapView.jumpTo(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
        }else if (update instanceof CameraUpdateFactory.ZoomUpdate) {
            mMapView.setZoom((CameraUpdateFactory.ZoomUpdate) update, 0);
        }
    }

    /**
     * Ease the map according to the update with an animation over a specified duration, and calls an optional callback on completion. See CameraUpdateFactory for a set of updates.
     * If getCameraPosition() is called during the animation, it will return the current location of the camera in flight.
     *
     * @param update     The change that should be applied to the camera.
     * @param durationMs The duration of the animation in milliseconds. This must be strictly positive, otherwise an IllegalArgumentException will be thrown.
     */
    @UiThread
    public final void easeCamera(CameraUpdate update, int durationMs) {
        easeCamera(update, durationMs, null);
    }

    /**
     * Ease the map according to the update with an animation over a specified duration, and calls an optional callback on completion. See CameraUpdateFactory for a set of updates.
     * If getCameraPosition() is called during the animation, it will return the current location of the camera in flight.
     *
     * @param update     The change that should be applied to the camera.
     * @param durationMs The duration of the animation in milliseconds. This must be strictly positive, otherwise an IllegalArgumentException will be thrown.
     * @param callback   An optional callback to be notified from the main thread when the animation stops. If the animation stops due to its natural completion, the callback will be notified with onFinish(). If the animation stops due to interruption by a later camera movement or a user gesture, onCancel() will be called. The callback should not attempt to move or animate the camera in its cancellation method. If a callback isn't required, leave it as null.
     */
    @UiThread
    public final void easeCamera(CameraUpdate update, int durationMs, final MapboxMap.CancelableCallback callback) {
        if (update instanceof CameraUpdateFactory.PositionCameraUpdate) {
            CameraUpdateFactory.PositionCameraUpdate positionCameraUpdate = (CameraUpdateFactory.PositionCameraUpdate) update;
            CameraPosition cameraPosition = new CameraPosition.Builder(positionCameraUpdate).build();
            updateCameraPosition(cameraPosition);
            mMapView.easeTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt, cameraPosition.zoom, callback);
        }else if (update instanceof CameraUpdateFactory.ZoomUpdate) {
            mMapView.setZoom((CameraUpdateFactory.ZoomUpdate) update, durationMs);
        }
    }

    /**
     * Animates the movement of the camera from the current position to the position defined in the update.
     * During the animation, a call to getCameraPosition() returns an intermediate location of the camera.
     * <p/>
     * See CameraUpdateFactory for a set of updates.
     *
     * @param update The change that should be applied to the camera.
     */
    @UiThread
    public final void animateCamera(CameraUpdate update) {
        animateCamera(update, MapboxConstants.ANIMATION_DURATION, null);
    }

    /**
     * Animates the movement of the camera from the current position to the position defined in the update and calls an optional callback on completion.
     * See CameraUpdateFactory for a set of updates.
     * During the animation, a call to getCameraPosition() returns an intermediate location of the camera.
     *
     * @param update   The change that should be applied to the camera.
     * @param callback The callback to invoke from the main thread when the animation stops. If the animation completes normally, onFinish() is called; otherwise, onCancel() is called. Do not update or animate the camera from within onCancel().
     */
    @UiThread
    public final void animateCamera(CameraUpdate update, MapboxMap.CancelableCallback callback) {
        animateCamera(update, MapboxConstants.ANIMATION_DURATION, callback);
    }

    /**
     * Moves the map according to the update with an animation over a specified duration. See CameraUpdateFactory for a set of updates.
     * If getCameraPosition() is called during the animation, it will return the current location of the camera in flight.
     *
     * @param update     The change that should be applied to the camera.
     * @param durationMs The duration of the animation in milliseconds. This must be strictly positive, otherwise an IllegalArgumentException will be thrown.
     */
    @UiThread
    public final void animateCamera(CameraUpdate update, int durationMs) {
        animateCamera(update, durationMs, null);
    }

    /**
     * Moves the map according to the update with an animation over a specified duration, and calls an optional callback on completion. See CameraUpdateFactory for a set of updates.
     * If getCameraPosition() is called during the animation, it will return the current location of the camera in flight.
     *
     * @param update     The change that should be applied to the camera.
     * @param durationMs The duration of the animation in milliseconds. This must be strictly positive, otherwise an IllegalArgumentException will be thrown.
     * @param callback   An optional callback to be notified from the main thread when the animation stops. If the animation stops due to its natural completion, the callback will be notified with onFinish(). If the animation stops due to interruption by a later camera movement or a user gesture, onCancel() will be called. The callback should not attempt to move or animate the camera in its cancellation method. If a callback isn't required, leave it as null.
     */
    @UiThread
    public final void animateCamera(CameraUpdate update, int durationMs, final MapboxMap.CancelableCallback callback) {
        if (update instanceof CameraUpdateFactory.PositionCameraUpdate) {
            CameraUpdateFactory.PositionCameraUpdate positionCameraUpdate = (CameraUpdateFactory.PositionCameraUpdate) update;
            CameraPosition cameraPosition = new CameraPosition.Builder(positionCameraUpdate).build();
            updateCameraPosition(cameraPosition);
            mMapView.flyTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt, cameraPosition.zoom, callback);
        } else if (update instanceof CameraUpdateFactory.ZoomUpdate) {
            mMapView.setZoom((CameraUpdateFactory.ZoomUpdate) update, durationMs);
        }
    }

    // internal setter for CameraPosition
    void updateCameraPosition(@NonNull CameraPosition cameraPosition) {
        mCurrentCameraPosition = cameraPosition;
    }

    // internal time layer conversion
    private long getDurationNano(long durationMs) {
        return durationMs > 0 ? TimeUnit.NANOSECONDS.convert(durationMs, TimeUnit.MILLISECONDS) : 0;
    }

    //
    // ZOOM
    //

    /**
     * <p>
     * Sets the minimum zoom level the map can be displayed at.
     * </p>
     *
     * @param minZoom The new minimum zoom level.
     */
    @UiThread
    public void setMinZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double minZoom) {
        if ((minZoom < MapboxConstants.MINIMUM_ZOOM) || (minZoom > MapboxConstants.MAXIMUM_ZOOM)) {
            Log.e(MapboxConstants.TAG, "Not setting minZoom, value is in unsupported range: " + minZoom);
            return;
        }
        mMapView.setMinZoom(minZoom);
    }

    /**
     * <p>
     * Gets the maximum zoom level the map can be displayed at.
     * </p>
     *
     * @return The minimum zoom level.
     */
    @UiThread
    public double getMinZoom() {
        return mMapView.getMinZoom();
    }

    /**
     * <p>
     * Sets the maximum zoom level the map can be displayed at.
     * </p>
     *
     * @param maxZoom The new maximum zoom level.
     */
    @UiThread
    public void setMaxZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double maxZoom) {
        if ((maxZoom < MapboxConstants.MINIMUM_ZOOM) || (maxZoom > MapboxConstants.MAXIMUM_ZOOM)) {
            Log.e(MapboxConstants.TAG, "Not setting maxZoom, value is in unsupported range: " + maxZoom);
            return;
        }
        mMapView.setMaxZoom(maxZoom);
    }

    /**
     * <p>
     * Gets the maximum zoom level the map can be displayed at.
     * </p>
     *
     * @return The maximum zoom level.
     */
    @UiThread
    public double getMaxZoom() {
        return mMapView.getMaxZoom();
    }

    //
    // Manual zoom controls
    //

    // used by UiSettings
    void setZoomControlsEnabled(boolean enabled) {
        mMapView.setZoomControlsEnabled(enabled);
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
        return mMapView.isDebugActive();
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
        mMapView.setDebugActive(debugActive);
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
        mMapView.cycleDebugOptions();
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
     * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be sent.
     *
     * @param url The URL of the map style
     * @see Style
     */
    @UiThread
    public void setStyleUrl(@NonNull String url) {
        mStyleUrl = url;
        mMapView.setStyleUrl(url);
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
     * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be sent.
     *
     * @param style The bundled style. Accepts one of the values from {@link Style}.
     * @see Style
     */
    @UiThread
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
        return mStyleUrl;
    }

    //
    // Access token
    //

    /**
     * <p>
     * Sets the current Mapbox access token used to load map styles and tiles.
     * </p>
     * <p>
     * You must set a valid access token before you call {@link MapView#onCreate(Bundle)}
     * or an exception will be thrown.
     * </p>
     * You can use {@link ApiAccess#getToken(Context)} to load an access token from your
     * application's manifest.
     *
     * @param accessToken Your public Mapbox access token.
     * @see MapView#onCreate(Bundle)
     * @see ApiAccess#getToken(Context)
     */
    @UiThread
    public void setAccessToken(@NonNull String accessToken) {
        mMapView.setAccessToken(accessToken);
    }

    /**
     * Returns the current Mapbox access token used to load map styles and tiles.
     *
     * @return The current Mapbox access token.
     */
    @UiThread
    @Nullable
    public String getAccessToken() {
        return mMapView.getAccessToken();
    }

    //
    // Projection
    //

    /**
     * Converts a point in this view's coordinate system to a map coordinate.
     *
     * @param point A point in this view's coordinate system.
     * @return The converted map coordinate.
     */
    @UiThread
    @NonNull
    public LatLng fromScreenLocation(@NonNull PointF point) {
        return mMapView.fromScreenLocation(point);
    }

    /**
     * Converts a map coordinate to a point in this view's coordinate system.
     *
     * @param location A map coordinate.
     * @return The converted point in this view's coordinate system.
     */
    @UiThread
    @NonNull
    public PointF toScreenLocation(@NonNull LatLng location) {
        return mMapView.toScreenLocation(location);
    }

    /**
     * <p>
     * Returns the distance spanned by one pixel at the specified latitude and current zoom level.
     * </p>
     * The distance between pixels decreases as the latitude approaches the poles.
     * This relationship parallels the relationship between longitudinal coordinates at different latitudes.
     *
     * @param latitude The latitude for which to return the value.
     * @return The distance measured in meters.
     */
    @UiThread
    public double getMetersPerPixelAtLatitude(@FloatRange(from = -180, to = 180) double latitude) {
        return mMapView.getMetersPerPixelAtLatitude(latitude);
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
     * @param markerOptions A marker options object that defines how to render the marker.
     * @return The {@code Marker} that was added to the map.
     */
    @UiThread
    @NonNull
    public Marker addMarker(@NonNull MarkerOptions markerOptions) {
        return mMapView.addMarker(markerOptions);
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
    public List<Marker> addMarkers(@NonNull List<MarkerOptions> markerOptionsList) {
        return mMapView.addMarkers(markerOptionsList);
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
        return mMapView.addPolyline(polylineOptions);
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
        return mMapView.addPolylines(polylineOptionsList);
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
        return mMapView.addPolygon(polygonOptions);
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
        return mMapView.addPolygons(polygonOptionsList);
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
     * Removes an annotation from the map.
     *
     * @param annotation The annotation object to remove.
     */
    @UiThread
    public void removeAnnotation(@NonNull Annotation annotation) {
        mMapView.removeAnnotation(annotation);
    }

    /**
     * Removes multiple annotations from the map.
     *
     * @param annotationList A list of annotation objects to remove.
     */
    @UiThread
    public void removeAnnotations(@NonNull List<? extends Annotation> annotationList) {
        mMapView.removeAnnotations(annotationList);
    }

    /**
     * Removes all annotations from the map.
     */
    @UiThread
    public void removeAllAnnotations() {
        mMapView.removeAllAnnotations();
    }

    /**
     * Returns a list of all the annotations on the map.
     *
     * @return A list of all the annotation objects. The returned object is a copy so modifying this
     * list will not update the map.
     */
    @NonNull
    public List<Annotation> getAllAnnotations() {
        return mMapView.getAllAnnotations();
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
            Log.w(MapboxConstants.TAG, "marker was null, so just" +
                    " returning");
            return;
        }

        if (mSelectedMarkers.contains(marker)) {
            return;
        }

        // Need to deselect any currently selected annotation first
        if (!isAllowConcurrentMultipleOpenInfoWindows()) {
            deselectMarkers();
        }

        boolean handledDefaultClick = false;
        if (mOnMarkerClickListener != null) {
            // end developer has provided a custom click listener
            handledDefaultClick = mOnMarkerClickListener.onMarkerClick(marker);
        }

        if (!handledDefaultClick) {
            if (isInfoWindowValidForMarker(marker)) {
                mInfoWindows.add(marker.showInfoWindow(this, mMapView));
            }
        }

        mSelectedMarkers.add(marker);
    }

    /**
     * Deselects any currently selected marker. All markers will have it's info window closed.
     */
    @UiThread
    public void deselectMarkers() {
        if (mSelectedMarkers.isEmpty()) {
            return;
        }

        for (Marker marker : mSelectedMarkers) {
            if (marker.isInfoWindowShown()) {
                marker.hideInfoWindow();
            }
        }

        // Removes all selected markers from the list
        mSelectedMarkers.clear();
    }

    /**
     * Deselects a currently selected marker. The selected marker will have it's info window closed.
     */
    @UiThread
    public void deselectMarker(@NonNull Marker marker) {
        if (!mSelectedMarkers.contains(marker)) {
            return;
        }

        if (marker.isInfoWindowShown()) {
            marker.hideInfoWindow();
        }

        mSelectedMarkers.remove(marker);
    }

    /**
     * Gets the currently selected marker.
     *
     * @return The currently selected marker.
     */
    @UiThread
    @Nullable
    public List<Marker> getSelectedMarkers() {
        return mSelectedMarkers;
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
        mInfoWindowAdapter = infoWindowAdapter;
    }

    /**
     * Gets the callback to be invoked when an info window will be shown.
     *
     * @return The callback to be invoked when an info window will be shown.
     */
    @UiThread
    @Nullable
    public InfoWindowAdapter getInfoWindowAdapter() {
        return mInfoWindowAdapter;
    }

    /**
     * Changes whether the map allows concurrent multiple infowindows to be shown.
     *
     * @param allow If true, map allows concurrent multiple infowindows to be shown.
     */
    @UiThread
    public void setAllowConcurrentMultipleOpenInfoWindows(boolean allow) {
        mAllowConcurrentMultipleInfoWindows = allow;
    }

    /**
     * Returns whether the map allows concurrent multiple infowindows to be shown.
     *
     * @return If true, map allows concurrent multiple infowindows to be shown.
     */
    @UiThread
    public boolean isAllowConcurrentMultipleOpenInfoWindows() {
        return mAllowConcurrentMultipleInfoWindows;
    }

    // used by MapView
    List<InfoWindow> getInfoWindows() {
        return mInfoWindows;
    }

    private boolean isInfoWindowValidForMarker(@NonNull Marker marker) {
        return !TextUtils.isEmpty(marker.getTitle()) || !TextUtils.isEmpty(marker.getSnippet());
    }

    //
    // Map events
    //

    /**
     * Sets a callback that's invoked on every frame rendered to the map view.
     *
     * @param listener The callback that's invoked on every frame rendered to the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnFpsChangedListener(@Nullable OnFpsChangedListener listener) {
        mOnFpsChangedListener = listener;
    }

    // used by MapView
    OnFpsChangedListener getOnFpsChangedListener() {
        return mOnFpsChangedListener;
    }

    /**
     * Sets a callback that's invoked when the map is scrolled.
     *
     * @param listener The callback that's invoked when the map is scrolled.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnScrollListener(@Nullable OnScrollListener listener) {
        mOnScrollListener = listener;
    }

    // used by MapView
    OnScrollListener getOnScrollListener() {
        return mOnScrollListener;
    }

    /**
     * Sets a callback that's invoked when the map is flinged.
     *
     * @param listener The callback that's invoked when the map is flinged.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnFlingListener(@Nullable OnFlingListener listener) {
        mOnFlingListener = listener;
    }

    // used by MapView
    OnFlingListener getOnFlingListener() {
        return mOnFlingListener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on the map view.
     *
     * @param listener The callback that's invoked when the user clicks on the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMapClickListener(@Nullable OnMapClickListener listener) {
        mOnMapClickListener = listener;
    }

    // used  by MapView
    OnMapClickListener getOnMapClickListener() {
        return mOnMapClickListener;
    }

    /**
     * Sets a callback that's invoked when the user long clicks on the map view.
     *
     * @param listener The callback that's invoked when the user long clicks on the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMapLongClickListener(@Nullable OnMapLongClickListener listener) {
        mOnMapLongClickListener = listener;
    }

    // used by MapView
    OnMapLongClickListener getOnMapLongClickListener() {
        return mOnMapLongClickListener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on a marker.
     *
     * @param listener The callback that's invoked when the user clicks on a marker.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMarkerClickListener(@Nullable OnMarkerClickListener listener) {
        mOnMarkerClickListener = listener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on an info window.
     *
     * @param listener The callback that's invoked when the user clicks on an info window.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnInfoWindowClickListener(@Nullable OnInfoWindowClickListener listener) {
        mOnInfoWindowClickListener = listener;
    }

    /**
     * Return the InfoWindow click listener
     *
     * @return Current active InfoWindow Click Listener
     */
    public OnInfoWindowClickListener getOnInfoWindowClickListener() {
        return mOnInfoWindowClickListener;
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
        return mMyLocationEnabled;
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
     * @throws SecurityException if no suitable permission is present
     */
    @UiThread
    @RequiresPermission(anyOf = {
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.ACCESS_FINE_LOCATION})
    public void setMyLocationEnabled(boolean enabled) {
        mMyLocationEnabled = enabled;
        mMapView.setMyLocationEnabled(enabled);
    }

    /**
     * Returns the currently displayed user location, or null if there is no location data available.
     *
     * @return The currently displayed user location.
     */
    @UiThread
    @Nullable
    public Location getMyLocation() {
        return mMapView.getMyLocation();
    }

    /**
     * Sets a callback that's invoked when the the My Location dot
     * (which signifies the user's location) changes location.
     *
     * @param listener The callback that's invoked when the user clicks on a marker.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMyLocationChangeListener(@Nullable MapboxMap.OnMyLocationChangeListener listener) {
        mMapView.setOnMyLocationChangeListener(listener);
    }

    /**
     * <p>
     * Set the current my location tracking mode.
     * </p>
     * <p>
     * Will enable my location if not active.
     * </p>
     * See {@link MyLocationTracking} for different values.
     *
     * @param myLocationTrackingMode The location tracking mode to be used.
     * @throws SecurityException if no suitable permission is present
     * @see MyLocationTracking
     */
    @UiThread
    @RequiresPermission(anyOf = {
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.ACCESS_FINE_LOCATION})
    public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        mMapView.setMyLocationTrackingMode(myLocationTrackingMode);
    }

    /**
     * Returns the current user location tracking mode.
     *
     * @return The current user location tracking mode.
     * One of the values from {@link MyLocationTracking.Mode}.
     * @see MyLocationTracking.Mode
     */
    @UiThread
    @MyLocationTracking.Mode
    public int getMyLocationTrackingMode() {
        return mMapView.getMyLocationTrackingMode();
    }

    /**
     * Sets a callback that's invoked when the location tracking mode changes.
     *
     * @param listener The callback that's invoked when the location tracking mode changes.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMyLocationTrackingModeChangeListener(@Nullable MapboxMap.OnMyLocationTrackingModeChangeListener listener) {
        mOnMyLocationTrackingModeChangeListener = listener;
    }

    // used by MapView
    MapboxMap.OnMyLocationTrackingModeChangeListener getOnMyLocationTrackingModeChangeListener() {
        return mOnMyLocationTrackingModeChangeListener;
    }

    /**
     * <p>
     * Set the current my bearing tracking mode.
     * </p>
     * Shows the direction the user is heading.
     * <p>
     * When location tracking is disabled the direction of {@link UserLocationView}  is rotated
     * When location tracking is enabled the {@link MapView} is rotated based on bearing value.
     * </p>
     * See {@link MyBearingTracking} for different values.
     *
     * @param myBearingTrackingMode The bearing tracking mode to be used.
     * @throws SecurityException if no suitable permission is present
     * @see MyBearingTracking
     */
    @UiThread
    @RequiresPermission(anyOf = {
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.ACCESS_FINE_LOCATION})
    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        mMapView.setMyBearingTrackingMode(myBearingTrackingMode);
    }

    /**
     * Returns the current user bearing tracking mode.
     * See {@link MyBearingTracking} for possible return values.
     *
     * @return the current user bearing tracking mode.
     * @see MyBearingTracking
     */
    @UiThread
    @MyLocationTracking.Mode
    public int getMyBearingTrackingMode() {
        return mMapView.getMyBearingTrackingMode();
    }

    /**
     * Sets a callback that's invoked when the bearing tracking mode changes.
     *
     * @param listener The callback that's invoked when the bearing tracking mode changes.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMyBearingTrackingModeChangeListener(@Nullable OnMyBearingTrackingModeChangeListener listener) {
        mOnMyBearingTrackingModeChangeListener = listener;
    }

    // used by MapView
    OnMyBearingTrackingModeChangeListener getOnMyBearingTrackingModeChangeListener() {
        return mOnMyBearingTrackingModeChangeListener;
    }

    //
    // Compass
    //

    // used by UiSettings
    void setCompassEnabled(boolean compassEnabled) {
        mMapView.setCompassEnabled(compassEnabled);
    }

    // used by UiSettings
    void setCompassGravity(int gravity) {
        mMapView.setCompassGravity(gravity);
    }

    // used by UiSettings
    void setCompassMargins(int left, int top, int right, int bottom) {
        mMapView.setCompassMargins(left, top, right, bottom);
    }

    //
    // Logo
    //

    // used by UiSettings
    void setLogoGravity(int gravity) {
        mMapView.setLogoGravity(gravity);
    }

    // used by UiSettings
    void setLogoMargins(int left, int top, int right, int bottom) {
        mMapView.setLogoMargins(left, top, right, bottom);
    }

    // used by UiSettings
    void setLogoVisibility(int visibility) {
        mMapView.setLogoVisibility(visibility);
    }

    //
    // Attribution
    //

    // used by UiSettings
    void setAttributionGravity(int gravity) {
        mMapView.setAttributionGravity(gravity);
    }

    // used by UiSettings
    void setAttributionMargins(int left, int top, int right, int bottom) {
        mMapView.setAttributionMargins(left, top, right, bottom);
    }

    // used by UiSettings
    void setAttributionVisibility(int visibility) {
        mMapView.setAttributionVisibility(visibility);
    }

    //
    // Custom layer
    //

    @UiThread
    public void addCustomLayer(CustomLayer customLayer, String before) {
        mMapView.addCustomLayer(customLayer, before);
    }

    @UiThread
    public void removeCustomLayer(String id) {
        mMapView.removeCustomLayer(id);
    }

    @UiThread
    public void invalidateCustomLayers() {
        mMapView.invalidateCustomLayers();
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
     * Interface definition for a callback to be invoked on every frame rendered to the map view.
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
        boolean onMarkerClick(@NonNull Marker marker);
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
     * Interface definition for a callback to be invoked when the the My Location dot
     * (which signifies the user's location) changes location.
     *
     * @see MapboxMap#setOnMyLocationChangeListener(OnMyLocationChangeListener)
     */
    public interface OnMyLocationChangeListener {
        /**
         * Called when the location of the My Location dot has changed
         * (be it latitude/longitude, bearing or accuracy).
         *
         * @param location The current location of the My Location dot The type of map change event.
         */
        void onMyLocationChange(@Nullable Location location);
    }

    /**
     * Interface definition for a callback to be invoked when the the My Location tracking mode changes.
     *
     * @see MapboxMap#setMyLocationTrackingMode(int)
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
     * @see MapboxMap#setMyLocationTrackingMode(int)
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
     * A callback interface for reporting when a task is complete or cancelled.
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

}
