package com.mapbox.mapboxsdk.testapp.activity.offline;


import com.mapbox.mapboxsdk.geometry.LatLngBounds;


public interface  Region {

  interface OnStatusChangeListener {
    void onStatusChanged(Region region);
  }

  public String getName();

  public LatLngBounds getBounds();

  public String getStyleURL();

  public double getMinZoom();

  public double getMaxZoom();

  public void startDownload();

  public void stopDownload();

  public long getDownloadTime();

  public void startTrackingStatus(OnStatusChangeListener listener);

  public void stopTrackingStatus();

  public boolean isComplete();

  public boolean isDownloadStarted();

  public long getRequiredResourceCount();

  public long getCompletedResourceCount();

  public  long getCompletedResourceSize();

}
