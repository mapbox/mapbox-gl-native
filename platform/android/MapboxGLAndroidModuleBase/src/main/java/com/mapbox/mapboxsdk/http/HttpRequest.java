package com.mapbox.mapboxsdk.http;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.support.annotation.NonNull;
import com.mapbox.mapboxsdk.utils.MapboxConfigurationWrapper;

/**
 * Base class for performing core http requests.
 * <p>
 * This abstraction allows to provide alternative implementations for the http interaction of this library.
 * </p>
 */
public abstract class HttpRequest {

  static final int CONNECTION_ERROR = 0;
  static final int TEMPORARY_ERROR = 1;
  static final int PERMANENT_ERROR = 2;

  /**
   * Executes the request.
   *
   * @param httpRequest callback to be invoked when we receive a response
   * @param nativePtr   the pointer associated to the request
   * @param resourceUrl the resource url to download
   * @param etag        http header, identifier for a specific version of a resource
   * @param modified    http header, used to determine if a resource hasn't been modified since
   */
  public abstract void executeRequest(HttpRequestResponder httpRequest, long nativePtr, String resourceUrl,
                                      String etag, String modified);

  /**
   * Cancels the request.
   */
  public abstract void cancelRequest();

  //
  // Utility methods
  //

  /**
   * Returns the application identifier, consisting out the package name, version name and version code.
   *
   * @return the appliciation identifier
   */
  static String getApplicationIdentifier() {
    return getApplicationIdentifier(MapboxConfigurationWrapper.getContext());
  }

  /**
   * Returns the application identifier, consisting out the package name, version name and version code.
   *
   * @param context the context used to retrieve the package manager from
   * @return the appliciation identifier
   */
  private static String getApplicationIdentifier(@NonNull Context context) {
    try {
      PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
      return String.format("%s/%s (%s)", context.getPackageName(), packageInfo.versionName, packageInfo.versionCode);
    } catch (Exception exception) {
      return "";
    }
  }

  /**
   * Adapts a resource request url based on the host and query size.
   *
   * @param host        the host used as endpoint
   * @param resourceUrl the resource to download
   * @param querySize   the query size of the resource request
   * @return the adapted resource url
   */
  String buildResourceUrl(String host, String resourceUrl, int querySize) {
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
  private boolean isValidMapboxEndpoint(String host) {
    return host.equals("mapbox.com") || host.endsWith(".mapbox.com") || host.equals("mapbox.cn")
      || host.endsWith(".mapbox.cn");
  }
}
