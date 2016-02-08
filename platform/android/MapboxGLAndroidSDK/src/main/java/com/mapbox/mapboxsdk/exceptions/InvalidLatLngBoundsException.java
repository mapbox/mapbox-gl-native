package com.mapbox.mapboxsdk.exceptions;

public class InvalidLatLngBoundsException extends RuntimeException {

    public InvalidLatLngBoundsException(int latLngsListSize) {
        super("Cannot create a LatLngBounds from " + latLngsListSize +" items");
    }
}
