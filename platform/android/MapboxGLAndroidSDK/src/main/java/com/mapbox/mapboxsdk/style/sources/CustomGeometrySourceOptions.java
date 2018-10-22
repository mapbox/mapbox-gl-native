package com.mapbox.mapboxsdk.style.sources;

/**
 * Builder class for composing CustomGeometrySource objects.
 */
public class CustomGeometrySourceOptions extends GeoJsonOptions {

  /**
   * If the data includes wrapped coordinates, setting this to true unwraps the coordinates.
   *
   * @param wrap defaults to false
   * @return the current instance for chaining
   */
  public CustomGeometrySourceOptions withWrap(boolean wrap) {
    this.put("wrap", wrap);
    return this;
  }

  /**
   * If the data includes geometry outside the tile boundaries, setting this to true clips the geometry
   * to the tile boundaries.
   *
   * @param clip defaults to false
   * @return the current instance for chaining
   */
  public CustomGeometrySourceOptions withClip(boolean clip) {
    this.put("clip", clip);
    return this;
  }

}
