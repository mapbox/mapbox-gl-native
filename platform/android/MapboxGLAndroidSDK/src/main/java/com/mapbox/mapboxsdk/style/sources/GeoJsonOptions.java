package com.mapbox.mapboxsdk.style.sources;

import java.util.HashMap;

/**
 * Builder class for composing GeoJsonSource objects.
 *
 * @see GeoJsonSource
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">The online documentation</a>
 */
public class GeoJsonOptions extends HashMap<String, Object> {

  /**
   * Maximum zoom level at which to create vector tiles (higher means greater detail at high zoom levels).
   *
   * @param minZoom the maximum zoom - Defaults to 18.
   * @return the current instance for chaining
   */
  public GeoJsonOptions withMinZoom(int minZoom) {
    this.put("minzoom", minZoom);
    return this;
  }

  /**
   * Maximum zoom level at which to create vector tiles (higher means greater detail at high zoom levels).
   *
   * @param maxZoom the maximum zoom - Defaults to 18.
   * @return the current instance for chaining
   */
  public GeoJsonOptions withMaxZoom(int maxZoom) {
    this.put("maxzoom", maxZoom);
    return this;
  }

  /**
   * Tile buffer size on each side (measured in 1/512ths of a tile; higher means fewer rendering artifacts near tile
   * edges but slower performance).
   *
   * @param buffer the buffer size - Defaults to 128.
   * @return the current instance for chaining
   */
  public GeoJsonOptions withBuffer(int buffer) {
    this.put("buffer", buffer);
    return this;
  }

  /**
   * Initialises whether to calculate line distance metrics.
   *
   * @param lineMetrics true to calculate line distance metrics.
   * @return the current instance for chaining
   */
  public GeoJsonOptions withLineMetrics(boolean lineMetrics) {
    this.put("lineMetrics", lineMetrics);
    return this;
  }

  /**
   * Douglas-Peucker simplification tolerance (higher means simpler geometries and faster performance).
   *
   * @param tolerance the tolerance - Defaults to 0.375
   * @return the current instance for chaining
   */
  public GeoJsonOptions withTolerance(float tolerance) {
    this.put("tolerance", tolerance);
    return this;
  }

  /**
   * If the data is a collection of point features, setting this to true clusters the points by radius into groups.
   *
   * @param cluster cluster? - Defaults to false
   * @return the current instance for chaining
   */
  public GeoJsonOptions withCluster(boolean cluster) {
    this.put("cluster", cluster);
    return this;
  }

  /**
   * Max zoom to cluster points on.
   *
   * @param clusterMaxZoom clusterMaxZoom cluster maximum zoom - Defaults to one zoom less than maxzoom (so that last
   *                       zoom features are not clustered)
   * @return the current instance for chaining
   */
  public GeoJsonOptions withClusterMaxZoom(int clusterMaxZoom) {
    this.put("clusterMaxZoom", clusterMaxZoom);
    return this;
  }

  /**
   * Radius of each cluster when clustering points, measured in 1/512ths of a tile.
   *
   * @param clusterRadius cluster radius - Defaults to 50
   * @return the current instance for chaining
   */
  public GeoJsonOptions withClusterRadius(int clusterRadius) {
    this.put("clusterRadius", clusterRadius);
    return this;
  }
}
