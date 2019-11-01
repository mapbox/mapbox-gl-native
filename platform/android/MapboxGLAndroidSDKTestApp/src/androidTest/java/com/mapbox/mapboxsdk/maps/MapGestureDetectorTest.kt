package com.mapbox.mapboxsdk.maps

import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.matcher.ViewMatchers.withId
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.constants.MapboxConstants
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.GesturesUiTestUtils.move
import com.mapbox.mapboxsdk.maps.GesturesUiTestUtils.quickScale
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.activity.BaseTest
import com.mapbox.mapboxsdk.testapp.activity.maplayout.SimpleMapActivity
import org.junit.Assert
import org.junit.Before
import org.junit.Test

class MapGestureDetectorTest : BaseTest() {
  override fun getActivityClass() = SimpleMapActivity::class.java

  private var maxWidth: Int = 0
  private var maxHeight: Int = 0

  @Before
  fun setup() {
    maxWidth = mapView.width
    maxHeight = mapView.height
  }

  @Test
  fun sanity_quickZoom() {
    validateTestSetup()
    var initialZoom: Double? = null
    rule.runOnUiThread {
      initialZoom = mapboxMap.cameraPosition.zoom
    }
    onView(withId(R.id.mapView)).perform(quickScale(maxHeight / 2f, withVelocity = false))
    rule.runOnUiThread {
      Assert.assertTrue(mapboxMap.cameraPosition.zoom > initialZoom!!)
    }
  }

