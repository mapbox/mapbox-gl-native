package com.mapbox.mapboxsdk.maps;

import android.location.Location;
import android.os.SystemClock;
import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.util.LongSparseArray;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Icon;
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

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

/**
 * The general class to interact with in the Android Mapbox SDK. It exposes the entry point for all
 * methods related to the MapView. You cannot instantiate {@link MapboxMap} object directly, rather,
 * you must obtain one from the getMapAsync() method on a MapFragment or MapView that you have
 * added to your application.
 * <p>
 * Note: Similar to a View object, a GoogleMap should only be read and modified from the main thread.
 * </p>
 */
public class MapboxMap {

    private MapView mMapView;
    private UiSettings mUiSettings;
    private TrackingSettings mTrackingSettings;
    private Projection mProjection;
    private CameraPosition mCameraPosition;
    private boolean mInvalidCameraPosition;
    private LongSparseArray<Annotation> mAnnotations;
    private List<Marker> mSelectedMarkers;
    private List<InfoWindow> mInfoWindows;
    private MapboxMap.InfoWindowAdapter mInfoWindowAdapter;

    private boolean mMyLocationEnabled;
    private boolean mAllowConcurrentMultipleInfoWindows;

    private MapboxMap.OnMapClickListener mOnMapClickListener;
    private MapboxMap.OnMapLongClickListener mOnMapLongClickListener;
    private MapboxMap.OnMarkerClickListener mOnMarkerClickListener;
    private MapboxMap.OnInfoWindowClickListener mOnInfoWindowClickListener;
    private MapboxMap.OnInfoWindowLongClickListener mOnInfoWindowLongClickListener;
    private MapboxMap.OnInfoWindowCloseListener mOnInfoWindowCloseListener;
    private MapboxMap.OnFlingListener mOnFlingListener;
    private MapboxMap.OnScrollListener mOnScrollListener;
    private MapboxMap.OnMyLocationTrackingModeChangeListener mOnMyLocationTrackingModeChangeListener;
    private MapboxMap.OnMyBearingTrackingModeChangeListener mOnMyBearingTrackingModeChangeListener;
    private MapboxMap.OnFpsChangedListener mOnFpsChangedListener;
    private MapboxMap.OnCameraChangeListener mOnCameraChangeListener;

    private double mMaxZoomLevel = -1;
    private double mMinZoomLevel = -1;

    MapboxMap(@NonNull MapView mapView) {
        mMapView = mapView;
        mMapView.addOnMapChangedListener(new MapChangeCameraPositionListener());
        mUiSettings = new UiSettings(mapView);
        mTrackingSettings = new TrackingSettings(mMapView, mUiSettings);
        mProjection = new Projection(mapView);
        mAnnotations = new LongSparseArray<>();
        mSelectedMarkers = new ArrayList<>();
        mInfoWindows = new ArrayList<>();
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
    @UiThread
    public void setMinZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double minZoom) {
        if ((minZoom < MapboxConstants.MINIMUM_ZOOM) || (minZoom > MapboxConstants.MAXIMUM_ZOOM)) {
            Log.e(MapboxConstants.TAG, "Not setting minZoom, value is in unsupported range: " + minZoom);
            return;
        }
        mMinZoomLevel = minZoom;
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
        if (mMinZoomLevel == -1) {
            return mMinZoomLevel = mMapView.getMinZoom();
        }
        return mMinZoomLevel;
    }

    //
    // MaxZoom
    //

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
        mMaxZoomLevel = maxZoom;
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
        if (mMaxZoomLevel == -1) {
            return mMaxZoomLevel = mMapView.getMaxZoom();
        }
        return mMaxZoomLevel;
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
    // TrackingSettings
    //

    /**
     * Gets the tracking interface settings for the map.
     *
     * @return
     */
    public TrackingSettings getTrackingSettings() {
        return mTrackingSettings;
    }

    //
    // Projection
    //

