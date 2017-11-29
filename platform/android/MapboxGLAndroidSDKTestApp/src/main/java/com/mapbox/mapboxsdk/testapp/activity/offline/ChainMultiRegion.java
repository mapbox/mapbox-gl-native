package com.mapbox.mapboxsdk.testapp.activity.offline;


import android.util.Log;

import com.mapbox.mapboxsdk.geometry.LatLngBounds;


import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

import timber.log.Timber;

public class ChainMultiRegion implements Region, Region.OnStatusChangeListener {

  private static final String TAG = "TEST-OFFLINE";

  private final String name;
  private final List<Region> regions;
  private Region curRegion = null;
  private LatLngBounds bounds = null;
  private OnStatusChangeListener listener;

  public ChainMultiRegion(List<Region> regionsList) {
    if (regionsList == null && regionsList.size() > 0) {
      name = "";
      regions = new ArrayList<>();
    } else {
      this.name = (regionsList.get(0).getName());
      regions = new ArrayList<>(regionsList.size());
      regions.addAll(regionsList);
    }
  }

  public ChainMultiRegion(String name, List<Region> regionsList) {
    this.name = name;
    regions = new ArrayList<>(regionsList == null ? 0 : regionsList.size());
    regions.addAll(regionsList);
  }


  @Override
  public String getName() {
    return name;
  }

  @Override
  public void startDownload() {
    startMeasuringDownload();

    // find the first region that was not downloaded yet
    for (Region region : regions) {
      if (!region.isComplete()) {
        curRegion = region;
        curRegion.startDownload();
        return;
      }
    }
  }

  @Override
  public void stopDownload() {
    for (Region region : regions) {
      region.stopDownload();
    }
    curRegion = null;
    stopMeasuringDownload();
  }

  @Override
  public void onStatusChanged(Region region) {
    if (this.listener != null) {

      if (curRegion == region && region.isComplete()) {

        Log.d(TAG, " >>>>> Another Region COMPLETED: completeCount=" + curRegion.getRequiredResourceCount()+
          " size=" + getSize(curRegion.getCompletedResourceSize()));
        curRegion.stopDownload();
        curRegion = null;

        // find the first region that was not downloaded yet
        for (Region nextRegion : regions) {
          if (!nextRegion.isComplete()) {
            curRegion = nextRegion;
            curRegion.startDownload();
            return;
          }
        }

        if (isComplete()) {
          stopMeasuringDownload();
        }
      }

      this.listener.onStatusChanged(ChainMultiRegion.this);
    }
  }

  @Override
  public void startTrackingStatus(OnStatusChangeListener listener) {
    this.listener = listener;
    for (Region region : regions) {
      region.startTrackingStatus(this);
    }
  }

  @Override
  public void stopTrackingStatus() {
    this.listener = null;
    for (Region region : regions) {
      region.stopTrackingStatus();
    }
  }

  public boolean isComplete() {
    for (Region region : regions) {
      if (!region.isComplete()) {
        return false;
      }
    }
    return true;
  }

  @Override
  public boolean isDownloadStarted() {
    for (Region region : regions) {
      if (region.isDownloadStarted()) {
        return true;
      }
    }
    return false;
  }

  @Override
  public long getRequiredResourceCount() {
    long resCount = 0;
    for (Region region : regions) {
      resCount += region.getRequiredResourceCount();
    }
    return resCount;
  }

  @Override
  public long getCompletedResourceCount() {
    long resCount = 0; int i=0;
    for (Region region : regions) {
      resCount += region.getCompletedResourceCount();
    }
    return resCount;
  }

  @Override
  public long getCompletedResourceSize() {
    long resCount = 0;
    for (Region region : regions) {
      resCount += region.getCompletedResourceSize();
    }
    return resCount;
  }

  @Override
  public LatLngBounds getBounds() {
    if (bounds == null) {
      bounds = regions.get(0).getBounds();
      bounds = LatLngBounds.from(bounds.getLatNorth(), bounds.getLonEast(),
        bounds.getLatSouth(), bounds.getLonWest());

      for(int i = 1; i < regions.size(); i++) {
        bounds.union(regions.get(i).getBounds());
      }
    }
    return bounds;
  }

  @Override
  public String getStyleURL() {
    String styleUrl = regions.get(0).getStyleURL();
    for (int i = 1; i < regions.size(); i++) {
      Region region = regions.get(i);
      if (!styleUrl.equals(region.getStyleURL())) {
        throw new IllegalArgumentException();
      }
    }
    return  styleUrl;
  }

  @Override
  public double getMinZoom() {
    double minZoom = 0;
    for (Region region : regions) {
      if (region.getMinZoom() > minZoom) {
        minZoom = region.getMinZoom();
      }
    }

    return minZoom;
  }

  @Override
  public double getMaxZoom() {
    double maxZoom = regions.get(0).getMaxZoom();
    for (int i = 1; i < regions.size(); i++) {
      Region region = regions.get(i);
      if (region.getMaxZoom() < maxZoom) {
        maxZoom = region.getMaxZoom();
      }
    }

    return maxZoom;
  }

  private long startTime = 0;
  private long downloadTime = 0;

  @Override
  public long getDownloadTime() {
    if (startTime > 0) {
      downloadTime = System.currentTimeMillis() - startTime;
    }
    return downloadTime;
  }

  private void startMeasuringDownload() {
    startTime = System.currentTimeMillis();
  }

  private void stopMeasuringDownload() {
    downloadTime = System.currentTimeMillis() - startTime;

    Log.d("TEST-OFFLINE", " >>>>> It took " +
      TimeUnit.MILLISECONDS.toMinutes(downloadTime) + " minutes to load " +
      getSize(getCompletedResourceSize()) + " the map of " +name);

    startTime = 0;
  }

  private static String getSize(long size) {
    if (size == 0) {
      return "0 B";
    } else if (size < 1024) {
      return size + " B";
    } else if (size < 1048576){
      return size / 1024 + " KB";
    } else {
      return size /1048576 + " MB";
    }
  }

}
