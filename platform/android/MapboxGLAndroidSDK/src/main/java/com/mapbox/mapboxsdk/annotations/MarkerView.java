package com.mapbox.mapboxsdk.annotations;

import android.graphics.Point;
import android.graphics.PointF;

public class MarkerView extends Marker {

    private PointF centerOffset;
    private Point infoWindowOffset;
    private boolean flat;

    private int selectAnimRes;
    private int deselectAnimRes;

    private float tiltValue;

    MarkerView(){
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

    public float getTiltValue() {
        return tiltValue;
    }

    public void setTiltValue(float tiltValue) {
        this.tiltValue = tiltValue;
    }
}
