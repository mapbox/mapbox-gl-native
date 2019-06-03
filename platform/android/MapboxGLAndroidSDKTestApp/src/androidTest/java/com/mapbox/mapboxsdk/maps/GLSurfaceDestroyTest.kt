package com.mapbox.mapboxsdk.maps

import android.support.test.filters.LargeTest
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.integration.BaseIntegrationTest
import com.mapbox.mapboxsdk.maps.renderer.glsurfaceview.GLSurfaceViewMapRenderer
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.activity.maplayout.SimpleMapActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class GLSurfaceDestroyTest : BaseIntegrationTest() {

  @get:Rule
  var activityRule: ActivityTestRule<SimpleMapActivity> = ActivityTestRule(SimpleMapActivity::class.java)

  private lateinit var mapView:MapView

  @Test
  @LargeTest
  fun reopenSimpleMapActivity() {
    device.waitForIdle()
    mapView = activityRule.activity.findViewById(R.id.mapView)
    device.pressHome()
    device.waitForIdle()
    (mapView.mapRenderer as GLSurfaceViewMapRenderer).onDrawFrame(null)
    device.waitForIdle()
  }
}
