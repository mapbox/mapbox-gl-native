package com.mapbox.mapboxsdk.testapp.activity.render;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class RenderTestStyleDefinition {

  private Integer version;
  private Metadata metadata;
  private Map<String, Object> additionalProperties = new HashMap<String, Object>();

  public Integer getVersion() {
    return version;
  }

  public void setVersion(Integer version) {
    this.version = version;
  }

  public Metadata getMetadata() {
    return metadata;
  }

  public void setMetadata(Metadata metadata) {
    this.metadata = metadata;
  }

  public Map<String, Object> getAdditionalProperties() {
    return this.additionalProperties;
  }

  public void setAdditionalProperty(String name, Object value) {
    this.additionalProperties.put(name, value);
  }

  public class Metadata {

    private Test test;
    private Map<String, Object> additionalProperties = new HashMap<String, Object>();

    public Test getTest() {
      return test;
    }

    public void setTest(Test test) {
      this.test = test;
    }

    public Map<String, Object> getAdditionalProperties() {
      return this.additionalProperties;
    }

    public void setAdditionalProperty(String name, Object value) {
      this.additionalProperties.put(name, value);
    }
  }

  public class Test {

    private Integer width;
    private Integer height;
    private Float pixelRatio;
    private List<Integer> center = null;
    private Integer zoom;
    private Double diff;
    private List<List<String>> operations = null;
    private Map<String, Object> additionalProperties = new HashMap<String, Object>();

    public Integer getWidth() {
      return width;
    }

    public void setWidth(Integer width) {
      this.width = width;
    }

    public Integer getHeight() {
      return height;
    }

    public void setHeight(Integer height) {
      this.height = height;
    }

    public Float getPixelRatio() {
      return pixelRatio;
    }

    public List<Integer> getCenter() {
      return center;
    }

    public void setCenter(List<Integer> center) {
      this.center = center;
    }

    public Integer getZoom() {
      return zoom;
    }

    public void setZoom(Integer zoom) {
      this.zoom = zoom;
    }

    public Double getDiff() {
      return diff;
    }

    public void setDiff(Double diff) {
      this.diff = diff;
    }

    public List<List<String>> getOperations() {
      return operations;
    }

    public void setOperations(List<List<String>> operations) {
      this.operations = operations;
    }

    public Map<String, Object> getAdditionalProperties() {
      return this.additionalProperties;
    }

    public void setAdditionalProperty(String name, Object value) {
      this.additionalProperties.put(name, value);
    }

  }
}