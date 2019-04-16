package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.support.annotation.FloatRange;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

/**
 * Event will be sent while offline download start.
 */
@SuppressLint("ParcelCreator")
public class OfflineDownloadStartEvent extends MapBaseEvent {
  private static final String EVENT_NAME = "map.offlineDownload.start";
  private final Double minZoom;
  private final Double maxZoom;
  private final String shapeForOfflineRegion;
  private String styleURL;

  OfflineDownloadStartEvent(PhoneState phoneState, String shapeForOfflineRegion,
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

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    OfflineDownloadStartEvent that = (OfflineDownloadStartEvent) o;

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
    return result;
  }

  @Override
  public String toString() {
    return "OfflineDownloadStartEvent{"
      + "event='" + event + '\''
      + ", created='" + created + '\''
      + ", minZoom=" + minZoom
      + ", maxZoom=" + maxZoom
      + ", shapeForOfflineRegion='" + shapeForOfflineRegion + '\''
      + ", styleURL='" + styleURL + '\''
      + '}';
  }

}
