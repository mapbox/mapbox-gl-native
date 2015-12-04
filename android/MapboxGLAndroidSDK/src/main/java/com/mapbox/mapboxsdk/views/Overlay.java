package com.mapbox.mapboxsdk.views;

import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.view.MotionEvent;

import com.mapbox.mapboxsdk.geometry.BoundingBox;
import com.mapbox.mapboxsdk.geometry.LatLng;

public interface Overlay {

    /**
     * Perform the overlay draw here.
     *
     * @param mapView   the map view.
     * @param canvas    a canvas for you to draw on.
     * @param wgsBounds bounds of the overlay in latitude, longitude
     * @param wgsCenter center of the overlay in latitude, longitude
     * @param bearing   the map bearing / rotation degrees
     * @param zoom      the map zoom level
     */
    void onOverlayDraw(final MapView mapView, final Canvas canvas,
                       final BoundingBox wgsBounds, final LatLng wgsCenter,
                       float bearing, float zoom);

    void onOverlayTouchEvent(final MotionEvent event);

    /**
     * Called when the overlay is attached to the MapView view hierarchy.
     */
    void onOverlayAttached(final MapView mapView);

    /**
     * Called when the overlay is detached to the MapView view hierarchy.
     */
    void onOverlayDetached();

    /**
     * A single tap event occurred.
     *
     * @param pressPosition the position of the press.
     */
    void onOverlaySingleTapConfirmed(LatLng pressPosition);

    /**
     * A long press event occurred.
     *
     * @param pressPosition the position of the press.
     */
    void onOverlayLongPress(LatLng pressPosition);

    /**
     * Let us know if you'd like to receive the onOverlayDraw() callback.
     *
     * @return true to get the
     * {@link Overlay#onOverlayDraw(MapView, Canvas, RectF, PointF, float, float)} callback.
     */
    boolean isOverlayDrawEnabled();

    /**
     * Called when the {@link MapView} size changes
     * @param mapPixelBounds the pixel bounds of the view
     */
    void onMapViewPixelSizeChanged(Rect mapPixelBounds);
}
