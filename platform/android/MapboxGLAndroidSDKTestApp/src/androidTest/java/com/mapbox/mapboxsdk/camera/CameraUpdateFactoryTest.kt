package com.mapbox.mapboxsdk.camera

import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.geometry.LatLngBounds
import com.mapbox.mapboxsdk.testapp.activity.BaseTest
import com.mapbox.mapboxsdk.testapp.activity.espresso.DeviceIndependentTestActivity
import org.junit.Assert.assertEquals
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class CameraUpdateFactoryTest : BaseTest() {

  override fun getActivityClass(): Class<*> {
    return DeviceIndependentTestActivity::class.java
  }

  @Test
  fun testLatLngBoundsUntiltedUnrotated() {
    rule.runOnUiThread {
      mapboxMap.cameraPosition = CameraPosition.Builder()
        .target(LatLng(60.0, 24.0))
        .bearing(0.0)
        .tilt(0.0)
        .build()

      val bounds: LatLngBounds = LatLngBounds.Builder()
        .include(LatLng(62.0, 26.0))
        .include(LatLng(58.0, 22.0))
        .build()

      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngBounds(bounds, 0))

      val cameraPosition = mapboxMap.cameraPosition
      assertEquals("latitude should match:", 60.0, cameraPosition.target.latitude, 0.1)
      assertEquals("longitude should match:", 24.0, cameraPosition.target.longitude, 0.1)
      assertEquals("bearing should match:", 0.0, cameraPosition.bearing, 0.1)
      assertEquals("zoom should match", 5.5, cameraPosition.zoom, 0.1)
      assertEquals("tilt should match:", 0.0, cameraPosition.tilt, 0.1)
    }
  }

  @Test
  fun testLatLngBoundsTilted() {
    rule.runOnUiThread {
      mapboxMap.cameraPosition = CameraPosition.Builder()
        .target(LatLng(60.0, 24.0))
        .bearing(0.0)
        .tilt(45.0)
        .build()

      val bounds: LatLngBounds = LatLngBounds.Builder()
        .include(LatLng(62.0, 26.0))
        .include(LatLng(58.0, 22.0))
        .build()

      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngBounds(bounds, 0))

      val cameraPosition = mapboxMap.cameraPosition
      assertEquals("latitude should match:", 60.0, cameraPosition.target.latitude, 0.1)
      assertEquals("longitude should match:", 24.0, cameraPosition.target.longitude, 0.1)
      assertEquals("bearing should match:", 0.0, cameraPosition.bearing, 0.1)
      assertEquals("zoom should match", 6.0, cameraPosition.zoom, 0.1)
      assertEquals("tilt should match:", 45.0, cameraPosition.tilt, 0.1)
    }
  }

  @Test
  fun testLatLngBoundsRotated() {
    rule.runOnUiThread {
      mapboxMap.cameraPosition = CameraPosition.Builder()
        .target(LatLng(60.0, 24.0))
        .bearing(30.0)
        .tilt(0.0)
        .build()

      val bounds: LatLngBounds = LatLngBounds.Builder()
        .include(LatLng(62.0, 26.0))
        .include(LatLng(58.0, 22.0))
        .build()

      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngBounds(bounds, 0))

      val cameraPosition = mapboxMap.cameraPosition
      assertEquals("latitude should match:", 60.0, cameraPosition.target.latitude, 0.1)
      assertEquals("longitude should match:", 24.0, cameraPosition.target.longitude, 0.1)
      assertEquals("bearing should match:", 30.0, cameraPosition.bearing, 0.1)
      assertEquals("zoom should match", 5.3, cameraPosition.zoom, 0.1)
      assertEquals("tilt should match:", 0.0, cameraPosition.tilt, 0.1)
    }
  }

  @Test
  fun testLatLngBoundsTiltedRotated() {
    rule.runOnUiThread {
      mapboxMap.cameraPosition = CameraPosition.Builder()
        .target(LatLng(60.0, 24.0))
        .bearing(30.0)
        .tilt(45.0)
        .build()

      val bounds: LatLngBounds = LatLngBounds.Builder()
        .include(LatLng(62.0, 26.0))
        .include(LatLng(58.0, 22.0))
        .build()

      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngBounds(bounds, 0))

      val cameraPosition = mapboxMap.cameraPosition
      assertEquals("latitude should match:", 60.0, cameraPosition.target.latitude, 0.1)
      assertEquals("longitude should match:", 24.0, cameraPosition.target.longitude, 0.1)
      assertEquals("bearing should match:", 30.0, cameraPosition.bearing, 0.1)
      assertEquals("zoom should match", 5.6, cameraPosition.zoom, 0.1)
      assertEquals("tilt should match:", 45.0, cameraPosition.tilt, 0.1)
    }
  }

  @Test
  fun testLatLngBoundsWithProvidedTiltAndRotation() {
    rule.runOnUiThread {
      mapboxMap.cameraPosition = CameraPosition.Builder()
        .target(LatLng(60.0, 24.0))
        .bearing(0.0)
        .tilt(0.0)
        .build()

      val bounds: LatLngBounds = LatLngBounds.Builder()
        .include(LatLng(62.0, 26.0))
        .include(LatLng(58.0, 22.0))
        .build()

      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngBounds(bounds, 30.0, 40.0, 0))

      val cameraPosition = mapboxMap.cameraPosition
      assertEquals("latitude should match:", 60.0, cameraPosition.target.latitude, 0.1)
      assertEquals("longitude should match:", 24.0, cameraPosition.target.longitude, 0.1)
      assertEquals("bearing should match:", 30.0, cameraPosition.bearing, 0.1)
      assertEquals("zoom should match", 5.6, cameraPosition.zoom, 0.1)
      assertEquals("tilt should match:", 40.0, cameraPosition.tilt, 0.1)
    }
  }
}