    /**
     * Get the Projection object that you can use to convert between screen coordinates and latitude/longitude coordinates.
     */
    public Projection getProjection() {
        return mProjection;
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
        if (mInvalidCameraPosition) {
            invalidateCameraPosition();
        }
        return mCameraPosition;
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
        moveCamera(update, null);
    }

    /**
     * Repositions the camera according to the instructions defined in the update.
     * The move is instantaneous, and a subsequent getCameraPosition() will reflect the new position.
     * See CameraUpdateFactory for a set of updates.
     *
     * @param update The change that should be applied to the camera.
     */
    @UiThread
    public final void moveCamera(CameraUpdate update, MapboxMap.CancelableCallback callback) {
        mCameraPosition = update.getCameraPosition(this);
        mMapView.jumpTo(mCameraPosition.bearing, mCameraPosition.target, mCameraPosition.tilt, mCameraPosition.zoom);
        if (callback != null) {
            callback.onFinish();
        }
        invalidateCameraPosition();
    }

    /**
     * Ease the map according to the update with an animation over a specified duration, and calls an optional callback on completion. See CameraUpdateFactory for a set of updates.
     * If getCameraPosition() is called during the animation, it will return the current location of the camera in flight.
     *
     * @param update The change that should be applied to the camera.
     */
    @UiThread
    public final void easeCamera(CameraUpdate update) {
        easeCamera(update, MapboxConstants.ANIMATION_DURATION);
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
        mCameraPosition = update.getCameraPosition(this);
        mMapView.easeTo(mCameraPosition.bearing, mCameraPosition.target, getDurationNano(durationMs), mCameraPosition.tilt, mCameraPosition.zoom, new CancelableCallback() {
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
        mCameraPosition = update.getCameraPosition(this);
        mMapView.flyTo(mCameraPosition.bearing, mCameraPosition.target, getDurationNano(durationMs), mCameraPosition.tilt, mCameraPosition.zoom, new CancelableCallback() {
            @Override
            public void onCancel() {
                if (callback != null) {
                    callback.onCancel();
                }
                invalidateCameraPosition();
            }

            @Override
            public void onFinish() {
                if (mOnCameraChangeListener != null) {
                    mOnCameraChangeListener.onCameraChange(mCameraPosition);
                }

                if (callback != null) {
                    callback.onFinish();
                }
                invalidateCameraPosition();
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
        mInvalidCameraPosition = false;

        CameraPosition cameraPosition = mMapView.invalidateCameraPosition();
        if (cameraPosition != null) {
            mCameraPosition = cameraPosition;
        }

        if (mOnCameraChangeListener != null) {
            mOnCameraChangeListener.onCameraChange(mCameraPosition);
        }
    }

    //
    //  Reset North
    //

    /**
     * Resets the map view to face north.
     */
    public void resetNorth() {
        mMapView.resetNorth();
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
        return mMapView.getStyleUrl();
    }

    //
    // Access token
    //

    /**
     * <p>
     * Sets the current Mapbox access token used to load map styles and tiles.
     * </p>
     *
     * @param accessToken Your public Mapbox access token.
     * @see MapView#setAccessToken(String)
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
        long id = mMapView.addMarker(marker);
        marker.setMapboxMap(this);
        marker.setId(id);
        mAnnotations.put(id, marker);
        return marker;
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
        int count = markerOptionsList.size();
        List<Marker> markers = new ArrayList<>(count);
        if (count > 0) {
            MarkerOptions markerOptions;
            Marker marker;
            for (int i = 0; i < count; i++) {
                markerOptions = markerOptionsList.get(i);
                marker = prepareMarker(markerOptions);
                markers.add(marker);
            }

            if (markers.size() > 0) {
                long[] ids = mMapView.addMarkers(markers);

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
                        mAnnotations.put(id, m);
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
        mMapView.updateMarker(updatedMarker);

        int index = mAnnotations.indexOfKey(updatedMarker.getId());
        if (index > -1) {
            mAnnotations.setValueAt(index, updatedMarker);
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
            long id = mMapView.addPolyline(polyline);
            polyline.setMapboxMap(this);
            polyline.setId(id);
            mAnnotations.put(id, polyline);
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

        if(count>0) {
            for (PolylineOptions options : polylineOptionsList) {
                polyline = options.getPolyline();
                if (!polyline.getPoints().isEmpty()) {
                    polylines.add(polyline);
                }
            }

            long[] ids = mMapView.addPolylines(polylines);

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
                    mAnnotations.put(id, p);
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
            long id = mMapView.addPolygon(polygon);
            polygon.setId(id);
            polygon.setMapboxMap(this);
            mAnnotations.put(id, polygon);
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
        if(count>0) {
            for (PolygonOptions polygonOptions : polygonOptionsList) {
                polygon = polygonOptions.getPolygon();
                if (!polygon.getPoints().isEmpty()) {
                    polygons.add(polygon);
                }
            }

            long[] ids = mMapView.addPolygons(polygons);

            // if unit tests or polygons correcly added to map
            if(ids==null || ids.length==polygons.size()) {
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
                    mAnnotations.put(id, polygon);
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
            ((Marker) annotation).hideInfoWindow();
        }
        long id = annotation.getId();
        mMapView.removeAnnotation(id);
        mAnnotations.remove(id);
    }

    /**
     * Removes an annotation from the map
     *
     * @param id The identifier associated to the annotation to be removed
     */
    @UiThread
    public void removeAnnotation(long id) {
        mMapView.removeAnnotation(id);
        mAnnotations.remove(id);
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
                ((Marker) annotation).hideInfoWindow();
            }
            ids[i] = annotationList.get(i).getId();
        }
        mMapView.removeAnnotations(ids);
        for (long id : ids) {
            mAnnotations.remove(id);
        }
    }

    /**
     * Removes all annotations from the map.
     */
    @UiThread
    public void removeAnnotations() {
        Annotation annotation;
        int count = mAnnotations.size();
        long[] ids = new long[count];
        for (int i = 0; i < count; i++) {
            ids[i] = mAnnotations.keyAt(i);
            annotation = mAnnotations.get(ids[i]);
            if (annotation instanceof Marker) {
                ((Marker) annotation).hideInfoWindow();
            }
        }
        mMapView.removeAnnotations(ids);
        mAnnotations.clear();
    }

    /**
     * Return a annotation based on its id.
     *
     * @return An annotation with a matched id, null is returned if no match was found.
     */
    @UiThread
    @Nullable
    public Annotation getAnnotation(long id) {
        return mAnnotations.get(id);
    }

    /**
     * Returns a list of all the annotations on the map.
     *
     * @return A list of all the annotation objects. The returned object is a copy so modifying this
     * list will not update the map.
     */
    @NonNull
    public List<Annotation> getAnnotations() {
        List<Annotation> annotations = new ArrayList<>();
        for (int i = 0; i < mAnnotations.size(); i++) {
            annotations.add(mAnnotations.get(mAnnotations.keyAt(i)));
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
        for (int i = 0; i < mAnnotations.size(); i++) {
            annotation = mAnnotations.get(mAnnotations.keyAt(i));
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
        for (int i = 0; i < mAnnotations.size(); i++) {
            annotation = mAnnotations.get(mAnnotations.keyAt(i));
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
        for (int i = 0; i < mAnnotations.size(); i++) {
            annotation = mAnnotations.get(mAnnotations.keyAt(i));
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
            if (isInfoWindowValidForMarker(marker) || getInfoWindowAdapter() != null) {
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
    public List<Marker> getSelectedMarkers() {
        return mSelectedMarkers;
    }

    private Marker prepareMarker(BaseMarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Icon icon = mMapView.loadIconForMarker(marker);
        marker.setTopOffsetPixels(mMapView.getTopOffsetPixelsForIcon(icon));
        return marker;
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
    // Padding
    //

    /**
     * Sets the distance from the edges of the map view’s frame to the edges of the map
     * view’s logical viewport.
     * <p/>
     * When the value of this property is equal to {0,0,0,0}, viewport
     * properties such as `centerCoordinate` assume a viewport that matches the map
     * view’s frame. Otherwise, those properties are inset, excluding part of the
     * frame from the viewport. For instance, if the only the top edge is inset, the
     * map center is effectively shifted downward.
     *
     * @param left   The left margin in pixels.
     * @param top    The top margin in pixels.
     * @param right  The right margin in pixels.
     * @param bottom The bottom margin in pixels.
     */
    public void setPadding(int left, int top, int right, int bottom) {
        mMapView.setContentPadding(left, top, right, bottom);
        mUiSettings.invalidate();

        moveCamera(CameraUpdateFactory.newCameraPosition(new CameraPosition.Builder(mCameraPosition).build()));
    }

    /**
     * Returns the current configured content padding on map view.
     *
     * @return An array with length 4 in the LTRB order.
     */
    public int[] getPadding() {
        return new int[]{mMapView.getContentPaddingLeft(),
                mMapView.getContentPaddingTop(),
                mMapView.getContentPaddingRight(),
                mMapView.getContentPaddingBottom()};
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
        mOnCameraChangeListener = listener;
    }

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
    @UiThread
    public OnInfoWindowClickListener getOnInfoWindowClickListener() {
        return mOnInfoWindowClickListener;
    }

    /**
     * Sets a callback that's invoked when a marker's info window is long pressed.
     *
     * @param listener The callback that's invoked when a marker's info window is long pressed. To unset the callback, use null.
     */
    @UiThread
    public void setOnInfoWindowLongClickListener(@Nullable OnInfoWindowLongClickListener listener) {
        mOnInfoWindowLongClickListener = listener;
    }

    /**
     * Return the InfoWindow long click listener
     *
     * @return Current active InfoWindow long Click Listener
     */
    public OnInfoWindowLongClickListener getOnInfoWindowLongClickListener() {
        return mOnInfoWindowLongClickListener;
    }

    public void setOnInfoWindowCloseListener(@Nullable OnInfoWindowCloseListener listener) {
        mOnInfoWindowCloseListener = listener;
    }

    /**
     * Return the InfoWindow close listener
     *
     * @return Current active InfoWindow Close Listener
     */
    @UiThread
    public OnInfoWindowCloseListener getOnInfoWindowCloseListener() {
        return mOnInfoWindowCloseListener;
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
     */
    @UiThread
    public void setMyLocationEnabled(boolean enabled) {
        if (!mMapView.isPermissionsAccepted()) {
            Log.e(MapboxConstants.TAG, "Could not activate user location tracking: " +
                    "user did not accept the permission or permissions were not requested.");
            return;
        }
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
     * Sets a callback that's invoked when the the My Location view
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
    // Custom layer
    //

    /**
     * Do not use this method, experimental feature.
     */
    @UiThread
    public void addCustomLayer(CustomLayer customLayer, String before) {
        mMapView.addCustomLayer(customLayer, before);
    }

    /**
     * Do not use this method, experimental feature.
     */
    @UiThread
    public void removeCustomLayer(String id) {
        mMapView.removeCustomLayer(id);
    }

    /**
     * Do not use this method, experimental feature.
     */
    @UiThread
    public void invalidateCustomLayers() {
        mMapView.invalidateCustomLayers();
    }

    MapView getMapView() {
        return mMapView;
    }

    //
    // Invalidate
    //

    /**
     * Triggers an invalidation of the map view.
     */
    public void invalidate() {
        mMapView.update();
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

    private class MapChangeCameraPositionListener implements MapView.OnMapChangedListener {

        private static final long UPDATE_RATE_MS = 400;
        private long mPreviousUpdateTimestamp = 0;

        @Override
        public void onMapChanged(@MapView.MapChange int change) {
            if (change >= MapView.REGION_WILL_CHANGE && change <= MapView.REGION_DID_CHANGE_ANIMATED) {
                mInvalidCameraPosition = true;
                long currentTime = SystemClock.elapsedRealtime();
                if (currentTime < mPreviousUpdateTimestamp) {
                    return;
                }
                invalidateCameraPosition();
                mPreviousUpdateTimestamp = currentTime + UPDATE_RATE_MS;
            }
        }
    }
}
