package com.mapbox.mapboxsdk.annotations;

import android.graphics.Point;
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

public class MarkerViewManager {

    private Map<MarkerView, View> mMarkerViewMap;
    private MapboxMap mapboxMap;
    private MapView mapView;
    private List<MapboxMap.MarkerViewAdapter> markerViewAdapters;
    private long mViewMarkerBoundsUpdateTime;
    private MapboxMap.OnMarkerViewClickListener onMarkerViewClickListener;

    public MarkerViewManager(@NonNull MapboxMap mapboxMap, @NonNull MapView mapView) {
        this.mapboxMap = mapboxMap;
        this.markerViewAdapters = new ArrayList<>();
        this.mapView = mapView;
        mMarkerViewMap = new HashMap<>();
    }

    public void animateRotation(@NonNull MarkerView marker, float rotation) {
        View convertView = mMarkerViewMap.get(marker);
        if (convertView != null) {
            AnimatorUtils.rotate(convertView, rotation);
        }
    }

    public void animateAlpha(@NonNull MarkerView marker, float alpha) {
        View convertView = mMarkerViewMap.get(marker);
        if (convertView != null) {
            AnimatorUtils.alpha(convertView, alpha);
        }
    }

    public void update() {
        View convertView;
        for (MarkerView marker : mMarkerViewMap.keySet()) {
            convertView = mMarkerViewMap.get(marker);
            if (convertView != null) {
                PointF point = mapboxMap.getProjection().toScreenLocation(marker.getPosition());
                convertView.setX(point.x - (convertView.getMeasuredWidth() / 2));
                convertView.setY(point.y - (convertView.getMeasuredHeight() / 2));
                if (convertView.getVisibility() == View.GONE) {
                    convertView.animate().cancel();
                    convertView.setAlpha(0);
                    AnimatorUtils.alpha(convertView, 1);
                }
            }
        }
    }

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

    public void deselect(@NonNull MarkerView marker) {
        final View convertView = mMarkerViewMap.get(marker);
        if (convertView != null) {
            int deselectAnimatorRes = marker.getDeselectAnimRes();
            if (deselectAnimatorRes != 0) {
                AnimatorUtils.animate(convertView, deselectAnimatorRes);
            }
        }
    }

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

    public void addMarkerViewAdapter(@Nullable MapboxMap.MarkerViewAdapter markerViewAdapter) {
        if (!markerViewAdapters.contains(markerViewAdapter)) {
            markerViewAdapters.add(markerViewAdapter);
            invalidateViewMarkersInBounds();
        }
    }

    public void setOnMarkerViewClickListener(@Nullable MapboxMap.OnMarkerViewClickListener listener) {
        onMarkerViewClickListener = listener;
    }

    public List<MapboxMap.MarkerViewAdapter> getMarkerViewAdapters() {
        return markerViewAdapters;
    }

    public void invalidateViewMarkers() {
        if (!markerViewAdapters.isEmpty()) {
            long currentTime = SystemClock.elapsedRealtime();
            if (currentTime < mViewMarkerBoundsUpdateTime) {
                return;
            }
            invalidateViewMarkersInBounds();
            mViewMarkerBoundsUpdateTime = currentTime + 250;
        }
    }

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
                        View adaptedView = adapter.getView(marker, convertView, mapView);

                        // InfoWindow offset
                        Point infoWindowOffset = marker.getInfoWindowOffset();
                        marker.setTopOffsetPixels(-infoWindowOffset.y);
                        marker.setRightOffsetPixels(infoWindowOffset.x);

                        if (adaptedView != null) {

                            // tilt
                            adaptedView.setRotationX(marker.getTilt());

                            // rotation
                            adaptedView.setRotation(marker.getRotation());

                            // alpha
                            adaptedView.setAlpha(marker.getAlpha());

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
