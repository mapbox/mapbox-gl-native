package com.mapbox.mapboxgl.views.widget;

import android.location.Location;

import com.mapbox.mapboxgl.views.MapDelegate;

public interface CompassDelegate extends MapDelegate{

    Location getMyLocation();

}
