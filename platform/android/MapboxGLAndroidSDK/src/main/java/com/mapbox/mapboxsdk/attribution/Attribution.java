package com.mapbox.mapboxsdk.attribution;

import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.List;

public class Attribution {

  private static final String OPENSTREETMAP = "OpenStreetMap";
  private static final String OPENSTREETMAP_ABBR = "OSM";
  static final String TELEMETRY = "Telemetry Settings";
  static final String MAPBOX_URL = "https://www.mapbox.com/about/maps/";
  static final String TELEMETRY_URL = "https://www.mapbox.com/telemetry/";
  static final List<String> IMPROVE_MAP_URLS = new ArrayList<>();

  static {
    // Using a List makes URL backwards compatible
    IMPROVE_MAP_URLS.add("https://www.mapbox.com/feedback/");
    IMPROVE_MAP_URLS.add("https://www.mapbox.com/map-feedback/");
    IMPROVE_MAP_URLS.add("https://apps.mapbox.com/feedback/");
  }

  private String title;
  private String url;

  Attribution(String title, String url) {
    this.title = title;
    this.url = url;
  }

  public String getTitle() {
    return title;
  }

  public String getTitleAbbreviated() {
    if (title.equals(OPENSTREETMAP)) {
      return OPENSTREETMAP_ABBR;
    }
    return title;
  }

  public String getUrl() {
    return url;
  }

  @Override
  public boolean equals(@Nullable Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    Attribution that = (Attribution) o;

    if (title != null ? !title.equals(that.title) : that.title != null) {
      return false;
    }
    return url != null ? url.equals(that.url) : that.url == null;
  }

  @Override
  public int hashCode() {
    int result = title != null ? title.hashCode() : 0;
    result = 31 * result + (url != null ? url.hashCode() : 0);
    return result;
  }
}
