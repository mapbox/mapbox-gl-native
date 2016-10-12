package com.mapbox.mapboxsdk.view;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.FrameLayout;

import com.mapbox.mapboxsdk.maps.MapView;


/**
 * Created by walczewski on 19.09.2016.
 */
public class TouchDispachableFrameLayout extends FrameLayout {
    public TouchDispachableFrameLayout(Context context) {
        super(context);
    }

    public TouchDispachableFrameLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public TouchDispachableFrameLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public TouchDispachableFrameLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {


        final boolean childResult = super.dispatchTouchEvent(ev);

        if(childResult)  {
            mapView.onTouchEvent(ev);
        }

        return childResult;
    }

    private MapView mapView;

    public void setMapView(MapView mapView) {
        this.mapView = mapView;
    }
}
