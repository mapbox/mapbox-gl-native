package com.mapbox.mapboxsdk.testapp.model.annotations;

import com.mapbox.mapboxsdk.annotations.BaseMarkerViewOptions;
import com.mapbox.mapboxsdk.annotations.MarkerView;

public class CountryMarkerView extends MarkerView {

  private String abbrevName;
  private int flagRes;

  public CountryMarkerView(BaseMarkerViewOptions baseMarkerViewOptions, String abbrevName, int flagRes) {
    super(baseMarkerViewOptions);
    this.abbrevName = abbrevName;
    this.flagRes = flagRes;
  }

  public String getAbbrevName() {
    return abbrevName;
  }

  public int getFlagRes() {
    return flagRes;
  }
}
