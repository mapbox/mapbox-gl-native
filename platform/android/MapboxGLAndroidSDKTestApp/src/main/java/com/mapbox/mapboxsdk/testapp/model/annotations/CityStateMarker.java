package com.mapbox.mapboxsdk.testapp.model.annotations;

import com.mapbox.mapboxsdk.annotations.Marker;

public class CityStateMarker extends Marker {

    private String mInfoWindowBackgroundColor;

    public CityStateMarker(CityStateMarkerOptions cityStateOptions, String color) {
        super(cityStateOptions);
        mInfoWindowBackgroundColor = color;
    }

    public String getInfoWindowBackgroundColor() {
        return mInfoWindowBackgroundColor;
    }

}