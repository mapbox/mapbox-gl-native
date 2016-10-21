package com.mapbox.mapboxsdk.maps;

/**
 * Settings for the overlain views of a MapboxMap. Used by UiSettings.
 */
class CompassViewSettings extends ViewSettings{

    private boolean fadeFacingNorth = true;

    public CompassViewSettings() {
        super();
    }

    public boolean isFadeFacingNorth() {
        return fadeFacingNorth;
    }

    public void setFadeFacingNorth(boolean fadeFacingNorth) {
        this.fadeFacingNorth = fadeFacingNorth;
    }
}
