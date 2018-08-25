package com.mapbox.mapboxsdk.http;

/**
 * Interface definition for a callback to be invoked when either a response was returned for a requested resource or
 * when an error occurred when requesting the resource.
 */
public interface HttpResponder {

  /**
   * Invoked when a resource has finished.
   *
   * @param responseCode    http response code
   * @param eTag            http header, identifier for a specific version of a resource
   * @param lastModified    http header, used to determine if a resource hasn't been modified since
   * @param cacheControl    http header, used to determine cache strategy of a resource
   * @param expires         http header, used to determine when a resource is stale
   * @param retryAfter      http header, used to indicate when the service is expected to be unavailable to the client
   * @param xRateLimitReset http header, used to determine the remaining window before the rate limit resets
   * @param body            http response body, in an array of bytes representation
   */
  void onResponse(int responseCode, String eTag, String lastModified, String cacheControl, String expires,
                  String retryAfter, String xRateLimitReset, byte[] body);

  /**
   * Invoked when a resource failed to be retrieved.
   *
   * @param type         the error type, either one of {@link HttpRequest#CONNECTION_ERROR},
   *                     {@link HttpRequest#TEMPORARY_ERROR} or {@link HttpRequest#PERMANENT_ERROR}
   * @param errorMessage the error message associated with the failure
   */
  void handleFailure(int type, String errorMessage);
}
