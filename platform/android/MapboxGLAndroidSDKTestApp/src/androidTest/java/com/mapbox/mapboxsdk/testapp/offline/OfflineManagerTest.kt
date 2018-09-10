package com.mapbox.mapboxsdk.testapp.offline

import android.R
import android.content.Context
import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.IdlingRegistry
import android.support.test.espresso.UiController
import android.support.test.espresso.assertion.ViewAssertions.matches
import android.support.test.espresso.idling.CountingIdlingResource
import android.support.test.espresso.matcher.ViewMatchers.isDisplayed
import android.support.test.espresso.matcher.ViewMatchers.withId
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.offline.OfflineManager
import com.mapbox.mapboxsdk.offline.OfflineRegion
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity
import org.junit.Test
import org.junit.runner.RunWith
import java.io.File
import java.io.FileOutputStream

@RunWith(AndroidJUnit4::class)
class OfflineManagerTest : BaseActivityTest() {

  companion object {
    private const val TEST_DB_FILE_NAME = "offline.db"
  }

  private val context: Context by lazy { rule.activity }

  private lateinit var offlineIdlingResource: CountingIdlingResource

  override fun getActivityClass(): Class<*> {
    return EspressoTestActivity::class.java
  }

  override fun beforeTest() {
    super.beforeTest()
    offlineIdlingResource = CountingIdlingResource("idling_resource")
    IdlingRegistry.getInstance().register(offlineIdlingResource)
  }

  @Test
  fun offlineMergeListDeleteTest() {
    validateTestSetup()

    invoke(mapboxMap) { _: UiController, _: MapboxMap ->
      offlineIdlingResource.increment()
      copyAsset(context)
      OfflineManager.getInstance(context).mergeOfflineRegions(
        context.filesDir.absolutePath + "/" + TEST_DB_FILE_NAME,
        object : OfflineManager.MergeOfflineRegionsCallback {
          override fun onMerge(offlineRegions: Array<out OfflineRegion>?) {
            assert(offlineRegions?.size == 1)
            offlineIdlingResource.decrement()
          }

          override fun onError(error: String?) {
            throw RuntimeException("Unable to merge external offline database. $error")
          }
        })
    }

    invoke(mapboxMap) { _: UiController, _: MapboxMap ->
      offlineIdlingResource.increment()
      OfflineManager.getInstance(context).listOfflineRegions(object : OfflineManager.ListOfflineRegionsCallback {
        override fun onList(offlineRegions: Array<out OfflineRegion>?) {
          assert(offlineRegions?.size == 1)
          if (offlineRegions != null) {
            for (region in offlineRegions) {
              offlineIdlingResource.increment()
              region.delete(object : OfflineRegion.OfflineRegionDeleteCallback {
                override fun onDelete() {
                  offlineIdlingResource.decrement()
                }

                override fun onError(error: String?) {
                  throw RuntimeException("Unable to delete region with ID: ${region.id}. $error")
                }
              })
            }
          } else {
            throw RuntimeException("Unable to find merged region.")
          }
          offlineIdlingResource.decrement()
        }

        override fun onError(error: String?) {
          throw RuntimeException("Unable to obtain offline regions list. $error")
        }
      })
    }

    // waiting for offline idling resource
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  override fun afterTest() {
    super.afterTest()
    IdlingRegistry.getInstance().unregister(offlineIdlingResource)
  }

  private fun copyAsset(context: Context) {
    val bufferSize = 1024
    val assetManager = context.assets
    val inputStream = assetManager.open(TEST_DB_FILE_NAME)
    val outputStream = FileOutputStream(File(context.filesDir.absoluteFile, TEST_DB_FILE_NAME))

    try {
      inputStream.copyTo(outputStream, bufferSize)
    } finally {
      inputStream.close()
      outputStream.flush()
      outputStream.close()
    }
  }
}