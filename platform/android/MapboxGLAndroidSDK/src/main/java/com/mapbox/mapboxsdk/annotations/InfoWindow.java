package com.mapbox.mapboxsdk.annotations;

import android.content.res.Resources;
import android.graphics.PointF;
import android.support.annotation.LayoutRes;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;

import java.lang.ref.WeakReference;

/**
 * <p>
 * InfoWindow is a tooltip shown when a {@link Marker} is tapped.
 * </p>
 * <p>
 * This is a UI element placed over a map at a specific geographic location.
 * </p>
 */
public class InfoWindow {

    private WeakReference<Marker> mBoundMarker;
    private WeakReference<MapboxMap> mMapboxMap;
    protected WeakReference<View> mView;

    private float mMarkerHeightOffset;
    private float mViewWidthOffset;
    private PointF mCoordinates;
    private boolean mIsVisible;

    @LayoutRes
    private int mLayoutRes;

    InfoWindow(MapView mapView, int layoutResId, MapboxMap mapboxMap) {
        mLayoutRes = layoutResId;
        View view = LayoutInflater.from(mapView.getContext()).inflate(layoutResId, mapView, false);
        initialize(view, mapboxMap);
    }

    InfoWindow(View view, MapboxMap mapboxMap) {
        initialize(view, mapboxMap);
    }

