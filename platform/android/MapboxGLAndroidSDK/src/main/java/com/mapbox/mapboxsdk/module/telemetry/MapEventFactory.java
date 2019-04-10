package com.mapbox.mapboxsdk.module.telemetry;

import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;

import com.mapbox.android.telemetry.TelemetryUtils;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.offline.OfflineRegion;

class MapEventFactory {

  static MapLoadEvent buildMapLoadEvent(@NonNull PhoneState phoneState) {
    String userId = TelemetryUtils.retrieveVendorId();
    return new MapLoadEvent(userId, phoneState);
  }

  static OfflineDownloadStartEvent buildOfflineDownloadStartEvent(PhoneState phoneState,
                                                                  String shapeForOfflineRegion,
                                                                  @FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
                                                                    to = MapboxConstants.MAXIMUM_ZOOM) Double minZoom,
                                                                  @FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
                                                                    to = MapboxConstants.MAXIMUM_ZOOM) Double maxZoom,
                                                                  String styleURL) {

    OfflineDownloadStartEvent offlineEvent =
      new OfflineDownloadStartEvent(phoneState, shapeForOfflineRegion, minZoom, maxZoom);
    offlineEvent.setStyleURL(styleURL);
    return offlineEvent;
  }

  static OfflineDownloadEndEvent buildOfflineDownloadCompleteEvent(PhoneState phoneState,
                                                                   String shapeForOfflineRegion,
                                                                   @FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
                                                                     to = MapboxConstants.MAXIMUM_ZOOM) Double minZoom,
                                                                   @FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
                                                                     to = MapboxConstants.MAXIMUM_ZOOM) Double maxZoom,
                                                                   String styleURL,
                                                                   Long sizeOfResourcesCompleted,
                                                                   Long numberOfTilesCompleted,
                                                                   @OfflineRegion.DownloadState int state) {

    OfflineDownloadEndEvent offlineEvent =
      new OfflineDownloadEndEvent(phoneState, shapeForOfflineRegion, minZoom, maxZoom);
    offlineEvent.setStyleURL(styleURL);
    offlineEvent.setSizeOfResourcesCompleted(sizeOfResourcesCompleted);
    offlineEvent.setNumberOfTilesCompleted(numberOfTilesCompleted);
    offlineEvent.setState(state);
    return offlineEvent;
  }

  static MapClickEvent buildMapClickEvent(@NonNull PhoneState phoneState, @NonNull MapState mapState) {
    return new MapClickEvent(phoneState, mapState);
  }

  static MapDragendEvent buildMapDragendEvent(@NonNull PhoneState phoneState, @NonNull MapState mapState) {
    return new MapDragendEvent(phoneState, mapState);
  }

}
