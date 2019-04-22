package com.mapbox.mapboxsdk.testapp.storage

import android.os.Handler
import android.support.test.annotation.UiThreadTest
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.storage.FileSource
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity
import org.junit.*
import org.junit.rules.TestName
import org.junit.runner.RunWith
import java.util.concurrent.CountDownLatch

@RunWith(AndroidJUnit4::class)
class FileSourceStandaloneTest {

  private lateinit var fileSourceTestUtils: FileSourceTestUtils
  private lateinit var fileSource: FileSource

  @get:Rule
  val rule = ActivityTestRule(FeatureOverviewActivity::class.java)

  @get:Rule
  val testName = TestName()

  @Before
  @UiThreadTest
  fun setup() {
    fileSource = FileSource.getInstance(rule.activity)
    fileSourceTestUtils = FileSourceTestUtils(rule.activity)
    fileSourceTestUtils.setup()
  }

  @Test
  @UiThreadTest
  fun testDefault() {
    Assert.assertFalse("FileSource should not be active", fileSource.isActivated)
  }

  @Test
  @UiThreadTest
  fun testActivateDeactivate() {
    Assert.assertFalse("1) FileSource should not be active", fileSource.isActivated)
    fileSource.activate()
    Assert.assertTrue("2) FileSource should be active", fileSource.isActivated)
    fileSource.deactivate()
    Assert.assertFalse("3) FileSource should not be active", fileSource.isActivated)
  }

  @Test
  fun pathChangeTest() {
    Assert.assertFalse("FileSource should not be active", fileSource.isActivated)
    Assert.assertEquals(fileSourceTestUtils.originalPath, FileSource.getResourcesCachePath(rule.activity))

    fileSourceTestUtils.changePath(fileSourceTestUtils.testPath)
    Assert.assertEquals(fileSourceTestUtils.testPath, FileSource.getResourcesCachePath(rule.activity))

    // workaround for https://github.com/mapbox/mapbox-gl-native/issues/14334
    val latch = CountDownLatch(1)
    rule.activity.runOnUiThread {
      fileSource.activate()
      Handler().postDelayed({
        fileSource.deactivate()
        latch.countDown()
      }, 2000)
    }
    latch.await()

    fileSourceTestUtils.changePath(fileSourceTestUtils.originalPath)
    Assert.assertEquals(fileSourceTestUtils.originalPath, FileSource.getResourcesCachePath(rule.activity))
  }

  @After
  fun cleanup() {
    fileSourceTestUtils.cleanup()
  }
}