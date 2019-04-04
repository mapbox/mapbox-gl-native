package com.mapbox.mapboxsdk.module.telemetry;

import android.os.Parcel;
import android.os.Parcelable;

import com.google.gson.annotations.SerializedName;
import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.TelemetryUtils;


public class OfflineDownloadEndEvent extends Event implements Parcelable {

  private static final String OFFLINE_DOWNLOAD_COMPLETE = "map.offlineDownload.end";

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

  @SerializedName("sizeOfResourcesCompleted")
  private Long sizeOfResourcesCompleted;

  @SerializedName("numberOfTilesCompleted")
  private Long numberOfTilesCompleted;

  @SerializedName("state")
  private String state;

  public void setStyleURL(String styleURL) {
    this.styleURL = styleURL;
  }

  public void setSizeOfResourcesCompleted(Long sizeOfResourcesCompleted) {
    this.sizeOfResourcesCompleted = sizeOfResourcesCompleted;
  }

  public void setNumberOfTilesCompleted(Long numberOfTilesCompleted) {
    this.numberOfTilesCompleted = numberOfTilesCompleted;
  }

  public void setState(String state) {
    this.state = state;
  }

  OfflineDownloadEndEvent(String shapeForOfflineRegion, Double minZoom, Double maxZoom) {
    this.event = OFFLINE_DOWNLOAD_COMPLETE;
    this.created = TelemetryUtils.obtainCurrentDate();
    this.shapeForOfflineRegion = shapeForOfflineRegion;
    this.minZoom = minZoom;
    this.maxZoom = maxZoom;
  }

  private OfflineDownloadEndEvent(Parcel in) {
    event = in.readString();
    created = in.readString();
    shapeForOfflineRegion = in.readString();
    minZoom = in.readDouble();
    maxZoom = in.readDouble();
    styleURL = in.readString();
    sizeOfResourcesCompleted = in.readLong();
    numberOfTilesCompleted = in.readLong();
    state = in.readString();

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
    dest.writeLong(sizeOfResourcesCompleted);
    dest.writeLong(numberOfTilesCompleted);
    dest.writeString(state);
  }

  public static final Creator<OfflineDownloadEndEvent> CREATOR = new Creator<OfflineDownloadEndEvent>() {
    @Override
    public OfflineDownloadEndEvent createFromParcel(Parcel in) {
      return new OfflineDownloadEndEvent(in);
    }

    @Override
    public OfflineDownloadEndEvent[] newArray(int size) {
      return new OfflineDownloadEndEvent[size];
    }
  };
}
