package com.mapbox.mapboxsdk.testapp.model.annotations;

import com.mapbox.mapboxsdk.annotations.BaseMarkerViewOptions;
import com.mapbox.mapboxsdk.annotations.MarkerView;

public class TextMarkerView extends MarkerView {

  private String text;

  public TextMarkerView(BaseMarkerViewOptions baseMarkerViewOptions, String text) {
    super(baseMarkerViewOptions);
    this.text = text;
  }

  public String getText() {
    return text;
  }
}
