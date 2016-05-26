package com.mapbox.mapboxsdk.annotations;

import android.graphics.PointF;
import android.os.SystemClock;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.util.Pools;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.utils.AnimatorUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * Interface for interacting with ViewMarkers objects inside of a MapView.
 * <p>
 * This class is responsible for managing a {@link MarkerView} item.
 * </p>
 */
public class MarkerViewManager {

    private Map<MarkerView, View> mMarkerViewMap;
    private MapboxMap mapboxMap;
    private MapView mapView;
    private List<MapboxMap.MarkerViewAdapter> markerViewAdapters;
    private long mViewMarkerBoundsUpdateTime;
    private MapboxMap.OnMarkerViewClickListener onMarkerViewClickListener;

    /**
     * Creates an instance of MarkerViewManager.
     *
     * @param mapboxMap the MapboxMap associated with the MarkerViewManager
     * @param mapView   the MapView associarted with the MarkerViewManager
     */
    public MarkerViewManager(@NonNull MapboxMap mapboxMap, @NonNull MapView mapView) {
        this.mapboxMap = mapboxMap;
        this.markerViewAdapters = new ArrayList<>();
        this.mapView = mapView;
        mMarkerViewMap = new HashMap<>();
    }

    /**
     * Animate a MarkerView to a given rotation.
     * <p>
     * The {@link MarkerView} will be rotated from its current rotation to the given rotation.
     * </p>
     *
     * @param marker   the MarkerView to rotate
     * @param rotation the rotation value
     */
    public void animateRotation(@NonNull MarkerView marker, float rotation) {
        View convertView = mMarkerViewMap.get(marker);
        if (convertView != null) {
            AnimatorUtils.rotate(convertView, rotation);
        }
    }

    /**
     * Animate a MarkerView to a given alpha value.
     * <p>
     * The {@link MarkerView} will be transformed from its current alpha value to the given value.
     * </p>
     *
     * @param marker the MarkerView to change its alpha value
     * @param alpha  the alpha value
     */
    public void animateAlpha(@NonNull MarkerView marker, float alpha) {
        View convertView = mMarkerViewMap.get(marker);
        if (convertView != null) {
            AnimatorUtils.alpha(convertView, alpha);
        }
    }

    /**
     * Animate a MarkerVIew to be visible or invisible
     * <p>
     * The {@link MarkerView} will be made {@link View#VISIBLE} or {@link View#GONE}.
     * </p>
     *
     * @param marker  the MarkerView to change its visibility
     * @param visible the flag indicating if MarkerView is visible
     */
    public void animateVisible(@NonNull MarkerView marker, boolean visible) {
        View convertView = mMarkerViewMap.get(marker);
        if (convertView != null) {
            convertView.setVisibility(visible ? View.VISIBLE : View.GONE);
        }
    }

    /**
     * Updates the position of MarkerViews currently found in the viewport.
     * <p>
     * The collection of {@link MarkerView} will be iterated and each item position will be updated.
     * If an item is View state is not visible and its related flag is set to visible,
     * The {@link MarkerView} will be animated to visible using alpha animation.
     * </p>
     */
    public void update() {
        View convertView;
        for (MarkerView marker : mMarkerViewMap.keySet()) {
            convertView = mMarkerViewMap.get(marker);
            if (convertView != null) {
                PointF point = mapboxMap.getProjection().toScreenLocation(marker.getPosition());
                int x = (int) (marker.getAnchorU() * convertView.getMeasuredWidth());
                int y = (int) (marker.getAnchorV() * convertView.getMeasuredHeight());

                marker.setOffsetX(x);
                marker.setOffsetY(y);

                convertView.setX(point.x - x);
                convertView.setY(point.y - y);

                if (marker.isVisible() && convertView.getVisibility() == View.GONE) {
                    convertView.animate().cancel();
                    convertView.setAlpha(0);
                    AnimatorUtils.alpha(convertView, 1);
                }
            }
        }
    }

