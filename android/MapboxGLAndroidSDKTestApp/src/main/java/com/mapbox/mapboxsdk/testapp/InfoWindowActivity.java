package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;

import com.mapbox.mapboxsdk.MapActivity;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.views.MapView;

public class InfoWindowActivity extends MapActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        MapView mapView = getMapView();

        mapView.addMarker(new MarkerOptions()
                .title("Intersection")
                .snippet("H St NW with 15th St NW")
                .position(new LatLng(38.9002073, -77.03364419)));

        mapView.addMarker(new MarkerOptions()
                .title("White House")
                .snippet("The official residence and principal workplace of the President of the United States, located at 1600 Pennsylvania Avenue NW in Washington, D.C. It has been the residence of every U.S. president since John Adams in 1800.")
                .position(new LatLng(38.897705003219784, -77.03655168667463)));

        mapView.addMarker(new MarkerOptions().title("Intersection")
                .snippet("E St NW with 17th St NW")
                .position(new LatLng(38.8954236, -77.0394623)));

        mapView.setCenterCoordinate(new LatLngZoom(38.897705003219784, -77.03655168667463, 15));
    }

}
