package com.mapbox.mapboxsdk.testapp.offline

import android.content.Context
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.AppCenter
import com.mapbox.mapboxsdk.offline.OfflineManager
import com.mapbox.mapboxsdk.offline.OfflineRegion
import com.mapbox.mapboxsdk.storage.FileSource
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity
import com.mapbox.mapboxsdk.testapp.utils.FileUtils
import org.junit.FixMethodOrder
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.MethodSorters
import java.io.IOException
import java.util.concurrent.CountDownLatch

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(AndroidJUnit4::class)
class OfflineManagerTest : AppCenter() {

  companion object {
    private const val TEST_DB_FILE_NAME = "offline_test.db"
    private lateinit var mergedRegion: OfflineRegion
  }

  @Rule
  @JvmField
  var rule = ActivityTestRule(FeatureOverviewActivity::class.java)

  private val context: Context by lazy { rule.activity }

  @Test(timeout = 30_000)
  fun a_copyFileFromAssets() {
    val latch = CountDownLatch(1)
    rule.runOnUiThread {
      FileUtils.CopyFileFromAssetsTask(rule.activity, object : FileUtils.OnFileCopiedFromAssetsListener {
        override fun onFileCopiedFromAssets() {
          latch.countDown()
        }

        override fun onError() {
          throw IOException("Unable to copy DB file.")
        }
      }).execute(TEST_DB_FILE_NAME, FileSource.getResourcesCachePath(rule.activity))
    }
    latch.await()
  }

  @Test(timeout = 30_000)
  fun b_mergeRegion() {
    val latch = CountDownLatch(1)
    rule.runOnUiThread {
      OfflineManager.getInstance(context).mergeOfflineRegions(
        FileSource.getResourcesCachePath(rule.activity) + "/" + TEST_DB_FILE_NAME,
        object : OfflineManager.MergeOfflineRegionsCallback {
          override fun onMerge(offlineRegions: Array<out OfflineRegion>?) {
            assert(offlineRegions?.size == 1)
            latch.countDown()
          }

          override fun onError(error: String?) {
            throw RuntimeException("Unable to merge external offline database. $error")
          }
        })
    }
    latch.await()
  }

  @Test(timeout = 30_000)
  fun c_listRegion() {
    val latch = CountDownLatch(1)
    rule.runOnUiThread {
      OfflineManager.getInstance(context).listOfflineRegions(object : OfflineManager.ListOfflineRegionsCallback {
        override fun onList(offlineRegions: Array<out OfflineRegion>?) {
          assert(offlineRegions?.size == 1)
          mergedRegion = offlineRegions!![0]
          latch.countDown()
        }

        override fun onError(error: String?) {
          throw RuntimeException("Unable to merge external offline database. $error")
        }
      })
    }
    latch.await()
  }

  @Test(timeout = 30_000)
  fun d_deleteRegion() {
    val latch = CountDownLatch(1)
    rule.runOnUiThread {
      mergedRegion.delete(object : OfflineRegion.OfflineRegionDeleteCallback {
        override fun onDelete() {
          latch.countDown()
        }

        override fun onError(error: String?) {
          throw RuntimeException("Unable to delete region")
        }
      })
    }
    latch.await()
  }
}