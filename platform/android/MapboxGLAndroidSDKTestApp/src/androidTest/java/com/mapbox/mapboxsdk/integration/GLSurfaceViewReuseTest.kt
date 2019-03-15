package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.runner.AndroidJUnit4
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates if a GLSurfaceView surface can be recreated without crashing.
 */
@RunWith(AndroidJUnit4::class)
class GLSurfaceViewReuseTest : BaseIntegrationTest() {

  @Before
  override fun beforeTest() {
    super.beforeTest()
    openFeature("RecyclerView GLSurfaceView")
  }

  @Test
  @LargeTest
  fun scrollRecylerView() {
    device.waitForIdle()
    scrollRecyclerViewTo("Twenty-one")
    device.waitForIdle()
    scrollRecyclerViewTo("One")
    device.waitForIdle()
  }
}