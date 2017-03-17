package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.Size;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.HashMap;
import java.util.Map;

/**
 * Tile set, allows using TileJson specification as source.
 *
 * @see <a href="https://github.com/mapbox/tilejson-spec/tree/master/2.1.0">The tileset specification</a>
 */
public class TileSet {
  private final String tilejson;
  private String name;
  private String description;
  private String version;
  private String attribution;
  private String template;
  private String legend;
  private String scheme;
  private final String[] tiles;
  private String[] grids;
  private String[] data;
  private Float minZoom;
  private Float maxZoom;
  private Float[] bounds;
  private Float[] center;

  /**
   * @param tilejson A semver.org style version number. Describes the version of the TileJSON spec that is implemented
   *                 by this JSON object.
   * @param tiles    An array of tile endpoints. {z}, {x} and {y}, if present, are replaced with the corresponding
   *                 integers.
   *                 If multiple endpoints are specified, clients may use any combination of endpoints. All endpoints
   *                 MUST return the same
   *                 content for the same URL. The array MUST contain at least one endpoint.
   *                 Example: "http:localhost:8888/admin/1.0.0/world-light,broadband/{z}/{x}/{y}.png"
   */
  public TileSet(String tilejson, String... tiles) {
    this.tilejson = tilejson;
    this.tiles = tiles;
  }

  public String getTilejson() {
    return tilejson;
  }

  public String getName() {
    return name;
  }

  /**
   * A name describing the tileset. The name can
   * contain any legal character. Implementations SHOULD NOT interpret the
   * name as HTML.
   * "name": "compositing",
   *
   * @param name the name to be set
   */
  public void setName(String name) {
    this.name = name;
  }

  public String getDescription() {
    return description;
  }

  /**
   * A text description of the tileset. The
   * description can contain any legal character.
   * Implementations SHOULD NOT
   * interpret the description as HTML.
   * "description": "A simple, light grey world."
   *
   * @param description the description to set
   */
  public void setDescription(String description) {
    this.description = description;
  }

  public String getVersion() {
    return version;
  }

  public void setVersion(String version) {
    this.version = version;
  }

  public String getAttribution() {
    return attribution;
  }

  /**
   * Default: null. Contains an attribution to be displayed
   * when the map is shown to a user. Implementations MAY decide to treat this
   * as HTML or literal text. For security reasons, make absolutely sure that
   * this field can't be abused as a vector for XSS or beacon tracking.
   * "attribution": "<a href='http:openstreetmap.org'>OSM contributors</a>",
   *
   * @param attribution the attribution to set
   */
  public void setAttribution(String attribution) {
    this.attribution = attribution;
  }

  public String getTemplate() {
    return template;
  }

  /**
   * Contains a mustache template to be used to
   * format data from grids for interaction.
   * See https:github.com/mapbox/utfgrid-spec/tree/master/1.2
   * for the interactivity specification.
   * "template": "{{#__teaser__}}{{NAME}}{{/__teaser__}}"
   *
   * @param template the template to set
   */
  public void setTemplate(String template) {
    this.template = template;
  }

  public String getLegend() {
    return legend;
  }

  /**
   * Contains a legend to be displayed with the map.
   * Implementations MAY decide to treat this as HTML or literal text.
   * For security reasons, make absolutely sure that this field can't be
   * abused as a vector for XSS or beacon tracking.
   * "legend": "Dangerous zones are red, safe zones are green"
   *
   * @param legend the legend to set
   */
  public void setLegend(String legend) {
    this.legend = legend;
  }

  public String getScheme() {
    return scheme;
  }

  /**
   * Default: "xyz". Either "xyz" or "tms". Influences the y
   * direction of the tile coordinates.
   * The global-mercator (aka Spherical Mercator) profile is assumed.
   * "scheme": "xyz"
   *
   * @param scheme the scheme to set
   */
  public void setScheme(String scheme) {
    this.scheme = scheme;
  }

  public String[] getTiles() {
    return tiles;
  }

  public String[] getGrids() {
    return grids;
  }

