package com.mapbox.mapboxsdk.testapp.activity.offline;

import android.util.Log;

import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.offline.OfflineRegion;
import com.mapbox.mapboxsdk.offline.OfflineRegionError;
import com.mapbox.mapboxsdk.offline.OfflineRegionStatus;
import com.mapbox.mapboxsdk.offline.OfflineTilePyramidRegionDefinition;
import com.mapbox.mapboxsdk.testapp.utils.OfflineUtils;

import java.util.concurrent.TimeUnit;

public class SingleRegion implements Region {

  private static final String TAG = "TEST-OFFLINE";

  private final String name;
  private final OfflineRegion offlineRegion;
  private OfflineRegionStatus lastReportedStatus = null;
  private OnStatusChangeListener listener = null;

  public SingleRegion(OfflineRegion offlineRegion) {
    this.name = OfflineUtils.convertRegionName(offlineRegion.getMetadata());
    this.offlineRegion = offlineRegion;
  }

  public SingleRegion(String name, OfflineRegion offlineRegion) {
    this.name = name;
    this.offlineRegion = offlineRegion;
  }

  @Override
  public  String getName() {
    return name;
  }

  @Override
  public void startDownload() {
    Log.d(TAG, ">>>>> START DOWNLOAD name=" +name);
    offlineRegion.setDownloadState(OfflineRegion.STATE_ACTIVE);
    startMeasuringDownload();
  }

  @Override
  public void stopDownload() {
    Log.d(TAG, ">>>>> STOP DOWNLOAD name=" +name);
    offlineRegion.setDownloadState(OfflineRegion.STATE_INACTIVE);
    stopMeasuringDownload();
  }

  @Override
  public void startTrackingStatus(OnStatusChangeListener listener) {

    Log.d(TAG, ">>>>> Start Tracking name=" +name);
    this.listener = listener;

    // Get observing offline region's status.
   offlineRegion.getStatus(new OfflineRegion.OfflineRegionStatusCallback() {
      @Override
      public void onStatus(OfflineRegionStatus status) {

        lastReportedStatus = status;

        if (SingleRegion.this.listener != null) {
          SingleRegion.this.listener.onStatusChanged(SingleRegion.this);
        }
      }

      @Override
      public void onError(String error) {
        Log.d(TAG, " Errro  adding region : "+ error);
      }
    });

    //Start observing offline region's status
    offlineRegion.setObserver(new OfflineRegion.OfflineRegionObserver() {
      @Override
      public void onStatusChanged(OfflineRegionStatus status) {

        lastReportedStatus = status;

        if (SingleRegion.this.listener != null) {
          SingleRegion.this.listener.onStatusChanged(SingleRegion.this);
        }

        // Stop measuring downlaod !
        if (status.isComplete()) {
          stopMeasuringDownload();
        }
      }

      @Override
      public void onError(OfflineRegionError error) {
        Log.d(TAG, ">>>>> OfflineRegionError : reason=" +error.getReason() +" msg=" +error.getMessage());

      }

      @Override
      public void mapboxTileCountLimitExceeded(long limit) {
        Log.d(TAG, ">>>>> mapboxTileCountLimitExceeded " + limit);

      }
    });
  }

  @Override
  public void stopTrackingStatus() {
    Log.d(TAG, ">>>>> Stop Tracking name=" +name);
    this.listener = null;
    offlineRegion.setObserver(null);
  }

  @Override
  public LatLngBounds getBounds() {
    return offlineRegion.getDefinition().getBounds();
  }

  @Override
  public String getStyleURL() {
    OfflineTilePyramidRegionDefinition definition =
      (OfflineTilePyramidRegionDefinition)offlineRegion.getDefinition();
    return definition.getStyleURL();
  }

  @Override
  public double getMinZoom() {
    OfflineTilePyramidRegionDefinition definition =
      (OfflineTilePyramidRegionDefinition)offlineRegion.getDefinition();
    return definition.getMinZoom();
  }

  @Override
  public double getMaxZoom() {
    OfflineTilePyramidRegionDefinition definition =
      (OfflineTilePyramidRegionDefinition)offlineRegion.getDefinition();
    return definition.getMaxZoom();
  }


  @Override
  public boolean isComplete() {
    if (lastReportedStatus != null) {
      return lastReportedStatus.isComplete();
    }

    return false;
  }

  @Override
  public boolean isDownloadStarted() {
    if (lastReportedStatus != null) {
      return lastReportedStatus.getDownloadState() == OfflineRegion.STATE_ACTIVE;
    }

    return false;
  }

  @Override
  public long getRequiredResourceCount() {
    return lastReportedStatus == null ? 0 : lastReportedStatus.getRequiredResourceCount();
  }

  @Override
  public long getCompletedResourceCount() {
    return lastReportedStatus == null ? 0 : lastReportedStatus.getCompletedResourceCount();
  }

  @Override
  public long getCompletedResourceSize() {
    return lastReportedStatus == null ? 0 : lastReportedStatus.getCompletedResourceSize();
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
