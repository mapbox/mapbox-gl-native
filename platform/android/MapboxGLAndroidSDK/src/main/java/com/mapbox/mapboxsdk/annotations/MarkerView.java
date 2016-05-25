package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.maps.MapboxMap;

public class MarkerView extends Marker {

    private MarkerViewManager markerViewManager;

    private float anchorU;
    private float anchorV;

    private float offsetX;
    private float offsetY;

    private float infoWindowAnchorU;
    private float infoWindowAnchorV;

    private boolean flat;
    private boolean visible = true;

    private int selectAnimRes;
    private int deselectAnimRes;

    private float tiltValue;
    private float rotation;
    private float alpha = 1;

    MarkerView() {
    }

    public MarkerView(BaseMarkerViewOptions baseMarkerViewOptions) {
        super(baseMarkerViewOptions);
        this.anchorU = baseMarkerViewOptions.getAnchorU();
        this.anchorV = baseMarkerViewOptions.getAnchorV();
        this.infoWindowAnchorU = baseMarkerViewOptions.getInfoWindowAnchorU();
        this.infoWindowAnchorV = baseMarkerViewOptions.getInfoWindowAnchorV();
        this.flat = baseMarkerViewOptions.isFlat();
        this.selectAnimRes = baseMarkerViewOptions.getSelectAnimRes();
        this.deselectAnimRes = baseMarkerViewOptions.getDeselectAnimRes();
        this.infoWindowAnchorU = baseMarkerViewOptions.infoWindowAnchorU;
        this.infoWindowAnchorV = baseMarkerViewOptions.infoWindowAnchorV;
        this.anchorU = baseMarkerViewOptions.anchorU;
        this.anchorV = baseMarkerViewOptions.anchorV;
    }

    public void setAnchor(float u, float v) {
        this.anchorU = u;
        this.anchorV = v;
    }

    public float getAnchorU() {
        return anchorU;
    }

    public float getAnchorV() {
        return anchorV;
    }

    void setOffsetX(float x){
        offsetX = x;
    }

    void setOffsetY(float y){
        offsetY = y;
    }

    float getOffsetX() {
        return offsetX;
    }

    float getOffsetY() {
        return offsetY;
    }

    public void setInfoWindowAnchor(float u, float v) {
        this.infoWindowAnchorU = u;
        this.infoWindowAnchorV = v;
    }

    public float getInfoWindowAnchorU() {
        return infoWindowAnchorU;
    }

    public float getInfoWindowAnchorV() {
        return infoWindowAnchorV;
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

    @Override
    public String toString() {
        return "MarkerView [position[" + getPosition() + "]]";
    }
}
