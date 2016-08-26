package com.mapbox.mapboxsdk.maps.widgets;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;

import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.constants.MapboxConstants;

import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

public class MarkerViewLayout extends FrameLayout {

    private MarkerViewManager markerViewManager;

    public MarkerViewLayout(Context context) {
        super(context);
        init();
    }

    public MarkerViewLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public MarkerViewLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public MarkerViewLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init();
    }

    private void init() {
        setChildrenDrawingOrderEnabled(true);
    }

    public void setMarkerViewManager(MarkerViewManager markerViewManager) {
        this.markerViewManager = markerViewManager;
    }

    @Override
    protected int getChildDrawingOrder(int childCount, int index) {
        if (markerViewManager != null) {
            HashMap<MarkerView,View> markerViewMap = markerViewManager.getMarkers();

            assert markerViewMap.keySet().size() == childCount;

            Iterator<MarkerView> iterator = markerViewMap.keySet().iterator();
            while (iterator.hasNext()) {
                MarkerView markerView = iterator.next();
                Log.v(MapboxConstants.TAG, "Z index markers" + markerView.getzIndex()+ "MarkerView get class "+markerView.getClass().getSimpleName());
            }
            Log.e(MapboxConstants.TAG, "-------------------------");
            TreeMap<MarkerView, View> sortedMarkerViewMap = new TreeMap<>(markerViewMap);
            Iterator<MarkerView> iteratorSorted = sortedMarkerViewMap.keySet().iterator();

            while (iteratorSorted.hasNext()) {
                MarkerView markerView = iteratorSorted.next();
                Log.v(MapboxConstants.TAG, "Z index markers" + markerView.getzIndex()+ "MarkerView get class "+markerView.getClass().getSimpleName());
            }

            Log.v(MapboxConstants.TAG, "getChildDrawingOrder: childcount  " + childCount + "- index " + "i");
            Log.v(MapboxConstants.TAG, "getChildDrawingOrder: childcount  " + markerViewManager.getMarkers() + "- index " + "i");

            // TODO insert correct index
            return index;
        }
        return index;
    }
}
