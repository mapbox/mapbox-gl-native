package com.mapbox.mapboxsdk.camera

import android.graphics.PointF
import androidx.test.annotation.UiThreadTest
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.geometry.LatLngBounds
import com.mapbox.mapboxsdk.testapp.activity.BaseTest
import com.mapbox.mapboxsdk.testapp.activity.espresso.DeviceIndependentTestActivity
import org.junit.Assert
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(androidx.test.ext.junit.runners.AndroidJUnit4::class)
class CameraUpdateFactoryTest : BaseTest() {

  override fun getActivityClass(): Class<*> {
    return DeviceIndependentTestActivity::class.java
  }

  @Test
  @UiThreadTest
  fun testLatLngBoundsUntiltedUnrotated() {
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

  @Test
  @UiThreadTest
  fun testLatLngBoundsTilted() {
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

  @Test
  @UiThreadTest
  fun testLatLngBoundsRotated() {
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

  @Test
  @UiThreadTest
  fun testLatLngBoundsTiltedRotated() {
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

  @Test
  @UiThreadTest
  fun testLatLngBoundsWithProvidedTiltAndRotation() {
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

  @Test
  @UiThreadTest
  fun withPadding_cameraInvalidated_paddingPersisting() {
    val initialCameraPosition = mapboxMap.cameraPosition
    val initialPoint = mapboxMap.projection.toScreenLocation(initialCameraPosition.target)

    val bottomPadding = mapView.height / 4
    val leftPadding = mapView.width / 4
    val padding = doubleArrayOf(leftPadding.toDouble(), 0.0, 0.0, bottomPadding.toDouble())
    mapboxMap.moveCamera(CameraUpdateFactory.paddingTo(leftPadding.toDouble(), 0.0, 0.0, bottomPadding.toDouble()))

    Assert.assertArrayEquals(intArrayOf(leftPadding, 0, 0, bottomPadding), mapboxMap.padding)

    val resultingCameraPosition = mapboxMap.cameraPosition
    assertEquals(initialCameraPosition.target, resultingCameraPosition.target)
    assertEquals(
      PointF(initialPoint.x + leftPadding / 2, initialPoint.y - bottomPadding / 2),
      mapboxMap.projection.toScreenLocation(resultingCameraPosition.target)
    )
    Assert.assertArrayEquals(padding, resultingCameraPosition.padding, 0.0001)
  }

  @Test
  @UiThreadTest
  fun withLatLngPadding_cameraInvalidated_paddingPersisting() {
    val expectedTarget = LatLng(2.0, 2.0)

    val topPadding = mapView.height / 4
    val rightPadding = mapView.width / 4
    val padding = doubleArrayOf(0.0, topPadding.toDouble(), rightPadding.toDouble(), 0.0)
    mapboxMap.moveCamera(CameraUpdateFactory.newLatLngPadding(expectedTarget, 0.0, topPadding.toDouble(), rightPadding.toDouble(), 0.0))

    Assert.assertArrayEquals(intArrayOf(0, topPadding, rightPadding, 0), mapboxMap.padding)

    val resultingCameraPosition = mapboxMap.cameraPosition
    assertEquals(expectedTarget.latitude, resultingCameraPosition.target.latitude, 0.1)
    assertEquals(expectedTarget.longitude, resultingCameraPosition.target.longitude, 0.1)

    val centerLatLng = mapboxMap.projection.fromScreenLocation(PointF((mapView.width / 2).toFloat(), (mapView.height / 2).toFloat()))
    assertTrue(centerLatLng.latitude > resultingCameraPosition.target.latitude)
    assertTrue(centerLatLng.longitude > resultingCameraPosition.target.longitude)
    Assert.assertArrayEquals(padding, resultingCameraPosition.padding, 0.0001)
  }
}