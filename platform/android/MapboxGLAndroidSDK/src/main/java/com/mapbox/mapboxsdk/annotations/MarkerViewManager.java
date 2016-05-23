package com.mapbox.mapboxsdk.annotations;

import android.animation.Animator;
import android.animation.AnimatorInflater;
import android.animation.AnimatorListenerAdapter;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.os.SystemClock;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.util.Pools;
import android.support.v4.view.animation.FastOutSlowInInterpolator;
import android.view.View;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class MarkerViewManager {

    private MapboxMap mapboxMap;
    private MapView mapView;
    private Context context;
    private List<MapboxMap.MarkerViewAdapter> markerViewAdapters;
    private MarkerViewTransformer markerViewTransformer;
    private long mViewMarkerBoundsUpdateTime;
    private MapboxMap.OnMarkerViewClickListener onMarkerViewClickListener;
    private Bitmap mViewMarkerBitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888);

    public MarkerViewManager(@NonNull MapboxMap mapboxMap, @NonNull MapView mapView) {
        this.mapboxMap = mapboxMap;
        this.markerViewAdapters = new ArrayList<>();
        this.markerViewTransformer = new MarkerViewTransformer();
        this.mapView = mapView;
        this.context = mapView.getContext().getApplicationContext();
    }

    public void deselect(@NonNull MarkerView marker) {
        final View viewMarker = markerViewTransformer.getMarkerViewMap().get(marker);
        if (viewMarker != null) {
            int deselectAnimatorRes = marker.getDeselectAnimRes();
            if (deselectAnimatorRes != 0) {
                viewMarker.setLayerType(View.LAYER_TYPE_HARDWARE, null);
                Animator animator = AnimatorInflater.loadAnimator(context, deselectAnimatorRes);
                animator.addListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                        super.onAnimationEnd(animation);
                        viewMarker.setLayerType(View.LAYER_TYPE_NONE, null);
                    }
                });
                animator.setTarget(viewMarker);
                animator.start();
            }
        }
    }

    public void removeMarkerView(MarkerView marker, boolean removeFromMap) {
        final View viewHolder = markerViewTransformer.getMarkerViewMap().get(marker);
        if (viewHolder != null && marker != null) {
            for (final MapboxMap.MarkerViewAdapter<?> adapter : markerViewAdapters) {
                if (adapter.getMarkerClass() == marker.getClass()) {

                    // get pool of Views associated to an adapter
                    final Pools.SimplePool<View> viewPool = adapter.getViewReusePool();

                    // cancel ongoing animations
                    viewHolder.animate().cancel();
                    viewHolder.setAlpha(1);

                    // animate alpha
                    viewHolder.animate()
                            .alpha(0)
                            .setDuration(MapboxConstants.ANIMATION_DURATION_SHORT)
                            .setInterpolator(new FastOutSlowInInterpolator())
                            .setListener(new AnimatorListenerAdapter() {

                                @Override
                                public void onAnimationEnd(Animator animation) {
                                    super.onAnimationEnd(animation);
                                    viewHolder.setVisibility(View.GONE);
                                    viewPool.release(viewHolder);
                                }
                            });
                }
            }
        }
        if (removeFromMap) {
            markerViewTransformer.getMarkerViewMap().remove(marker);
        }
    }

    public MarkerViewTransformer getMarkerViewTransformer() {
        return markerViewTransformer;
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
        Map<MarkerView, View> markerView = markerViewTransformer.getMarkerViewMap();
        View convertView;

        // remove old markers
        Iterator<MarkerView> iterator = markerView.keySet().iterator();
        while (iterator.hasNext()) {
            MarkerView m = iterator.next();
            if (!markers.contains(m)) {
                // remove marker
                convertView = markerView.get(m);
                int deselectAnimRes = m.getDeselectAnimRes();
                if (deselectAnimRes != 0) {
                    Animator animator = AnimatorInflater.loadAnimator(context, deselectAnimRes);
                    animator.setDuration(0);
                    animator.setTarget(convertView);
                    animator.start();
                }
                removeMarkerView(m, false);
                iterator.remove();
            }
        }

        // introduce new markers
        for (final MarkerView marker : markers) {
            if (!markerView.containsKey(marker)) {
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
                            adaptedView.setRotationX(marker.getTiltValue());

                            // rotation
                            adaptedView.setRotation(marker.getRotation());

                            if (mapboxMap.getSelectedMarkers().contains(marker)) {
                                // if a marker to be shown was selected
                                // replay that animation with duration 0
                                int selectAnimRes = marker.getSelectAnimRes();
                                if (selectAnimRes != 0) {
                                    Animator animator = AnimatorInflater.loadAnimator(mapView.getContext(), selectAnimRes);
                                    animator.setDuration(0);
                                    animator.setTarget(convertView);
                                    animator.start();
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
                                            v.setLayerType(View.LAYER_TYPE_HARDWARE, null);
                                            Animator animator = AnimatorInflater.loadAnimator(context, animSelectRes);
                                            animator.setTarget(v);
                                            animator.addListener(new AnimatorListenerAdapter() {
                                                @Override
                                                public void onAnimationEnd(Animator animation) {
                                                    super.onAnimationEnd(animation);
                                                    mapboxMap.selectMarker(marker);
                                                    v.setLayerType(View.LAYER_TYPE_NONE, null);
                                                }
                                            });
                                            animator.start();
                                        } else {
                                            mapboxMap.selectMarker(marker);
                                        }
                                    }
                                }
                            });

                            markerView.put(marker, adaptedView);
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
