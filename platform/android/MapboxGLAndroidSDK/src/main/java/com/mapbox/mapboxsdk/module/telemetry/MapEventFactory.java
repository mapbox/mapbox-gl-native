package com.mapbox.mapboxsdk.module.telemetry;

import android.os.Bundle;
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
                                                                   long sizeOfResourcesCompleted,
                                                                   long numberOfTilesCompleted,
                                                                   @OfflineRegion.DownloadState int state) {

    OfflineDownloadEndEvent offlineEvent =
      new OfflineDownloadEndEvent(phoneState, shapeForOfflineRegion, minZoom, maxZoom);
    offlineEvent.setStyleURL(styleURL);
    offlineEvent.setSizeOfResourcesCompleted(sizeOfResourcesCompleted);
    offlineEvent.setNumberOfTilesCompleted(numberOfTilesCompleted);
    offlineEvent.setState(state);
    return offlineEvent;
  }

  static PerformanceEvent buildPerformanceEvent(@NonNull PhoneState phoneState, @NonNull String sessionId,
                                                @NonNull Bundle data) {
    return new PerformanceEvent(phoneState, sessionId, data);
  }

}