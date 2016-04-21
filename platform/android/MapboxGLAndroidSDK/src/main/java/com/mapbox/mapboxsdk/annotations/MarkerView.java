package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.graphics.PointF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.Projection;

public class MarkerView extends FrameLayout {

    private LatLng latLng;
    private Projection projection;
    private Marker marker;

    private float widthOffset;
    private float heightOffset;

    public MarkerView(Context context) {
        super(context);
        init(context);
    }

    public MarkerView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public MarkerView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    private void init(Context context){
        setLayoutParams(new MapView.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
    }

    public void setProjection(Projection projection) {
        this.projection = projection;
    }

    public void setLatLng(LatLng latLng) {
        this.latLng = latLng;
    }

    public void setMarker(Marker marker) {
        this.marker = marker;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        widthOffset = getMeasuredWidth() / 2;
        heightOffset = getMeasuredHeight() / 2;

        Log.v(MapboxConstants.TAG,"Measure "+widthOffset + heightOffset);
    }

    public void update() {
        PointF point = projection.toScreenLocation(latLng);
        setX(point.x - widthOffset);
        setY(point.y - heightOffset);
    }
}
