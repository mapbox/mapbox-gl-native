package com.mapbox.mapboxsdk.maps

import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.geometry.LatLngBounds
import com.mapbox.mapboxsdk.style.layers.TransitionOptions
import io.mockk.every
import io.mockk.mockk
import io.mockk.spyk
import io.mockk.verify
import junit.framework.Assert.assertEquals
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class MapboxMapTest {

    private lateinit var mapboxMap: MapboxMap

    private lateinit var nativeMapView: NativeMapView

    @Before
    fun setup() {
        val cameraChangeDispatcher = spyk<CameraChangeDispatcher>()
        val mapView = mockk<MapView>()
        nativeMapView = mockk()
        mapboxMap = MapboxMap(nativeMapView, Transform(mapView, nativeMapView, cameraChangeDispatcher), null, null, null, cameraChangeDispatcher)
        every { nativeMapView.styleUrl = any() } answers {}
        every { nativeMapView.transitionOptions = any() } answers {}
        every { nativeMapView.isDestroyed } returns false
        every { nativeMapView.cameraPosition } returns CameraPosition.DEFAULT
        every { nativeMapView.cancelTransitions() } answers {}
        every { nativeMapView.jumpTo(any(), any(), any(), any()) } answers {}
        every { nativeMapView.minZoom = any() } answers {}
        every { nativeMapView.maxZoom = any() } answers {}
        every { nativeMapView.setOnFpsChangedListener(any()) } answers {}
        every { nativeMapView.prefetchTiles = any() } answers {}
        every { nativeMapView.setLatLngBounds(any()) } answers {}
        mapboxMap.injectLocationComponent(spyk())
        mapboxMap.setStyle(Style.MAPBOX_STREETS)
        mapboxMap.onFinishLoadingStyle()
    }

    @Test
    fun testTransitionOptions() {
        val expected = TransitionOptions(100, 200)
        mapboxMap.style?.transition = expected
        verify { nativeMapView.transitionOptions = expected }
    }

    @Test
    fun testMoveCamera() {
        val callback = mockk<MapboxMap.CancelableCallback>()
        every { callback.onFinish() } answers {}
        val target = LatLng(1.0, 2.0)
        val expected = CameraPosition.Builder().target(target).build()
        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(expected), callback)
        verify { nativeMapView.jumpTo(target, -1.0, -1.0, -1.0) }
        verify { callback.onFinish() }
    }

    @Test
    fun testMinZoom() {
        mapboxMap.setMinZoomPreference(10.0)
        verify { nativeMapView.minZoom = 10.0 }
    }

    @Test
    fun testMaxZoom() {
        mapboxMap.setMaxZoomPreference(10.0)
        verify { nativeMapView.maxZoom = 10.0 }
    }

    @Test
    fun testFpsListener() {
        val fpsChangedListener = mockk<MapboxMap.OnFpsChangedListener>()
        mapboxMap.onFpsChangedListener = fpsChangedListener
        assertEquals("Listener should match", fpsChangedListener, mapboxMap.onFpsChangedListener)
    }

    @Test
    fun testTilePrefetch() {
        mapboxMap.prefetchesTiles = true
        verify { nativeMapView.prefetchTiles = true }
    }

    @Test
    fun testCameraForLatLngBounds() {
        val bounds = LatLngBounds.Builder().include(LatLng()).include(LatLng(1.0, 1.0)).build()
        mapboxMap.setLatLngBoundsForCameraTarget(bounds)
        verify { nativeMapView.setLatLngBounds(bounds) }
    }

    @Test(expected = IllegalArgumentException::class)
    fun testAnimateCameraChecksDurationPositive() {
        mapboxMap.animateCamera(CameraUpdateFactory.newLatLng(LatLng(30.0, 30.0)), 0, null)
    }

    @Test(expected = IllegalArgumentException::class)
    fun testEaseCameraChecksDurationPositive() {
        mapboxMap.easeCamera(CameraUpdateFactory.newLatLng(LatLng(30.0, 30.0)), 0, null)
    }
}