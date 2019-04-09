package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Parcel;

import com.mapbox.android.telemetry.Event;

/**
 * Event will be sent while offline download start.
 */
@SuppressLint("ParcelCreator")
public class OfflineDownloadStartEvent extends Event {
  private final String event = "map.offlineDownload.start";
  private final String created;
  private final Double minZoom;
  private final Double maxZoom;
  private final String shapeForOfflineRegion;
  private String styleURL;

  void setStyleURL(String styleURL) {
    this.styleURL = styleURL;
  }

  OfflineDownloadStartEvent(PhoneState phoneState, String shapeForOfflineRegion, Double minZoom, Double maxZoom) {
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

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
  }
}
