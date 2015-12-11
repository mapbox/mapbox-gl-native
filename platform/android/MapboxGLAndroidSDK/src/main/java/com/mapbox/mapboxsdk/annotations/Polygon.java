package com.mapbox.mapboxsdk.annotations;

import android.graphics.Color;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public final class Polygon extends MultiPoint {

    private int fillColor = Color.BLACK; // default fillColor is black
    private int strokeColor = Color.BLACK; // default strokeColor is black

    Polygon() {
        super();
    }

    public int getFillColor() {
        return fillColor;
    }

    public int getStrokeColor() {
        return strokeColor;
    }

    void setFillColor(int color) {
        fillColor = color;
    }

    void setStrokeColor(int color) {
        strokeColor = color;
    }
}
