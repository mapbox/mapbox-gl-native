package com.mapbox.mapboxsdk.maps.widgets.scaleview;


import com.mapbox.mapboxsdk.geometry.LatLng;

public class DistanceUtils {

    public static final double EARTH = 6371.137; //Radius of the Earth in km

    public static LatLng translatePoint(LatLng point, double distance, double bearing) {
        distance = distance / 1000d;

        //converts the Latitude, Longitude and bearings into radians
        double lat = Math.toRadians(point.getLatitude());
        double lng = Math.toRadians(point.getLongitude());
        bearing = Math.toRadians(bearing);

        //Give the distance and the first Latitude, computes the second latitude
        double Lat2 = Math.asin((Math.sin(lat) * Math.cos(distance / EARTH)) +
                (Math.cos(lat) * Math.sin(distance / EARTH) * Math.cos(bearing)));

        //Give the distance and the first longitude, computes the second longitude
        double Long2 = lng + Math.atan2(Math.sin(bearing) * Math.sin(distance / EARTH) * Math.cos(lat),
                Math.cos(distance / EARTH) - (Math.sin(lat) * Math.sin(Lat2)));

        //Converting the new Latitude and Longitude from radians to degrees
        Lat2 = Math.toDegrees(Lat2);
        Long2 = Math.toDegrees(Long2);

        //Creates a point object to return back. X is the longitude, Y is the Latitude
        return new LatLng(Lat2, Long2);
    }

}
