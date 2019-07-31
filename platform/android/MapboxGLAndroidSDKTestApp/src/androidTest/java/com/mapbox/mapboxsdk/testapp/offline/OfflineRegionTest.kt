package com.mapbox.mapboxsdk.testapp.offline

import android.content.Context
import android.os.Handler
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.geojson.Geometry
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.log.Logger
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.offline.*
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import java.util.concurrent.CountDownLatch

@RunWith(AndroidJUnit4::class)
class OfflineRegionTest {

  @Rule
  @JvmField
  var rule = ActivityTestRule(FeatureOverviewActivity::class.java)

  private val context: Context by lazy { rule.activity }

  private val latch: CountDownLatch = CountDownLatch(1)

  private lateinit var handler: Handler

  @Test
  fun testRegionDownload() {
    rule.activity.runOnUiThread {
      handler = Handler()
      val definition = OfflineGeometryRegionDefinition(
        Style.MAPBOX_STREETS,
        Point.fromLngLat(0.0, 0.0) as Geometry,
        5.0,
        6.0,
        1.0f
      )
      OfflineManager.getInstance(context).createOfflineRegion(definition, byteArrayOf(), object : OfflineManager.CreateOfflineRegionCallback {
        override fun onCreate(offlineRegion: OfflineRegion) {
          offlineRegion.setObserver(object : OfflineRegion.OfflineRegionObserver {
            override fun onStatusChanged(status: OfflineRegionStatus) {
              if (status.isComplete) {
                Logger.e(TAG, "Download is complete")
                latch.countDown()
              } else {
                Logger.i(TAG, "OnStatusChanged: ${status.completedResourceCount} / ${status.requiredResourceCount}")
              }
            }

            override fun onError(error: OfflineRegionError) {
              Logger.e(TAG, error.message)
            }

            override fun mapboxTileCountLimitExceeded(limit: Long) {
              Logger.e(TAG, "Exceeded limit tile count: $limit")
            }
          })
          offlineRegion.setDownloadState(OfflineRegion.STATE_ACTIVE)
          toggle(offlineRegion)
        }

        override fun onError(error: String) {
          Logger.e(TAG, "Exceeded limit tile count: $error")
        }
      })
    }
    latch.await()
  }

  private fun toggle(offlineRegion: OfflineRegion) {
    handler.postDelayed({
      Logger.e(TAG, "Pausing download!")
      offlineRegion.setDownloadState(OfflineRegion.STATE_INACTIVE)
      handler.postDelayed({
        Logger.e(TAG, "Restarting download!")
        offlineRegion.setDownloadState(OfflineRegion.STATE_ACTIVE)
        toggle(offlineRegion)
      }, 5000)
    }, 5000)
  }

  companion object {
    const val TAG = "OfflineTest"
  }
}