package com.mapbox.mapboxsdk.maps.widgets;

import com.google.gson.internal.LinkedTreeMap;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Build;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;

import com.mapbox.mapboxsdk.annotations.MarkerViewManager;

import java.util.Iterator;
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
        View goneView = new View(getContext());
        goneView.setVisibility(View.GONE);
        addView(goneView,0);
    }

    public void setMarkerViewManager(MarkerViewManager markerViewManager) {
        this.markerViewManager = markerViewManager;
    }

    @Override
    protected int getChildDrawingOrder(int childCount, int drawOrder) {


        if (drawOrder > markerViewManager.getMarkers().size()) {
            return 0;
        }

        Iterator<View> iteratorSorted = new TreeMap(markerViewManager.getMarkers()).values().iterator();

        int i = 0;
        while (iteratorSorted.hasNext()) {
            View currentView = iteratorSorted.next();
            if (i == drawOrder) {
                if (currentView.getVisibility() == View.GONE) {
                    return 0;
                }
                return indexOfChild(currentView);
            }
            i++;
        }

        return 0;
    }
}
