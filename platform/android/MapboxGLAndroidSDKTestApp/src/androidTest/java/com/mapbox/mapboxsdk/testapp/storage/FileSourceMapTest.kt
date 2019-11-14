package com.mapbox.mapboxsdk.testapp.storage

import androidx.test.annotation.UiThreadTest
import androidx.test.rule.ActivityTestRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.mapbox.mapboxsdk.AppCenter
import com.mapbox.mapboxsdk.storage.FileSource
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity
import java.util.concurrent.CountDownLatch
import junit.framework.Assert
import org.junit.After
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.rules.TestName
import org.junit.runner.RunWith

@RunWith(androidx.test.ext.junit.runners.AndroidJUnit4::class)
open class FileSourceMapTest : AppCenter() {

  private lateinit var fileSourceTestUtils: FileSourceTestUtils

  @get:Rule
  val rule = ActivityTestRule(EspressoTestActivity::class.java)

  @get:Rule
  val testName = TestName()

  @Before
  @UiThreadTest
  fun setup() {
    fileSourceTestUtils = FileSourceTestUtils(rule.activity)
    fileSourceTestUtils.setup()
  }

  @Test
  fun changeResourcesPathWhileMapVisible() {
    val latch = CountDownLatch(1)
    rule.activity.runOnUiThread {
      FileSource.setResourcesCachePath(fileSourceTestUtils.testPath, object : FileSource.ResourcesCachePathChangeCallback {
        override fun onSuccess(path: String) {
          Assert.fail("Requested resources change while the map is running should fail")
        }

        override fun onError(message: String) {
          Assert.assertEquals("Cannot set path, file source is activated." +
            " Make sure that the map or a resources download is not running.", message)
          latch.countDown()
        }
      })
    }
    latch.await()
  }

  @After
  fun cleanup() {
    fileSourceTestUtils.cleanup()
  }
}