package com.mapbox.mapboxsdk.maps;

public class Attribution {

  private String title;
  private String url;

  public Attribution(String title, String url) {
    this.title = title;
    this.url = url;
  }

  public String getTitle() {
    return title;
  }

  public String getUrl() {
    return url;
  }
}
