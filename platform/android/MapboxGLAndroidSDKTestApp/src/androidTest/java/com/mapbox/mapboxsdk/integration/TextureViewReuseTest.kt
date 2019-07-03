package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.testapp.activity.maplayout.TextureRecyclerViewActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates if a GLSurfaceView surface can be recreated without crashing.
 */
@RunWith(AndroidJUnit4::class)
class TextureViewReuseTest : BaseIntegrationTest() {

  @get:Rule
  var activityRule: ActivityTestRule<TextureRecyclerViewActivity> = ActivityTestRule(TextureRecyclerViewActivity::class.java)

  @Test
  @LargeTest
  fun scrollRecyclerView() {
    device.waitForIdle()
    device.scrollRecyclerViewTo("Twenty-one")
    device.waitForIdle()
    device.scrollRecyclerViewTo("One")
    device.waitForIdle()
  }
}