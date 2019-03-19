package com.mapbox.mapboxsdk.testapp.offline

import android.content.Context
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.offline.OfflineManager
import com.mapbox.mapboxsdk.offline.OfflineRegion
import com.mapbox.mapboxsdk.storage.FileSource
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity
import com.mapbox.mapboxsdk.testapp.utils.FileUtils
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import java.io.IOException
import java.util.concurrent.CountDownLatch

@RunWith(AndroidJUnit4::class)
class OfflineManagerTest {

  companion object {
    private const val TEST_DB_FILE_NAME = "offline_test.db"
  }

  @Rule
  @JvmField
  var rule = ActivityTestRule(FeatureOverviewActivity::class.java)

  private val context: Context by lazy { rule.activity }
  private val latch: CountDownLatch = CountDownLatch(1)

  @Test
  fun offlineMergeListDeleteTest() {
    rule.runOnUiThread {
      FileUtils.CopyFileFromAssetsTask(rule.activity, object : FileUtils.OnFileCopiedFromAssetsListener {
        override fun onFileCopiedFromAssets() {
          mergeRegion()
        }

        override fun onError() {
          throw IOException("Unable to copy DB file.")
        }
      }).execute(TEST_DB_FILE_NAME, FileSource.getResourcesCachePath(rule.activity))
    }
    latch.await()
  }

  private fun mergeRegion() {
    OfflineManager.getInstance(context).mergeOfflineRegions(
      FileSource.getResourcesCachePath(rule.activity) + "/" + TEST_DB_FILE_NAME,
      object : OfflineManager.MergeOfflineRegionsCallback {
        override fun onMerge(offlineRegions: Array<out OfflineRegion>?) {
          assert(offlineRegions?.size == 1)
          listRegion()
        }

        override fun onError(error: String?) {
          throw RuntimeException("Unable to merge external offline database. $error")
        }
      })
  }

  private fun listRegion() {
    OfflineManager.getInstance(context).listOfflineRegions(object : OfflineManager.ListOfflineRegionsCallback {
      override fun onList(offlineRegions: Array<out OfflineRegion>?) {
        assert(offlineRegions?.size == 1)
        deleteRegion(offlineRegions!![0])
      }
      override fun onError(error: String?) {
        throw RuntimeException("Unable to merge external offline database. $error")
      }
    })
  }

  private fun deleteRegion(offlineRegion: OfflineRegion) {
    offlineRegion.delete(object : OfflineRegion.OfflineRegionDeleteCallback {
      override fun onDelete() {
        latch.countDown()
      }

      override fun onError(error: String?) {
        throw RuntimeException("Unable to delete region")
      }
    })
  }
}