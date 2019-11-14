package com.mapbox.mapboxsdk.offline

import androidx.test.rule.ActivityTestRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.log.Logger
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import java.util.concurrent.TimeoutException
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Integration test that validates downloading an offline region from a point geometry at zoomlevel 17
 */
@RunWith(androidx.test.ext.junit.runners.AndroidJUnit4::class)
class OfflineDownloadTest : OfflineRegion.OfflineRegionObserver {

  @Rule
  @JvmField
  var rule = ActivityTestRule(FeatureOverviewActivity::class.java)

  private val countDownLatch = CountDownLatch(1)
  private lateinit var offlineRegion: OfflineRegion

  @Test(timeout = 60000)
  fun offlineDownload() {
    rule.runOnUiThreadActivity {
      OfflineManager.getInstance(rule.activity).createOfflineRegion(
        createTestRegionDefinition(),
        ByteArray(0),
        object : OfflineManager.CreateOfflineRegionCallback {
          override fun onCreate(region: OfflineRegion) {
            offlineRegion = region
            offlineRegion.setDownloadState(OfflineRegion.STATE_ACTIVE)
            offlineRegion.setObserver(this@OfflineDownloadTest)
          }

          override fun onError(error: String) {
            Logger.e(TAG, "Error while creating offline region: $error")
          }
        })
    }

    if (!countDownLatch.await(60, TimeUnit.SECONDS)) {
      throw TimeoutException()
    }
  }

  override fun onStatusChanged(status: OfflineRegionStatus) {
    Logger.i(TAG, "Download percentage ${100.0 * status.completedResourceCount / status.requiredResourceCount}")
    if (status.isComplete) {
      offlineRegion.setDownloadState(OfflineRegion.STATE_INACTIVE)
      countDownLatch.countDown()
    }
  }

  override fun onError(error: OfflineRegionError) {
    Logger.e(TAG, "Error while downloading offline region: $error")
  }

  override fun mapboxTileCountLimitExceeded(limit: Long) {
    Logger.e(TAG, "Tile count limited exceeded: $limit")
  }

  private fun createTestRegionDefinition(): OfflineRegionDefinition {
    return OfflineGeometryRegionDefinition(
      Style.MAPBOX_STREETS,
      Point.fromLngLat(50.847857, 4.360137),
      17.0,
      17.0,
      1.0f,
      false
    )
  }

  companion object {
    const val TAG = "OfflineDownloadTest"
  }
}

fun ActivityTestRule<*>.runOnUiThreadActivity(runnable: () -> Unit) = activity.runOnUiThread(runnable)