package com.mapbox.mapboxsdk.maps

import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.action.EspressoKey
import android.support.test.espresso.action.ViewActions.click
import android.support.test.espresso.action.ViewActions.pressKey
import android.support.test.espresso.matcher.ViewMatchers.withId
import android.view.KeyEvent
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.activity.BaseTest
import com.mapbox.mapboxsdk.testapp.activity.maplayout.SimpleMapActivity
import org.junit.Assert.assertNotEquals
import org.junit.Before
import org.junit.Test

class MapKeyListenerTest : BaseTest() {
  override fun getActivityClass() = SimpleMapActivity::class.java

  private var maxWidth: Int = 0
  private var maxHeight: Int = 0

  @Before
  fun setup() {
    maxWidth = mapView.width
    maxHeight = mapView.height
  }

  @Test
  fun pitchAdjustWithShiftAndUp() {
    validateTestSetup()
    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      // zoom in so we can move vertically
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(10.0))
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isTiltGesturesEnabled = true
    }
    // Tap the up arrow while holding shift on the keyboard directional pad
    onView(withId(R.id.mapView)).perform(click()).perform(pressKey(
      EspressoKey.Builder()
        .withShiftPressed(true)
        .withKeyCode(KeyEvent.KEYCODE_DPAD_UP)
        .build()))

    rule.runOnUiThread {
      assertNotEquals(initialCameraPosition!!.tilt, mapboxMap.cameraPosition.tilt)
    }
  }

  @Test
  fun bearingAdjustWithShiftAndLeft() {
    validateTestSetup()
    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isRotateGesturesEnabled = true
    }
    // Tap the left arrow while holding shift on the keyboard directional pad
    onView(withId(R.id.mapView)).perform(click()).perform(pressKey(
      EspressoKey.Builder()
        .withShiftPressed(true)
        .withKeyCode(KeyEvent.KEYCODE_DPAD_LEFT)
        .build()))

    rule.runOnUiThread {
      assertNotEquals(initialCameraPosition!!.bearing, mapboxMap.cameraPosition.bearing)
    }
  }

  @Test
  fun targetAdjustWithUp() {
    validateTestSetup()
    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      // zoom in so we can move vertically
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.0))
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isQuickZoomGesturesEnabled = true
    }

    // Tap the up arrow on the keyboard directional pad
    onView(withId(R.id.mapView)).perform(click()).perform(pressKey(
      EspressoKey.Builder()
        .withKeyCode(KeyEvent.KEYCODE_DPAD_UP)
        .build()))

    rule.runOnUiThread {
      assertNotEquals(initialCameraPosition!!.target.latitude, mapboxMap.cameraPosition.target.latitude)
      assertNotEquals(initialCameraPosition!!.target.longitude, mapboxMap.cameraPosition.target.longitude)
    }
  }

  @Test
  fun targetAdjustWithDown() {
    validateTestSetup()
    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      // zoom in so we can move vertically
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.0))
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isQuickZoomGesturesEnabled = true
    }

    // Tap the down arrow on the keyboard directional pad
    onView(withId(R.id.mapView)).perform(click()).perform(pressKey(
      EspressoKey.Builder()
        .withKeyCode(KeyEvent.KEYCODE_DPAD_DOWN)
        .build()))

    rule.runOnUiThread {
      assertNotEquals(initialCameraPosition!!.target.latitude, mapboxMap.cameraPosition.target.latitude)
      assertNotEquals(initialCameraPosition!!.target.longitude, mapboxMap.cameraPosition.target.longitude)
    }
  }

  @Test
  fun increaseZoom() {
    validateTestSetup()
    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      // zoom in so we can move vertically
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.0))
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isQuickZoomGesturesEnabled = true
    }

    // Tap the down arrow on the keyboard directional pad
    onView(withId(R.id.mapView)).perform(click()).perform(pressKey(KeyEvent.KEYCODE_EQUALS))
    onView(withId(R.id.mapView)).perform(pressKey(KeyEvent.KEYCODE_EQUALS))

    rule.runOnUiThread {
      assertNotEquals(initialCameraPosition!!.target.latitude, mapboxMap.cameraPosition.target.latitude)
      assertNotEquals(initialCameraPosition!!.target.longitude, mapboxMap.cameraPosition.target.longitude)
    }
  }

  @Test
  fun decreaseZoom() {
    validateTestSetup()
    var initialCameraPosition: CameraPosition? = null
    rule.runOnUiThread {
      // zoom in so we can move vertically
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.0))
      initialCameraPosition = mapboxMap.cameraPosition
      mapboxMap.uiSettings.isQuickZoomGesturesEnabled = true
    }

    // Tap the down arrow on the keyboard directional pad
    onView(withId(R.id.mapView)).perform(click()).perform(pressKey(KeyEvent.KEYCODE_MINUS))
    onView(withId(R.id.mapView)).perform(pressKey(KeyEvent.KEYCODE_MINUS))

    rule.runOnUiThread {
      assertNotEquals(initialCameraPosition!!.target.latitude, mapboxMap.cameraPosition.target.latitude)
      assertNotEquals(initialCameraPosition!!.target.longitude, mapboxMap.cameraPosition.target.longitude)
    }
  }
}