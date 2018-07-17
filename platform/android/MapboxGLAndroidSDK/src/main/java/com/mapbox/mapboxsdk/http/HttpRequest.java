package com.mapbox.mapboxsdk.http;

/**
 * Interface definition for performing http requests.
 * <p>
 * This allows to provide alternative implementations for the http interaction of this library.
 * </p>
 */
public interface HttpRequest {

  int CONNECTION_ERROR = 0;
  int TEMPORARY_ERROR = 1;
  int PERMANENT_ERROR = 2;

  /**
   * Executes the request.
   *
   * @param httpRequest callback to be invoked when we receive a response
   * @param nativePtr   the pointer associated to the request
   * @param resourceUrl the resource url to download
   * @param etag        http header, identifier for a specific version of a resource
   * @param modified    http header, used to determine if a resource hasn't been modified since
   */
  void executeRequest(HttpResponder httpRequest, long nativePtr, String resourceUrl,
                      String etag, String modified);

  /**
   * Cancels the request.
  */
  void cancelRequest();
}
