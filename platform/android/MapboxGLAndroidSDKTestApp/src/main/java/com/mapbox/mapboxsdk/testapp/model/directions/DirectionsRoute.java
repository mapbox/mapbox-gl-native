package com.mapbox.mapboxsdk.testapp.model.directions;

import java.util.ArrayList;
import java.util.List;

public class DirectionsRoute {

    private int distance;
    private int duration;
    private String summary;
    private RouteGeometry geometry;
    private List<RouteStep> steps;

    public DirectionsRoute() {
        steps = new ArrayList<>();
    }

    public int getDistance() {
        return distance;
    }

    public void setDistance(int distance) {
        this.distance = distance;
    }

    public int getDuration() {
        return duration;
    }

    public void setDuration(int duration) {
        this.duration = duration;
    }

    public String getSummary() {
        return summary;
    }

    public void setSummary(String summary) {
        this.summary = summary;
    }

    public RouteGeometry getGeometry() {
        return geometry;
    }

    public void setGeometry(RouteGeometry geometry) {
        this.geometry = geometry;
    }

    public List<RouteStep> getSteps() {
        return steps;
    }

    public void setSteps(List<RouteStep> steps) {
        this.steps = steps;
    }
}
