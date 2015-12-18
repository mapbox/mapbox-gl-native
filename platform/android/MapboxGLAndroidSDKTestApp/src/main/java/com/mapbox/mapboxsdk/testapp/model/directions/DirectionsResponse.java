package com.mapbox.mapboxsdk.testapp.model.directions;

import java.util.ArrayList;
import java.util.List;

public class DirectionsResponse {

    private DirectionsFeature origin;
    private DirectionsFeature destination;
    private List<DirectionsFeature> waypoints;
    private List<DirectionsRoute> routes;

    public DirectionsResponse() {
        waypoints = new ArrayList<>();
        routes = new ArrayList<>();
    }

    public DirectionsFeature getOrigin() {
        return origin;
    }

    public void setOrigin(DirectionsFeature origin) {
        this.origin = origin;
    }

    public DirectionsFeature getDestination() {
        return destination;
    }

    public void setDestination(DirectionsFeature destination) {
        this.destination = destination;
    }

    public List<DirectionsFeature> getWaypoints() {
        return waypoints;
    }

    public void setWaypoints(List<DirectionsFeature> waypoints) {
        this.waypoints = waypoints;
    }

    public List<DirectionsRoute> getRoutes() {
        return routes;
    }

    public void setRoutes(List<DirectionsRoute> routes) {
        this.routes = routes;
    }
}