  /**
   * An array of interactivity endpoints. {z}, {x}
   * and {y}, if present, are replaced with the corresponding integers. If multiple
   * endpoints are specified, clients may use any combination of endpoints.
   * All endpoints MUST return the same content for the same URL.
   * If the array doesn't contain any entries, interactivity is not supported
   * for this tileset.     See https:github.com/mapbox/utfgrid-spec/tree/master/1.2
   * for the interactivity specification.
   * <p>
   * Example: "http:localhost:8888/admin/1.0.0/broadband/{z}/{x}/{y}.grid.json"
   * </p>
   *
   * @param grids the grids to set
   */
  public void setGrids(String... grids) {
    this.grids = grids;
  }

  public String[] getData() {
    return data;
  }

  /**
   * An array of data files in GeoJSON format.
   * {z}, {x} and {y}, if present,
   * are replaced with the corresponding integers. If multiple
   * endpoints are specified, clients may use any combination of endpoints.
   * All endpoints MUST return the same content for the same URL.
   * If the array doesn't contain any entries, then no data is present in
   * the map.
   * <p>
   * "http:localhost:8888/admin/data.geojson"
   * </p>
   *
   * @param data the data array to set
   */
  public void setData(String... data) {
    this.data = data;
  }

  public float getMinZoom() {
    return minZoom;
  }

  /**
   * 0. &gt;= 0, &lt; 22. An integer specifying the minimum zoom level.
   *
   * @param minZoom the minZoom level to set
   */
  public void setMinZoom(float minZoom) {
    this.minZoom = minZoom;
  }

  public float getMaxZoom() {
    return maxZoom;
  }

  /**
   * 0. &gt;= 0, &lt;= 22. An integer specifying the maximum zoom level.
   *
   * @param maxZoom the maxZoom level to set
   */
  public void setMaxZoom(float maxZoom) {
    this.maxZoom = maxZoom;
  }

  public Float[] getBounds() {
    return bounds;
  }

  /**
   * Default: [-180, -90, 180, 90]. The maximum extent of available map tiles. Bounds MUST define an area
   * covered by all zoom levels. The bounds are represented in WGS:84
   * latitude and longitude values, in the order left, bottom, right, top.
   * Values may be integers or floating point numbers.
   *
   * @param bounds the Float array to set
   */
  public void setBounds(@Size(value = 4) Float... bounds) {
    this.bounds = bounds;
  }

  public Float[] getCenter() {
    return center;
  }

  /**
   * The first value is the longitude, the second is latitude (both in
   * WGS:84 values), the third value is the zoom level as an integer.
   * Longitude and latitude MUST be within the specified bounds.
   * The zoom level MUST be between minzoom and maxzoom.
   * Implementations can use this value to set the default location. If the
   * value is null, implementations may use their own algorithm for
   * determining a default location.
   *
   * @param center the Float array to set
   */
  public void setCenter(@Size(value = 2) Float... center) {
    this.center = center;
  }

  public void setCenter(LatLng center) {
    this.center = new Float[] {(float) center.getLongitude(), (float) center.getLatitude()};
  }

  Map<String, Object> toValueObject() {
    Map<String, Object> result = new HashMap<>();
    result.put("tilejson", tilejson);
    result.put("tiles", tiles);

    if (name != null) {
      result.put("name", name);
    }
    if (description != null) {
      result.put("description", description);
    }
    if (version != null) {
      result.put("version", version);
    }
    if (attribution != null) {
      result.put("attribution", attribution);
    }
    if (template != null) {
      result.put("template", template);
    }
    if (legend != null) {
      result.put("legend", legend);
    }
    if (scheme != null) {
      result.put("scheme", scheme);
    }
    if (grids != null) {
      result.put("grids", grids);
    }
    if (data != null) {
      result.put("data", data);
    }
    if (minZoom != null) {
      result.put("minzoom", minZoom);
    }
    if (maxZoom != null) {
      result.put("maxzoom", maxZoom);
    }
    if (bounds != null) {
      result.put("bounds", bounds);
    }
    if (center != null) {
      result.put("center", center);
    }

    return result;
  }
}
