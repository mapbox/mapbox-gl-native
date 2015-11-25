package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.PointF;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.views.MapView;

import java.lang.ref.WeakReference;

/**
 * A tooltip view
 */
public class InfoWindow {

    private WeakReference<Marker> mBoundMarker;
    private WeakReference<MapView> mMapView;
    private float mMarkerHeightOffset;
    private boolean mIsVisible;
    protected View mView;

    static int mTitleId = 0;
    static int mDescriptionId = 0;
    static int mSubDescriptionId = 0;
    static int mImageId = 0;

    InfoWindow(int layoutResId, MapView mapView) {
        View view = LayoutInflater.from(mapView.getContext()).inflate(layoutResId, mapView, false);

        if (mTitleId == 0) {
            setResIds(mapView.getContext());
        }

        initialize(view, mapView);
    }

    InfoWindow(View view, MapView mapView) {
        initialize(view, mapView);
    }

    private void initialize(View view, MapView mapView) {
        mMapView = new WeakReference<>(mapView);
        mIsVisible = false;
        mView = view;

        // default behavior: close it when clicking on the tooltip:
        mView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent e) {
                if (e.getAction() == MotionEvent.ACTION_UP) {
                    boolean handledDefaultClick = false;
                    MapView.OnInfoWindowClickListener onInfoWindowClickListener =
                            mMapView.get().getOnInfoWindowClickListener();
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
    InfoWindow open(Marker boundMarker, LatLng position, int offsetX, int offsetY) {
        setBoundMarker(boundMarker);
        mMarkerHeightOffset = offsetY;

        MapView.LayoutParams lp = new MapView.LayoutParams(MapView.LayoutParams.WRAP_CONTENT, MapView.LayoutParams.WRAP_CONTENT);
        mView.measure(View.MeasureSpec.UNSPECIFIED, View.MeasureSpec.UNSPECIFIED);

        // Calculate default Android x,y coordinate
        PointF coords = mMapView.get().toScreenLocation(position);
        float x = coords.x - (mView.getMeasuredWidth() / 2) + offsetX;
        float y = coords.y - mView.getMeasuredHeight() + mMarkerHeightOffset;

        if (mView instanceof InfoWindowView) {
            // only apply repositioning/margin for InfoWindowView
            Resources resources = mMapView.get().getContext().getResources();

            // get right/left popup window
            float rightSideInfowWindow = x + mView.getMeasuredWidth();
            float leftSideInfoWindow = x;

            // get right/left map view
            final float mapRight = mMapView.get().getRight();
            final float mapLeft = mMapView.get().getLeft();

            float marginHorizontal = resources.getDimension(R.dimen.infowindow_margin);
            float tipViewOffset = resources.getDimension(R.dimen.infowindow_tipview_width) / 2;
            float tipViewMarginLeft = mView.getMeasuredWidth() / 2 - tipViewOffset;

            boolean outOfBoundsLeft = false;
            boolean outOfBoundsRight = false;

            // if out of bounds right
            if (rightSideInfowWindow > mapRight) {
                outOfBoundsRight = true;
                x -= rightSideInfowWindow - mapRight;
                tipViewMarginLeft += rightSideInfowWindow - mapRight + tipViewOffset;
                rightSideInfowWindow = x + mView.getMeasuredWidth();
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
            InfoWindowView infoWindowView = (InfoWindowView) mView;
            infoWindowView.setTipViewMarginLeft((int) tipViewMarginLeft);
        }

        // set anchor popupwindowview
        mView.setX(x);
        mView.setY(y);

        close(); //if it was already opened
        mMapView.get().addView(mView, lp);
        mIsVisible = true;
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
            ((ViewGroup) mView.getParent()).removeView(mView);
            setBoundMarker(null);
            onClose();
        }
        return this;
    }

    /**
     * Constructs the view that is displayed when the InfoWindow opens.
     * This retrieves data from overlayItem and shows it in the tooltip.
     *
     * @param overlayItem the tapped overlay item
     */
    void adaptDefaultMarker(Marker overlayItem) {
        String title = overlayItem.getTitle();
        ((TextView) mView.findViewById(mTitleId /*R.id.title*/)).setText(title);
        String snippet = overlayItem.getSnippet();
        ((TextView) mView.findViewById(mDescriptionId /*R.id.description*/)).setText(snippet);

/*
        //handle sub-description, hiding or showing the text view:
        TextView subDescText = (TextView) mView.findViewById(mSubDescriptionId);
        String subDesc = overlayItem.getSubDescription();
        if ("".equals(subDesc)) {
            subDescText.setVisibility(View.GONE);
        } else {
            subDescText.setText(subDesc);
            subDescText.setVisibility(View.VISIBLE);
        }
*/
    }

    private void onClose() {
        mMapView.get().deselectMarker(getBoundMarker());
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

    /**
     * Given a context, set the resource ids for the layout
     * of the InfoWindow.
     *
     * @param context the apps Context
     */
    private static void setResIds(Context context) {
        String packageName = context.getPackageName(); //get application package name
        mTitleId = context.getResources().getIdentifier("id/infowindow_title", null, packageName);
        mDescriptionId =
                context.getResources().getIdentifier("id/infowindow_description", null, packageName);
        mSubDescriptionId = context.getResources()
                .getIdentifier("id/infowindow_subdescription", null, packageName);
        mImageId = context.getResources().getIdentifier("id/infowindow_image", null, packageName);
    }

    public void update() {
        MapView mapView = mMapView.get();
        Marker marker = mBoundMarker.get();
        if (mapView != null && marker != null) {
            PointF pointF = mapView.toScreenLocation(marker.getPosition());
            mView.setX(pointF.x - mView.getWidth() / 2);
            mView.setY(pointF.y - mView.getHeight() + mMarkerHeightOffset);
        }
    }

}
