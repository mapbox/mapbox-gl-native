package com.mapbox.mapboxsdk.maps;

/**
 * Interface definition for a callback to be invoked when the map is ready to be used.
 * <p>
 * Once an instance of this interface is set on a {@link MapFragment} or {@link MapView} object,
 * the onMapReady(MapboxMap) method is triggered when the map is ready to be used and provides an instance of
 * {@link MapboxMap}.
 * </p>
 */
public interface OnMapReadyCallback {

  /**
   * Called when the map is ready to be used.
   *
   * @param mapboxMap An instance of MapboxMap associated with the {@link MapFragment} or
   *                  {@link MapView} that defines the callback.
   */
  void onMapReady(MapboxMap mapboxMap);
}
