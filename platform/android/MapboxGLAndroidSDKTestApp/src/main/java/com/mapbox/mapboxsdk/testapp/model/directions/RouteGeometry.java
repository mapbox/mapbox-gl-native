package com.mapbox.mapboxsdk.testapp.model.directions;

import java.util.ArrayList;
import java.util.List;

public class RouteGeometry {

    private String type;
    private List<List<Double>> coordinates;

    public RouteGeometry() {
        coordinates = new ArrayList<>();
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public List<List<Double>> getCoordinates() {
        return coordinates;
    }

    public void setCoordinates(List<List<Double>> coordinates) {
        this.coordinates = coordinates;
    }
}
