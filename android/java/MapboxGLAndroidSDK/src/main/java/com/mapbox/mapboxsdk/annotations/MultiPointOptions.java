package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

public abstract class MultiPointOptions extends AnnotationOptions {

    public MultiPointOptions() {}

    public List<LatLng> getPoints() {
        // the getter gives us a copy, which is the safe thing to do...
        return ((MultiPoint)annotation).getPoints();
    }
}
