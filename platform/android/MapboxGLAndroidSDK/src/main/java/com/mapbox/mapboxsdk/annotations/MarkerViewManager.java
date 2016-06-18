package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.graphics.PointF;
import android.os.SystemClock;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.util.Pools;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;
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

    private Map<MarkerView, View> markerViewMap;
    private MapboxMap mapboxMap;
    private MapView mapView;
    private List<MapboxMap.MarkerViewAdapter> markerViewAdapters;
    private long viewMarkerBoundsUpdateTime;
    private MapboxMap.OnMarkerViewClickListener onMarkerViewClickListener;
    private ImageMarkerViewAdapter defaultMarkerViewAdapter;

    /**
     * Creates an instance of MarkerViewManager.
     *
     * @param mapboxMap the MapboxMap associated with the MarkerViewManager
     * @param mapView   the MapView associated with the MarkerViewManager
     */
    public MarkerViewManager(@NonNull MapboxMap mapboxMap, @NonNull MapView mapView) {
        this.mapboxMap = mapboxMap;
        this.markerViewAdapters = new ArrayList<>();
        this.mapView = mapView;
        this.markerViewMap = new HashMap<>();
        this.defaultMarkerViewAdapter = new ImageMarkerViewAdapter(mapView.getContext());
        this.markerViewAdapters.add(defaultMarkerViewAdapter);
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
        View convertView = markerViewMap.get(marker);
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
        View convertView = markerViewMap.get(marker);
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
        View convertView = markerViewMap.get(marker);
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
        for (MarkerView marker : markerViewMap.keySet()) {
            convertView = markerViewMap.get(marker);
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
        for (MarkerView markerView : markerViewMap.keySet()) {
            if (markerView.isFlat()) {
                convertView = markerViewMap.get(markerView);
                if (convertView != null) {
                    markerView.setTilt(tilt);
                    convertView.setRotationX(tilt);
                }
            }
        }
    }

    /**
     *
     */
    public void updateIcon(@NonNull MarkerView markerView) {
        View convertView = markerViewMap.get(markerView);
        if (convertView != null && convertView instanceof ImageView) {
            ((ImageView) convertView).setImageBitmap(markerView.getIcon().getBitmap());
        }
    }

    /**
     * Animate a MarkerView to a deselected state.
     * <p>
     * The {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter#onDeselect(MarkerView, View)} will be called to execute an animation.
     * </p>
     *
     * @param marker the MarkerView to deselect
     */
    public void deselect(@NonNull MarkerView marker) {
        final View convertView = markerViewMap.get(marker);
        if (convertView != null) {
            for (MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
                if (adapter.getMarkerClass().equals(marker.getClass())) {
                    adapter.onDeselect(marker, convertView);
                }
            }
        }
    }

    /**
     * Animate a MarkerView to a selected state.
     *
     * @param marker the MarkerView object to select
     */
    public void select(@NonNull MarkerView marker) {
        final View convertView = markerViewMap.get(marker);
        for (MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
            if (adapter.getMarkerClass().equals(marker.getClass())) {
                select(marker, convertView, adapter);
            }
        }
    }

    /**
     * Animate a MarkerView to a selected state.
     * <p>
     * The {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter#onSelect(MarkerView, View, boolean)} will be called to execute an animation.
     * </p>
     * @param marker the MarkerView object to select
     * @param convertView the View presentation of the MarkerView
     * @param adapter the adapter used to adapt the marker to the convertView
     */
    public void select(@NonNull MarkerView marker, View convertView, MapboxMap.MarkerViewAdapter adapter) {
        if (convertView != null) {
            if (adapter.onSelect(marker, convertView, false)) {
                mapboxMap.selectMarker(marker);
            }
            marker.setSelected(true);
            convertView.bringToFront();
        }
    }

    /**
     * Get view representation from a MarkerView.
     * <p>
     * If marker is not found in current viewport, null is returned.
     * </p>
     *
     * @param marker the marker to get the view for
     * @return the android SDK View object
     */
    @Nullable
    public View getView(MarkerView marker) {
        return markerViewMap.get(marker);
    }

    /**
     * Remove a MarkerView from a map.
     * <p>
     * The {@link MarkerView} will be removed using an alpha animation and related {@link View}
     * will be released to the android.support.v4.util.Pools.SimplePool from the related
     * {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter}. It's possible to remove
     * the {@link MarkerView} from the underlying collection if needed.
     * </p>
     *
     * @param marker the MarkerView to remove
     */
    public void removeMarkerView(MarkerView marker) {
        final View viewHolder = markerViewMap.get(marker);
        if (viewHolder != null && marker != null) {
            for (final MapboxMap.MarkerViewAdapter<?> adapter : markerViewAdapters) {
                if (adapter.getMarkerClass().equals(marker.getClass())) {
                    if (adapter.prepareViewForReuse(marker, viewHolder)) {
                        adapter.releaseView(viewHolder);
                    }
                }
            }
        }
        markerViewMap.remove(marker);
    }

    /**
     * Add a MarkerViewAdapter to the MarkerViewManager.
     * <p>
     * The provided MarkerViewAdapter must use supply a generic subclass of MarkerView.
     * </p>
     *
     * @param markerViewAdapter the MarkerViewAdapter to add
     */
    public void addMarkerViewAdapter(MapboxMap.MarkerViewAdapter markerViewAdapter) {
        if (markerViewAdapter.getMarkerClass().equals(MarkerView.class)) {
            throw new RuntimeException("Providing a custom MarkerViewAdapter requires subclassing MarkerView");
        }

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
            if (currentTime < viewMarkerBoundsUpdateTime) {
                return;
            }
            invalidateViewMarkersInBounds();
            viewMarkerBoundsUpdateTime = currentTime + 250;
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
        Iterator<MarkerView> iterator = markerViewMap.keySet().iterator();
        while (iterator.hasNext()) {
            MarkerView m = iterator.next();
            if (!markers.contains(m)) {
                // remove marker
                convertView = markerViewMap.get(m);
                for (MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
                    if (adapter.getMarkerClass().equals(m.getClass())) {
                        adapter.prepareViewForReuse(m, convertView);
                        adapter.releaseView(convertView);
                        iterator.remove();
                    }
                }
            }
        }

        // introduce new markers
        for (final MarkerView marker : markers) {
            if (!markerViewMap.containsKey(marker)) {
                for (final MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
                    if (adapter.getMarkerClass().equals(marker.getClass())) {
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
                                if (adapter.onSelect(marker, adaptedView, true)) {
                                    mapboxMap.selectMarker(marker);
                                }
                            }

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
                                        select(marker, v, adapter);
                                    }
                                }
                            });

                            markerViewMap.put(marker, adaptedView);
                            if (convertView == null) {
                                mapView.getMarkerViewContainer().addView(adaptedView);
                            }
                        }
                    }
                }
            }
        }
    }

    /**
     * Default MarkerViewAdapter used for base class of MarkerView to adapt a MarkerView to an ImageView
     */
    public static class ImageMarkerViewAdapter extends MapboxMap.MarkerViewAdapter<MarkerView> {

        private LayoutInflater inflater;

        public ImageMarkerViewAdapter(Context context) {
            super(context);
            inflater = LayoutInflater.from(context);
        }

        @Nullable
        @Override
        public View getView(@NonNull MarkerView marker, @Nullable View convertView, @NonNull ViewGroup parent) {
            ViewHolder viewHolder;
            if (convertView == null) {
                viewHolder = new ViewHolder();
                convertView = inflater.inflate(R.layout.view_image_marker, parent, false);
                viewHolder.imageView = (ImageView) convertView.findViewById(R.id.image);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }
            viewHolder.imageView.setImageBitmap(marker.getIcon().getBitmap());
            return convertView;
        }

        private static class ViewHolder {
            ImageView imageView;
        }
    }
}