    /**
     * Set tilt on every non flat MarkerView currently shown in the Viewport.
     *
     * @param tilt the tilt value
     */
    public void setTilt(float tilt) {
        View convertView;
        for (MarkerView markerView : mMarkerViewMap.keySet()) {
            if (markerView.isFlat()) {
                convertView = mMarkerViewMap.get(markerView);
                if (convertView != null) {
                    markerView.setTilt(tilt);
                    convertView.setRotationX(tilt);
                }
            }
        }
    }

    /**
     * Animate a MarkerView to a deselected state.
     * <p>
     * The {@link MarkerView#getDeselectAnimRes()} will be called to get the related animation.
     * If non are provided, no animation will be started.
     * </p>
     *
     * @param marker the MarkerView to deselect
     */
    public void deselect(@NonNull MarkerView marker) {
        final View convertView = mMarkerViewMap.get(marker);
        if (convertView != null) {
            int deselectAnimatorRes = marker.getDeselectAnimRes();
            if (deselectAnimatorRes != 0) {
                AnimatorUtils.animate(convertView, deselectAnimatorRes);
            }
        }
    }

    /**
     * Remove a MarkerView from a map.
     * <p>
     * The {@link MarkerView} will be removed using an alpha animation and related {@link View}
     * will be released to the {@link android.support.v4.util.Pools.SimplePool} from the related
     * {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter}. It's possible to remove
     * the {@link MarkerView} from the underlying collection if needed.
     * </p>
     *
     * @param marker        the MarkerView to remove
     * @param removeFromMap flag indicating if a MarkerView will be removed from the collection.
     */
    public void removeMarkerView(MarkerView marker, boolean removeFromMap) {
        final View viewHolder = mMarkerViewMap.get(marker);
        if (viewHolder != null && marker != null) {
            for (final MapboxMap.MarkerViewAdapter<?> adapter : markerViewAdapters) {
                if (adapter.getMarkerClass() == marker.getClass()) {

                    // get pool of Views associated to an adapter
                    final Pools.SimplePool<View> viewPool = adapter.getViewReusePool();

                    // cancel ongoing animations
                    viewHolder.animate().cancel();
                    viewHolder.setAlpha(1);
                    AnimatorUtils.alpha(viewHolder, 0, new AnimatorUtils.OnAnimationEndListener() {
                        @Override
                        public void onAnimationEnd() {
                            viewHolder.setVisibility(View.GONE);
                            viewPool.release(viewHolder);
                        }
                    });
                }
            }
        }
        if (removeFromMap) {
            mMarkerViewMap.remove(marker);
        }
    }

    /**
     * Add a MarkerViewAdapter.
     *
     * @param markerViewAdapter the MarkerViewAdapter to add
     */
    public void addMarkerViewAdapter(@Nullable MapboxMap.MarkerViewAdapter markerViewAdapter) {
        if (!markerViewAdapters.contains(markerViewAdapter)) {
            markerViewAdapters.add(markerViewAdapter);
            invalidateViewMarkersInBounds();
        }
    }

    /**
     * Get all MarkerViewAdapters associated with this MarkerViewManager.
     *
     * @return a List of MarkerViewAdapters
     */
    public List<MapboxMap.MarkerViewAdapter> getMarkerViewAdapters() {
        return markerViewAdapters;
    }


    /**
     * Register a callback to be invoked when this view is clicked.
     *
     * @param listener the callback to be invoked
     */
    public void setOnMarkerViewClickListener(@Nullable MapboxMap.OnMarkerViewClickListener listener) {
        onMarkerViewClickListener = listener;
    }

    /**
     * Schedule that ViewMarkers found in the viewport are invalidated.
     * <p>
     * This method is rate limited, and {@link #invalidateViewMarkersInBounds} will only be called
     * once each 250 ms.
     * </p>
     */
    public void scheduleViewMarkerInvalidation() {
        if (!markerViewAdapters.isEmpty()) {
            long currentTime = SystemClock.elapsedRealtime();
            if (currentTime < mViewMarkerBoundsUpdateTime) {
                return;
            }
            invalidateViewMarkersInBounds();
            mViewMarkerBoundsUpdateTime = currentTime + 250;
        }
    }

