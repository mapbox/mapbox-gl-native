package com.mapbox.mapboxsdk.testapp.activity.offline

import android.os.Bundle
import com.mapbox.mapboxsdk.geometry.LatLngBounds
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.offline.*
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_download_not_completing.*
import timber.log.Timber
import java.util.concurrent.TimeUnit

private const val STOP_INTERVAL = 1000L
private const val STATUS_UPDATE_TIMEOUT_MS = 30_000L

class DownloadNotCompletingActivity : BaseOfflineTestActivity(), OfflineRegion.OfflineRegionObserver {

  private var offlineRegion: OfflineRegion? = null
  private var previousCompletedResourceCount: Long = 0
  private var previousUpdateTimestamp: Long = 0
  private var nextStop = STOP_INTERVAL

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)

    setContentView(R.layout.activity_download_not_completing)

    deleteOldOfflineRegions {
      createOfflineRegion()
    }
  }

  override fun onDestroy() {
    offlineRegion?.let {
      it.setObserver(null)

      it.delete(object : OfflineRegion.OfflineRegionDeleteCallback {
        override fun onDelete() {
        }

        override fun onError(error: String?) {
        }
      })
    }

    super.onDestroy()
  }

  private fun createOfflineRegion() {
    val definition = OfflineTilePyramidRegionDefinition(
      Style.OUTDOORS,
      LatLngBounds.from(61.3, 24.0, 61.0, 23.6),
      1.0,
      15.0,
      resources.displayMetrics.density)

    logMessage("Creating offline region")
    offlineManager.createOfflineRegion(definition, byteArrayOf(), object : OfflineManager.CreateOfflineRegionCallback {
      override fun onCreate(region: OfflineRegion) {
        logMessage("Region with id ${region.id} created")
        offlineRegion = region

        startDownload(region)
      }

      override fun onError(error: String) {
        logMessage("Failed to create offline region: $error")
      }
    })
  }

  private fun startDownload(region: OfflineRegion) {
    logMessage("Downloading...")

    region.setObserver(this)
    region.setDownloadState(OfflineRegion.STATE_ACTIVE)

    previousUpdateTimestamp = System.currentTimeMillis()
    handler.postDelayed(object : Runnable {
      override fun run() {
        if (System.currentTimeMillis() > previousUpdateTimestamp + STATUS_UPDATE_TIMEOUT_MS) {
          logMessage("FAILURE! No progress in ${TimeUnit.MILLISECONDS.toSeconds(STATUS_UPDATE_TIMEOUT_MS)} seconds")
        } else {
          handler.postDelayed(this, 500)
        }
      }
    }, 500)
  }

  private fun pauseDownload(region: OfflineRegion) {
    handler.removeCallbacksAndMessages(null)

    region.setDownloadState(OfflineRegion.STATE_INACTIVE)
    handler.postDelayed({ startDownload(region) }, 500)

    "Paused".let {
      logMessage(it)
      download_status.text = it
    }
  }

  override fun onStatusChanged(status: OfflineRegionStatus) {
    val downloadProgressText = "Downloaded ${status.completedResourceCount}/${status.requiredResourceCount}"
    Timber.d(downloadProgressText)
    if (status.isComplete) {
      if (status.downloadState == OfflineRegion.STATE_INACTIVE) {
        val statusText = "Completed"
        logMessage("SUCCESS! $statusText")
        download_status.text = statusText
        offlineRegion?.setObserver(null)
        handler.removeCallbacksAndMessages(null)
      } else {
        logMessage("Completed, inactivating download")
        offlineRegion?.setDownloadState(OfflineRegion.STATE_INACTIVE)
      }
    } else {
      download_status.text = downloadProgressText
      if (previousCompletedResourceCount < nextStop && status.completedResourceCount >= nextStop) {
        nextStop += STOP_INTERVAL
        offlineRegion?.let { pauseDownload(it) }
      }
    }

    if (status.completedResourceCount > status.requiredResourceCount &&
      status.isRequiredResourceCountPrecise &&
      previousCompletedResourceCount <= status.requiredResourceCount) {
      logMessage("FAILURE! Completed > required")
    }

    previousCompletedResourceCount = status.completedResourceCount
    previousUpdateTimestamp = System.currentTimeMillis()
  }

  override fun onError(error: OfflineRegionError) {
    logMessage("Error: $error")
  }

  override fun mapboxTileCountLimitExceeded(limit: Long) {
    logMessage("Error: tile count limit exceeded")
  }
}
