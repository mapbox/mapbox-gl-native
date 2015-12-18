package com.mapbox.mapboxsdk.testapp.model.directions;

import com.google.gson.annotations.SerializedName;

public class RouteStep {

    private int distance;
    private int duration;
    @SerializedName("wayName")
    private String wayName;
    private String direction;
    private double heading;
    private StepManeuver maneuver;

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

    public String getWayName() {
        return wayName;
    }

    public void setWayName(String wayName) {
        this.wayName = wayName;
    }

    public String getDirection() {
        return direction;
    }

    public void setDirection(String direction) {
        this.direction = direction;
    }

    public double getHeading() {
        return heading;
    }

    public void setHeading(double heading) {
        this.heading = heading;
    }

    public StepManeuver getManeuver() {
        return maneuver;
    }

    public void setManeuver(StepManeuver maneuver) {
        this.maneuver = maneuver;
    }
}
