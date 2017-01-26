package com.mapbox.mapboxsdk.testapp.model.annotations;

import com.mapbox.mapboxsdk.annotations.Marker;

public class CityStateMarker extends Marker {

  private String infoWindowBackgroundColor;

  public CityStateMarker(CityStateMarkerOptions cityStateOptions, String color) {
    super(cityStateOptions);
    infoWindowBackgroundColor = color;
  }

  public String getInfoWindowBackgroundColor() {
    return infoWindowBackgroundColor;
  }

}
