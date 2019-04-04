package com.mapbox.mapboxsdk.module.telemetry;

import android.os.Parcel;
import android.os.Parcelable;

import com.google.gson.annotations.SerializedName;
import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.TelemetryUtils;


public class OfflineDownloadStartEvent extends Event implements Parcelable {

  private static final String OFFLINE_DOWNLOAD_START = "map.offlineDownload.start";

  @SerializedName("event")
  private final String event;

  @SerializedName("created")
  private final String created;

  @SerializedName("minZoom")
  private final Double minZoom;

  @SerializedName("maxZoom")
  private final Double maxZoom;

  @SerializedName("shapeForOfflineRegion")
  private final String shapeForOfflineRegion;

  @SerializedName("styleURL")
  private String styleURL;

  public void setStyleURL(String styleURL) {
    this.styleURL = styleURL;
  }

  OfflineDownloadStartEvent(String shapeForOfflineRegion, Double minZoom, Double maxZoom) {
    this.event = OFFLINE_DOWNLOAD_START;
    this.created = TelemetryUtils.obtainCurrentDate();
    this.shapeForOfflineRegion = shapeForOfflineRegion;
    this.minZoom = minZoom;
    this.maxZoom = maxZoom;
  }

  private OfflineDownloadStartEvent(Parcel in) {
    event = in.readString();
    created = in.readString();
    shapeForOfflineRegion = in.readString();
    minZoom = in.readDouble();
    maxZoom = in.readDouble();
    styleURL = in.readString();
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
    dest.writeString(event);
    dest.writeString(created);
    dest.writeString(shapeForOfflineRegion);
    dest.writeDouble(minZoom);
    dest.writeDouble(maxZoom);
    dest.writeString(styleURL);
  }

  public static final Creator<OfflineDownloadStartEvent> CREATOR = new Creator<OfflineDownloadStartEvent>() {
    @Override
    public OfflineDownloadStartEvent createFromParcel(Parcel in) {
      return new OfflineDownloadStartEvent(in);
    }

    @Override
    public OfflineDownloadStartEvent[] newArray(int size) {
      return new OfflineDownloadStartEvent[size];
    }
  };
}