  @Test
  fun quickZoomDisabled_phantomQuickZoom_moveStillEnabled_15091() {
    // regression test for https://github.com/mapbox/mapbox-gl-native/issues/15091
    validateTestSetup()
    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      // zoom in so we can move vertically
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.0))
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isQuickZoomGesturesEnabled = false
    }

    onView(withId(R.id.mapView)).perform(quickScale(maxHeight / 2f))
    rule.runOnUiThread {
      // camera did not move
      Assert.assertEquals(initialCameraPosition!!, mapboxMap.cameraPosition)
    }

    // move to expected target
    onView(withId(R.id.mapView)).perform(move(-maxWidth / 2f, -maxHeight / 2f, withVelocity = false))
    rule.runOnUiThread {
      Assert.assertNotEquals(initialCameraPosition!!.target.latitude, mapboxMap.cameraPosition.target.latitude, 1.0)
      Assert.assertNotEquals(initialCameraPosition!!.target.longitude, mapboxMap.cameraPosition.target.longitude, 1.0)
    }
  }

  @Test
  fun quickZoom_doNotMove_14227() {
    // test for https://github.com/mapbox/mapbox-gl-native/issues/14227
    validateTestSetup()
    var initialTarget: LatLng? = null
    rule.runOnUiThread {
      initialTarget = mapboxMap.cameraPosition.target
    }

    onView(withId(R.id.mapView)).perform(quickScale(maxHeight / 2f))
    rule.runOnUiThread {
      // camera did not move
      Assert.assertEquals(initialTarget!!.latitude, mapboxMap.cameraPosition.target.latitude, 1.0)
      Assert.assertEquals(initialTarget!!.longitude, mapboxMap.cameraPosition.target.longitude, 1.0)
    }
  }

  @Test
  fun quickZoom_interrupted_moveStillEnabled_14598() {
    // test for https://github.com/mapbox/mapbox-gl-native/issues/14598
    validateTestSetup()
    onView(withId(R.id.mapView)).perform(quickScale(maxHeight / 2f, interrupt = true))

    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      // zoom in so we can move vertically
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.0))
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isQuickZoomGesturesEnabled = false
    }

    // move to expected target
    onView(withId(R.id.mapView)).perform(move(-maxWidth / 2f, -maxHeight / 2f, withVelocity = false))
    rule.runOnUiThread {
      Assert.assertNotEquals(initialCameraPosition!!.target.latitude, mapboxMap.cameraPosition.target.latitude, 1.0)
      Assert.assertNotEquals(initialCameraPosition!!.target.longitude, mapboxMap.cameraPosition.target.longitude, 1.0)
    }
  }

  @Test
  fun quickZoom_ignoreDoubleTap() {
    // test for https://github.com/mapbox/mapbox-gl-native/issues/14013
    validateTestSetup()
    var initialZoom: Double? = null
    rule.runOnUiThread {
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(2.0))
      initialZoom = mapboxMap.cameraPosition.zoom
    }
    onView(withId(R.id.mapView)).perform(quickScale(-(mapboxMap.gesturesManager.standardScaleGestureDetector.spanSinceStartThreshold * 2), withVelocity = false, duration = 1000L))
    R.id.mapView.loopFor(MapboxConstants.ANIMATION_DURATION.toLong())
    rule.runOnUiThread {
      Assert.assertTrue(mapboxMap.cameraPosition.zoom < initialZoom!!)
    }
  }

  @Test
  fun doubleTap_minimalMovement() {
    validateTestSetup()
    var initialZoom: Double? = null
    rule.runOnUiThread {
      initialZoom = mapboxMap.cameraPosition.zoom
    }
    onView(withId(R.id.mapView)).perform(quickScale(mapboxMap.gesturesManager.standardScaleGestureDetector.spanSinceStartThreshold / 2, withVelocity = false, duration = 50L))
    R.id.mapView.loopFor(MapboxConstants.ANIMATION_DURATION.toLong())
    rule.runOnUiThread {
      Assert.assertEquals(initialZoom!! + 1, mapboxMap.cameraPosition.zoom, 0.1)
    }
  }

  @Test
  fun doubleTap_overMaxThreshold_ignore_14013() {
    // test for https://github.com/mapbox/mapbox-gl-native/issues/14013
    validateTestSetup()
    var initialZoom: Double? = null
    rule.runOnUiThread {
      initialZoom = mapboxMap.cameraPosition.zoom
      mapboxMap.uiSettings.isQuickZoomGesturesEnabled = false
    }
    onView(withId(R.id.mapView)).perform(quickScale(mapboxMap.gesturesManager.standardScaleGestureDetector.spanSinceStartThreshold * 2, withVelocity = false, duration = 50L))
    R.id.mapView.loopFor(MapboxConstants.ANIMATION_DURATION.toLong())
    rule.runOnUiThread {
      Assert.assertEquals(initialZoom!!, mapboxMap.cameraPosition.zoom, 0.01)
    }
  }

  @Test
  fun doubleTap_interrupted_moveStillEnabled() {
    validateTestSetup()

    rule.runOnUiThread {
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.0))
    }

    onView(withId(R.id.mapView)).perform(quickScale(mapboxMap.gesturesManager.standardScaleGestureDetector.spanSinceStartThreshold / 2, withVelocity = false, duration = 50L, interrupt = true))

    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      // zoom in so we can move vertically
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.0))
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isQuickZoomGesturesEnabled = false
    }

    // move to expected target
    onView(withId(R.id.mapView)).perform(move(-maxWidth / 2f, -maxHeight / 2f, withVelocity = false))
    rule.runOnUiThread {
      Assert.assertNotEquals(initialCameraPosition!!.target.latitude, mapboxMap.cameraPosition.target.latitude, 1.0)
      Assert.assertNotEquals(initialCameraPosition!!.target.longitude, mapboxMap.cameraPosition.target.longitude, 1.0)
    }
  }

  @Test
  fun quickZoom_roundTripping() {
    validateTestSetup()
    rule.runOnUiThread {
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(51.0, 16.0), 3.0))
    }
    onView(withId(R.id.mapView)).perform(quickScale(300f, withVelocity = false, duration = 750L))
    onView(withId(R.id.mapView)).perform(quickScale(-300f, withVelocity = false, duration = 750L))

    rule.runOnUiThread {
      Assert.assertEquals(3.0, mapboxMap.cameraPosition.zoom, 0.0001)
    }
  }
}