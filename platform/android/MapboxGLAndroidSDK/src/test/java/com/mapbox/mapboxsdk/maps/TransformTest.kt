package com.mapbox.mapboxsdk.maps

import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.geometry.LatLng
import io.mockk.*
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class TransformTest {

  private lateinit var mapView: MapView
  private lateinit var nativeMapView: NativeMap
  private lateinit var transform: Transform

  @Before
  fun setup() {
    val cameraChangeDispatcher = spyk<CameraChangeDispatcher>()
    mapView = mockk()
    nativeMapView = mockk()
    transform = Transform(mapView, nativeMapView, cameraChangeDispatcher)
    every { nativeMapView.isDestroyed } returns false
    every { nativeMapView.cameraPosition } returns CameraPosition.DEFAULT
    every { nativeMapView.cancelTransitions() } answers {}
    every { nativeMapView.jumpTo(any(), any(), any(), any(), any()) } answers {}
    every { nativeMapView.flyTo(any(), any(), any(), any(), any(), any()) } answers {}
    every { nativeMapView.minZoom = any() } answers {}
    every { nativeMapView.maxZoom = any() } answers {}
  }

  @Test
  fun testMoveCamera() {
    val mapboxMap = mockk<MapboxMap>()
    every { mapboxMap.cameraPosition } answers { CameraPosition.DEFAULT }

    val callback = mockk<MapboxMap.CancelableCallback>()
    every { callback.onFinish() } answers {}

    val target = LatLng(1.0, 2.0)
    val expected = CameraPosition.Builder().target(target).build()
    val update = CameraUpdateFactory.newCameraPosition(expected)
    transform.moveCamera(mapboxMap, update, callback)

    verify { nativeMapView.jumpTo(target, -1.0, -1.0, -1.0, null) }
    verify { callback.onFinish() }
  }

  @Test
  fun testMinZoom() {
    transform.minZoom = 10.0
    verify { nativeMapView.minZoom = 10.0 }
  }

  @Test
  fun testMaxZoom() {
    transform.maxZoom = 10.0
    verify { nativeMapView.maxZoom = 10.0 }
  }

  @Test
  fun testCancelNotInvokedFromOnFinish() {
    val slot = slot<MapView.OnCameraDidChangeListener>()
    every { mapView.addOnCameraDidChangeListener(capture(slot)) } answers { slot.captured.onCameraDidChange(true) }
    every { mapView.removeOnCameraDidChangeListener(any()) } answers {}
    // regression test for https://github.com/mapbox/mapbox-gl-native/issues/13735
    val mapboxMap = mockk<MapboxMap>()
    every { mapboxMap.cameraPosition } answers { CameraPosition.DEFAULT }

    val target = LatLng(1.0, 2.0)
    val expected = CameraPosition.Builder().target(target).build()

    val callback = object : MapboxMap.CancelableCallback {
      override fun onCancel() {
        throw IllegalStateException("onCancel shouldn't be called from onFinish")
      }

      override fun onFinish() {
        transform.animateCamera(mapboxMap, CameraUpdateFactory.newCameraPosition(expected), 500, null)
      }
    }
    transform.animateCamera(mapboxMap, CameraUpdateFactory.newCameraPosition(expected), 500, callback)
  }
}