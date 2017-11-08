package com.mapbox.mapboxsdk.attribution;

public class Attribution {

  private String title;
  private String url;

  Attribution(String title, String url) {
    this.title = title;
    this.url = url;
  }

  public String getTitle() {
    return title;
  }

  public String getUrl() {
    return url;
  }

  @Override
  public boolean equals(Object o) {
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