    /**
     * Invalidate the ViewMarkers found in the viewport.
     * <p>
     * This method will remove any markers that aren't in the viewport any more and will add new
     * ones for each found Marker in the changed viewport.
     * </p>
     */
    public void invalidateViewMarkersInBounds() {
        Projection projection = mapboxMap.getProjection();
        List<MarkerView> markers = mapView.getMarkerViewsInBounds(projection.getVisibleRegion().latLngBounds);
        View convertView;

        // remove old markers
        Iterator<MarkerView> iterator = mMarkerViewMap.keySet().iterator();
        while (iterator.hasNext()) {
            MarkerView m = iterator.next();
            if (!markers.contains(m)) {
                // remove marker
                convertView = mMarkerViewMap.get(m);
                int deselectAnimRes = m.getDeselectAnimRes();
                if (deselectAnimRes != 0) {
                    AnimatorUtils.animate(convertView, deselectAnimRes, 0);
                }
                removeMarkerView(m, false);
                iterator.remove();
            }
        }

        // introduce new markers
        for (final MarkerView marker : markers) {
            if (!mMarkerViewMap.containsKey(marker)) {
                for (final MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
                    if (adapter.getMarkerClass() == marker.getClass()) {
                        convertView = (View) adapter.getViewReusePool().acquire();
                        final View adaptedView = adapter.getView(marker, convertView, mapView);
                        if (adaptedView != null) {

                            // tilt
                            adaptedView.setRotationX(marker.getTilt());

                            // rotation
                            adaptedView.setRotation(marker.getRotation());

                            // alpha
                            adaptedView.setAlpha(marker.getAlpha());

                            // visible
                            adaptedView.setVisibility(marker.isVisible() ? View.VISIBLE : View.GONE);

                            if (mapboxMap.getSelectedMarkers().contains(marker)) {
                                // if a marker to be shown was selected
                                // replay that animation with duration 0
                                int selectAnimRes = marker.getSelectAnimRes();
                                if (selectAnimRes != 0) {
                                    AnimatorUtils.animate(convertView, selectAnimRes, 0);
                                }
                            }

                            final int animSelectRes = marker.getSelectAnimRes();
                            adaptedView.setOnClickListener(new View.OnClickListener() {
                                @Override
                                public void onClick(final View v) {
                                    boolean clickHandled = false;
                                    if (onMarkerViewClickListener != null) {
                                        clickHandled = onMarkerViewClickListener.onMarkerClick(marker, v, adapter);
                                    }

                                    if (!clickHandled) {
                                        // InfoWindow offset
                                        int infoWindowOffsetX = (int) ((adaptedView.getWidth() * marker.getInfoWindowAnchorU()) - marker.getOffsetX());
                                        int infoWindowOffsetY = (int) ((adaptedView.getHeight() * marker.getInfoWindowAnchorV()) - marker.getOffsetY());
                                        marker.setTopOffsetPixels(infoWindowOffsetY);
                                        marker.setRightOffsetPixels(infoWindowOffsetX);

                                        if (animSelectRes != 0) {
                                            AnimatorUtils.animate(v, animSelectRes, new AnimatorUtils.OnAnimationEndListener() {
                                                @Override
                                                public void onAnimationEnd() {
                                                    mapboxMap.selectMarker(marker);
                                                }
                                            });
                                        } else {
                                            mapboxMap.selectMarker(marker);
                                        }
                                    }
                                }
                            });

                            mMarkerViewMap.put(marker, adaptedView);
                            if (convertView == null) {
                                mapView.addView(adaptedView);
                            }
                        }
                    }
                }
            }
        }
    }
}
