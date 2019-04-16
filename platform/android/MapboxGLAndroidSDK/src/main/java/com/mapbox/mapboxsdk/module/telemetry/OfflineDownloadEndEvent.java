package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.support.annotation.FloatRange;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.offline.OfflineRegion;

/**
 * Event will be sent while offline download end.
 */
@SuppressLint("ParcelCreator")
public class OfflineDownloadEndEvent extends MapBaseEvent {
  private static final String EVENT_NAME = "map.offlineDownload.end";
  private final Double minZoom;
  private final Double maxZoom;
  private final String shapeForOfflineRegion;
  private String styleURL;
  private long sizeOfResourcesCompleted;
  private long numberOfTilesCompleted;
  private int state;

  OfflineDownloadEndEvent(PhoneState phoneState, String shapeForOfflineRegion,
                          @FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
                            to = MapboxConstants.MAXIMUM_ZOOM) Double minZoom,
                          @FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
                            to = MapboxConstants.MAXIMUM_ZOOM) Double maxZoom) {
    super(phoneState);
    this.shapeForOfflineRegion = shapeForOfflineRegion;
    this.minZoom = minZoom;
    this.maxZoom = maxZoom;
  }

  @Override
  String getEventName() {
    return EVENT_NAME;
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

  void setStyleURL(String styleURL) {
    this.styleURL = styleURL;
  }

  long getSizeOfResourcesCompleted() {
    return sizeOfResourcesCompleted;
  }

  void setSizeOfResourcesCompleted(long sizeOfResourcesCompleted) {
    this.sizeOfResourcesCompleted = sizeOfResourcesCompleted;
  }

  long getNumberOfTilesCompleted() {
    return numberOfTilesCompleted;
  }

  void setNumberOfTilesCompleted(long numberOfTilesCompleted) {
    this.numberOfTilesCompleted = numberOfTilesCompleted;
  }

  int getState() {
    return state;
  }

  void setState(@OfflineRegion.DownloadState int state) {
    this.state = state;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    OfflineDownloadEndEvent that = (OfflineDownloadEndEvent) o;

    if (sizeOfResourcesCompleted != that.sizeOfResourcesCompleted) {
      return false;
    }
    if (numberOfTilesCompleted != that.numberOfTilesCompleted) {
      return false;
    }
    if (state != that.state) {
      return false;
    }
    if (created != null ? !created.equals(that.created) : that.created != null) {
      return false;
    }
    if (minZoom != null ? !minZoom.equals(that.minZoom) : that.minZoom != null) {
      return false;
    }
    if (maxZoom != null ? !maxZoom.equals(that.maxZoom) : that.maxZoom != null) {
      return false;
    }
    if (shapeForOfflineRegion != null ? !shapeForOfflineRegion.equals(that.shapeForOfflineRegion) :
      that.shapeForOfflineRegion != null) {
      return false;
    }
    return styleURL != null ? styleURL.equals(that.styleURL) : that.styleURL == null;
  }

  @Override
  public int hashCode() {
    int result = event.hashCode();
    result = 31 * result + (created != null ? created.hashCode() : 0);
    result = 31 * result + (minZoom != null ? minZoom.hashCode() : 0);
    result = 31 * result + (maxZoom != null ? maxZoom.hashCode() : 0);
    result = 31 * result + (shapeForOfflineRegion != null ? shapeForOfflineRegion.hashCode() : 0);
    result = 31 * result + (styleURL != null ? styleURL.hashCode() : 0);
    result = 31 * result + (int) (sizeOfResourcesCompleted ^ (sizeOfResourcesCompleted >>> 32));
    result = 31 * result + (int) (numberOfTilesCompleted ^ (numberOfTilesCompleted >>> 32));
    result = 31 * result + state;
    return result;
  }

  @Override
  public String toString() {
    return "OfflineDownloadEndEvent{"
      + "event='" + event + '\''
      + ", created='" + created + '\''
      + ", minZoom=" + minZoom
      + ", maxZoom=" + maxZoom
      + ", shapeForOfflineRegion='" + shapeForOfflineRegion + '\''
      + ", styleURL='" + styleURL + '\''
      + ", sizeOfResourcesCompleted=" + sizeOfResourcesCompleted
      + ", numberOfTilesCompleted=" + numberOfTilesCompleted
      + ", state=" + state
      + '}';
  }
}
