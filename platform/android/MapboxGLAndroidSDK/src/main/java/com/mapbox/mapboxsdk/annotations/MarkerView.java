package com.mapbox.mapboxsdk.annotations;

import android.graphics.Point;
import android.graphics.PointF;

import com.mapbox.mapboxsdk.maps.MapboxMap;

public class MarkerView extends Marker {

    private MarkerViewManager markerViewManager;

    private PointF centerOffset;
    private Point infoWindowOffset;
    private boolean flat;
    private boolean visible;

    private int selectAnimRes;
    private int deselectAnimRes;

    private float tiltValue;
    private float rotation;
    private float alpha = 1;

    MarkerView() {
        centerOffset = new PointF();
        infoWindowOffset = new Point();
    }

    public MarkerView(BaseMarkerViewOptions baseMarkerViewOptions) {
        super(baseMarkerViewOptions);
        this.centerOffset = baseMarkerViewOptions.getCenterOffset();
        this.infoWindowOffset = baseMarkerViewOptions.getInfoWindowOffset();
        this.flat = baseMarkerViewOptions.isFlat();
        this.selectAnimRes = baseMarkerViewOptions.getSelectAnimRes();
        this.deselectAnimRes = baseMarkerViewOptions.getDeselectAnimRes();
    }

    public PointF getCenterOffset() {
        return centerOffset;
    }

    public void setCenterOffset(PointF centerOffset) {
        this.centerOffset = centerOffset;
    }

    public Point getInfoWindowOffset() {
        return infoWindowOffset;
    }

    public void setInfoWindowOffset(Point infoWindowOffset) {
        this.infoWindowOffset = infoWindowOffset;
    }

    public boolean isFlat() {
        return flat;
    }

    public void setFlat(boolean flat) {
        this.flat = flat;
    }

    public int getSelectAnimRes() {
        return selectAnimRes;
    }

    public void setSelectAnimRes(int selectAnimRes) {
        this.selectAnimRes = selectAnimRes;
    }

    public int getDeselectAnimRes() {
        return deselectAnimRes;
    }

    public void setDeselectAnimRes(int deselectAnimRes) {
        this.deselectAnimRes = deselectAnimRes;
    }

    float getTilt() {
        return tiltValue;
    }

    void setTilt(float tiltValue) {
        this.tiltValue = tiltValue;
    }

    public void setVisible(boolean visible) {
        this.visible = visible;
        if (markerViewManager != null) {
            markerViewManager.animateVisible(this, visible);
        }
    }

    public boolean isVisible() {
        return visible;
    }

    public void setRotation(float rotation) {
        this.rotation = rotation;
        if (markerViewManager != null) {
            markerViewManager.animateRotation(this, rotation);
        }
    }

    public float getRotation() {
        return rotation;
    }

    public float getAlpha() {
        return alpha;
    }

    public void setAlpha(float alpha) {
        this.alpha = alpha;
        if (markerViewManager != null) {
            markerViewManager.animateAlpha(this, alpha);
        }
    }

    @Override
    public void setMapboxMap(MapboxMap mapboxMap) {
        super.setMapboxMap(mapboxMap);
        markerViewManager = mapboxMap.getMarkerViewManager();
    }
}
