package com.mapbox.mapboxsdk.annotations;

import android.content.res.Resources;
import android.graphics.PointF;
import android.support.annotation.LayoutRes;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;

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
    private float mMarkerWidthOffset;
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

        view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MapboxMap mapboxMap = mMapboxMap.get();
                if (mapboxMap != null) {
                    MapboxMap.OnInfoWindowClickListener onInfoWindowClickListener = mapboxMap.getOnInfoWindowClickListener();
                    boolean handledDefaultClick = false;
                    if (onInfoWindowClickListener != null) {
                        handledDefaultClick = onInfoWindowClickListener.onInfoWindowClick(getBoundMarker());
                    }

                    if (!handledDefaultClick) {
                        // default behavior: close it when clicking on the tooltip:
                        close();
                    }
                }
            }
        });

        view.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                MapboxMap mapboxMap = mMapboxMap.get();
                if (mapboxMap != null) {
                    MapboxMap.OnInfoWindowLongClickListener listener = mapboxMap.getOnInfoWindowLongClickListener();
                    if (listener != null) {
                        listener.onInfoWindowLongClick(getBoundMarker());
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
            mMarkerWidthOffset = -offsetX;

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
        MapboxMap mapboxMap = mMapboxMap.get();
        if (mIsVisible && mapboxMap != null) {
            mIsVisible = false;
            View view = mView.get();
            if (view != null && view.getParent() != null) {
                ((ViewGroup) view.getParent()).removeView(view);
            }

            Marker marker = getBoundMarker();
            MapboxMap.OnInfoWindowCloseListener listener = mapboxMap.getOnInfoWindowCloseListener();
            if (listener != null) {
                listener.onInfoWindowClose(marker);
            }

            setBoundMarker(null);
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
        TextView titleTextView = ((TextView) view.findViewById(R.id.infowindow_title));
        if (!TextUtils.isEmpty(title)) {
            titleTextView.setText(title);
            titleTextView.setVisibility(View.VISIBLE);
        } else {
            titleTextView.setVisibility(View.GONE);
        }

        String snippet = overlayItem.getSnippet();
        TextView snippetTextView = ((TextView) view.findViewById(R.id.infowindow_description));
        if (!TextUtils.isEmpty(snippet)) {
            snippetTextView.setText(snippet);
            snippetTextView.setVisibility(View.VISIBLE);
        } else {
            snippetTextView.setVisibility(View.GONE);
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

            if (view instanceof InfoWindowView) {
                view.setX(mCoordinates.x + mViewWidthOffset - mMarkerWidthOffset);
            } else {
                view.setX(mCoordinates.x - (view.getMeasuredWidth() / 2) - mMarkerWidthOffset);
            }
            view.setY(mCoordinates.y + mMarkerHeightOffset);
        }
    }

    public View getView() {
        return mView != null ? mView.get() : null;
    }

    boolean isVisible() {
        return mIsVisible;
    }

}
