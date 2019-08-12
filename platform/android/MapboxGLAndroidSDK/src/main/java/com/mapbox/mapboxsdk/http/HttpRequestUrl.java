package com.mapbox.mapboxsdk.http;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.Mapbox;

public class HttpRequestUrl {

  private HttpRequestUrl() {
  }

  /**
   * Adapts a resource request url based on the host, query size, and offline requirement.
   * The Maps SDK SKU token is not included with offline tile requests because offline requests
   * are counted as billable Vector Tile API requests. The offline tile requests are considered
   * raster Tile API requests if satellite imagery is being fetched.
   *
   * @param host        the host used as endpoint
   * @param resourceUrl the resource to download
   * @param querySize   the query size of the resource request
   * @param offline     the type of resource, either offline or online
   * @return the adapted resource url
   */
  public static String buildResourceUrl(@NonNull String host, String resourceUrl, int querySize, boolean offline) {
    if (isValidMapboxEndpoint(host)) {
      if (querySize == 0) {
        resourceUrl = resourceUrl + "?";
      } else {
        resourceUrl = resourceUrl + "&";
      }
      if (offline) {
        resourceUrl = resourceUrl + "offline=true";
      } else {
        resourceUrl = resourceUrl + "sku=" + Mapbox.getSkuToken();
      }
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