    private void initialize(View view, MapboxMap mapboxMap) {
        mMapboxMap = new WeakReference<>(mapboxMap);
        mIsVisible = false;
        mView = new WeakReference<>(view);

        // default behavior: close it when clicking on the tooltip:
        view.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent e) {
                if (e.getAction() == MotionEvent.ACTION_UP) {
                    boolean handledDefaultClick = false;
                    MapboxMap.OnInfoWindowClickListener onInfoWindowClickListener =
                            mMapboxMap.get().getOnInfoWindowClickListener();
                    if (onInfoWindowClickListener != null) {
                        handledDefaultClick = onInfoWindowClickListener.onMarkerClick(getBoundMarker());
                    }

                    if (!handledDefaultClick) {
                        close();
                    }
                }
                return true;
            }
        });
    }


    /**
     * open the window at the specified position.
     *
     * @param boundMarker the marker on which is hooked the view
     * @param position    to place the window on the map
     * @param offsetX     (&offsetY) the offset of the view to the position, in pixels.
     *                    This allows to offset the view from the object position.
     * @return this infowindow
     */
    InfoWindow open(MapView mapView, Marker boundMarker, LatLng position, int offsetX, int offsetY) {
        setBoundMarker(boundMarker);

        MapView.LayoutParams lp = new MapView.LayoutParams(MapView.LayoutParams.WRAP_CONTENT, MapView.LayoutParams.WRAP_CONTENT);

        MapboxMap mapboxMap = mMapboxMap.get();
        View view = mView.get();
        if (view != null && mapboxMap != null) {
            view.measure(View.MeasureSpec.UNSPECIFIED, View.MeasureSpec.UNSPECIFIED);

            // Calculate y-offset for update method
            mMarkerHeightOffset = -view.getMeasuredHeight() + offsetY;

            // Calculate default Android x,y coordinate

            mCoordinates = mapboxMap.getProjection().toScreenLocation(position);
            float x = mCoordinates.x - (view.getMeasuredWidth() / 2) + offsetX;
            float y = mCoordinates.y - view.getMeasuredHeight() + offsetY;

            if (view instanceof InfoWindowView) {
                // only apply repositioning/margin for InfoWindowView
                Resources resources = mapView.getContext().getResources();

                // get right/left popup window
                float rightSideInfowWindow = x + view.getMeasuredWidth();
                float leftSideInfoWindow = x;

                // get right/left map view
                final float mapRight = mapView.getRight();
                final float mapLeft = mapView.getLeft();

                float marginHorizontal = resources.getDimension(R.dimen.infowindow_margin);
                float tipViewOffset = resources.getDimension(R.dimen.infowindow_tipview_width) / 2;
                float tipViewMarginLeft = view.getMeasuredWidth() / 2 - tipViewOffset;

                boolean outOfBoundsLeft = false;
                boolean outOfBoundsRight = false;

                // if out of bounds right
                if (rightSideInfowWindow > mapRight) {
                    outOfBoundsRight = true;
                    x -= rightSideInfowWindow - mapRight;
                    tipViewMarginLeft += rightSideInfowWindow - mapRight + tipViewOffset;
                    rightSideInfowWindow = x + view.getMeasuredWidth();
                }

                // fit screen left
                if (leftSideInfoWindow < mapLeft) {
                    outOfBoundsLeft = true;
                    x += mapLeft - leftSideInfoWindow;
                    tipViewMarginLeft -= mapLeft - leftSideInfoWindow + tipViewOffset;
                    leftSideInfoWindow = x;
                }

                // Add margin right
                if (outOfBoundsRight && mapRight - rightSideInfowWindow < marginHorizontal) {
                    x -= marginHorizontal - (mapRight - rightSideInfowWindow);
                    tipViewMarginLeft += marginHorizontal - (mapRight - rightSideInfowWindow) - tipViewOffset;
                    leftSideInfoWindow = x;
                }

                // Add margin left
                if (outOfBoundsLeft && leftSideInfoWindow - mapLeft < marginHorizontal) {
                    x += marginHorizontal - (leftSideInfoWindow - mapLeft);
                    tipViewMarginLeft -= (marginHorizontal - (leftSideInfoWindow - mapLeft)) - tipViewOffset;
                }

                // Adjust tipView
                InfoWindowView infoWindowView = (InfoWindowView) view;
                infoWindowView.setTipViewMarginLeft((int) tipViewMarginLeft);
            }

            // set anchor popupwindowview
            view.setX(x);
            view.setY(y);

            // Calculate x-offset for update method
            mViewWidthOffset = x - mCoordinates.x - offsetX;

            close(); //if it was already opened
            mapView.addView(view, lp);
            mIsVisible = true;
        }
        return this;
    }

    /**
     * Close this InfoWindow if it is visible, otherwise don't do anything.
     *
     * @return this info window
     */
    InfoWindow close() {
        if (mIsVisible) {
            mIsVisible = false;
            View view = mView.get();
            if (view != null) {
                ((ViewGroup) view.getParent()).removeView(view);
                setBoundMarker(null);
                onClose();
            }
        }
        return this;
    }

    /**
     * Constructs the view that is displayed when the InfoWindow opens.
     * This retrieves data from overlayItem and shows it in the tooltip.
     *
     * @param overlayItem the tapped overlay item
     */
    void adaptDefaultMarker(Marker overlayItem, MapboxMap mapboxMap, MapView mapView) {
        View view = mView.get();
        if (view == null) {
            view = LayoutInflater.from(mapView.getContext()).inflate(mLayoutRes, mapView, false);
            initialize(view, mapboxMap);
        }
        mMapboxMap = new WeakReference<>(mapboxMap);
        String title = overlayItem.getTitle();
        ((TextView) view.findViewById(R.id.infowindow_title)).setText(title);
        String snippet = overlayItem.getSnippet();
        ((TextView) view.findViewById(R.id.infowindow_description)).setText(snippet);
    }

    private void onClose() {
        MapboxMap mapboxMap = mMapboxMap.get();
        if (mapboxMap != null) {
            mapboxMap.deselectMarker(getBoundMarker());
        }
    }

    InfoWindow setBoundMarker(Marker boundMarker) {
        mBoundMarker = new WeakReference<>(boundMarker);
        return this;
    }

    Marker getBoundMarker() {
        if (mBoundMarker == null) {
            return null;
        }
        return mBoundMarker.get();
    }

    public void update() {
        MapboxMap mapboxMap = mMapboxMap.get();
        Marker marker = mBoundMarker.get();
        View view = mView.get();
        if (mapboxMap != null && marker != null && view != null) {
            mCoordinates = mapboxMap.getProjection().toScreenLocation(marker.getPosition());
            view.setX(mCoordinates.x + mViewWidthOffset);
            view.setY(mCoordinates.y + mMarkerHeightOffset);
        }
    }

}
