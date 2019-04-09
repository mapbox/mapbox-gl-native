package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Parcel;

import com.mapbox.android.telemetry.Event;

/**
 * Event will be sent while offline download end.
 */
@SuppressLint("ParcelCreator")
public class OfflineDownloadEndEvent extends Event {
  private final String event = "map.offlineDownload.end";
  private final String created;
  private final Double minZoom;
  private final Double maxZoom;
  private final String shapeForOfflineRegion;
  private String styleURL;
  private Long sizeOfResourcesCompleted;
  private Long numberOfTilesCompleted;
  private String state;

  void setStyleURL(String styleURL) {
    this.styleURL = styleURL;
  }

  void setSizeOfResourcesCompleted(Long sizeOfResourcesCompleted) {
    this.sizeOfResourcesCompleted = sizeOfResourcesCompleted;
  }

  void setNumberOfTilesCompleted(Long numberOfTilesCompleted) {
    this.numberOfTilesCompleted = numberOfTilesCompleted;
  }

  void setState(String state) {
    this.state = state;
  }

  OfflineDownloadEndEvent(PhoneState phoneState, String shapeForOfflineRegion, Double minZoom, Double maxZoom) {
    this.created = phoneState.getCreated();
    this.shapeForOfflineRegion = shapeForOfflineRegion;
    this.minZoom = minZoom;
    this.maxZoom = maxZoom;
  }

  String getCreated() {
    return created;
  }

  Double getMinZoom() {
    return minZoom;
  }

  Double getMaxZoom() {
    return maxZoom;
  }

  String getShapeForOfflineRegion() {
    return shapeForOfflineRegion;
  }

  String getStyleURL() {
    return styleURL;
  }

  Long getSizeOfResourcesCompleted() {
    return sizeOfResourcesCompleted;
  }

  Long getNumberOfTilesCompleted() {
    return numberOfTilesCompleted;
  }

  String getState() {
    return state;
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
  }
}
