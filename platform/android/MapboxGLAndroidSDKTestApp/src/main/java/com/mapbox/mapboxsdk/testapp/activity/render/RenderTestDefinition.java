package com.mapbox.mapboxsdk.testapp.activity.render;

import com.mapbox.mapboxsdk.snapshotter.MapSnapshotter;

public class RenderTestDefinition {

  private static final int DEFAULT_WIDTH = 512;
  private static final int DEFAULT_HEIGHT = 512;

  private String category; // eg. background-color
  private String name; // eg. colorSpace-hcl
  private String styleJson;
  private RenderTestStyleDefinition definition;

  RenderTestDefinition(String category, String name, String styleJson, RenderTestStyleDefinition definition) {
    this.category = category;
    this.name = name;
    this.styleJson = styleJson;
    this.definition = definition;
  }

  public String getName() {
    return name;
  }

  public String getCategory() {
    return category;
  }

  public int getWidth() {
    RenderTestStyleDefinition.Test test = getTest();
    if (test != null) {
      Integer testWidth = test.getWidth();
      if (testWidth != null && testWidth > 0) {
        return testWidth;
      }
    }
    return DEFAULT_WIDTH;
  }

  public int getHeight() {
    RenderTestStyleDefinition.Test test = getTest();
    if (test != null) {
      Integer testHeight = test.getHeight();
      if (testHeight != null && testHeight > 0) {
        return testHeight;
      }
    }
    return DEFAULT_HEIGHT;
  }

  public float getPixelRatio() {
    RenderTestStyleDefinition.Test test = getTest();
    if (test != null) {
      Float pixelRatio = test.getPixelRatio();
      if (pixelRatio != null && pixelRatio > 0) {
        return pixelRatio;
      }
    }
    return 1;
  }

  public String getStyleJson() {
    return styleJson;
  }

  public boolean hasOperations() {
    return getTest().getOperations() != null;
  }

  public RenderTestStyleDefinition.Test getTest() {
    return definition.getMetadata().getTest();
  }

  public MapSnapshotter.Options toOptions() {
    return new MapSnapshotter
      .Options(getWidth(), getHeight())
      .withStyleJson(styleJson)
      .withPixelRatio(getPixelRatio())
      .withLogo(false);
  }

  @Override
  public String toString() {
    return "RenderTestDefinition{"
      + "category='" + category + '\''
      + ", name='" + name + '\''
      + ", styleJson='" + styleJson + '\''
      + '}';
  }
}
