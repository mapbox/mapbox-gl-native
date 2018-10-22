package com.mapbox.mapboxsdk.http;

public class HttpRequestUrl {

  private HttpRequestUrl() {
  }

  /**
   * Adapts a resource request url based on the host and query size.
   *
   * @param host        the host used as endpoint
   * @param resourceUrl the resource to download
   * @param querySize   the query size of the resource request
   * @return the adapted resource url
   */
  public static String buildResourceUrl(String host, String resourceUrl, int querySize) {
    if (isValidMapboxEndpoint(host)) {
      if (querySize == 0) {
        resourceUrl = resourceUrl + "?";
      } else {
        resourceUrl = resourceUrl + "&";
      }
      resourceUrl = resourceUrl + "events=true";
    }
    return resourceUrl;
  }

  /**
   * Validates if the host used as endpoint is a valid Mapbox endpoint.
   *
   * @param host the host used as endpoint
   * @return true if a valid Mapbox endpoint
   */
  private static boolean isValidMapboxEndpoint(String host) {
    return host.equals("mapbox.com")
      || host.endsWith(".mapbox.com")
      || host.equals("mapbox.cn")
      || host.endsWith(".mapbox.cn");
  }
}
