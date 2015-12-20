package com.mapbox.mapboxsdk.testapp.model.directions;

public class StepManeuver {

    private String type;
    private ManeuverPoint location;
    private String instruction;
    private String mode;

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public ManeuverPoint getLocation() {
        return location;
    }

    public void setLocation(ManeuverPoint location) {
        this.location = location;
    }

    public String getInstruction() {
        return instruction;
    }

    public void setInstruction(String instruction) {
        this.instruction = instruction;
    }

    public String getMode() {
        return mode;
    }

    public void setMode(String mode) {
        this.mode = mode;
    }
}
