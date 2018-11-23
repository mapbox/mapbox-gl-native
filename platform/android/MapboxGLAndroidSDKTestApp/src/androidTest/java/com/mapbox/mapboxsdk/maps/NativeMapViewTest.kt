package com.mapbox.mapboxsdk.maps

import android.content.Context
import android.graphics.PointF
import android.support.test.InstrumentationRegistry
import android.support.test.annotation.UiThreadTest
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.renderer.MapRenderer
import junit.framework.Assert.assertEquals
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class NativeMapViewTest {

    private lateinit var nativeMapView: NativeMapView

    companion object {
        const val DELTA = 0.000001
        const val DELTA_BIG = 1.0
        const val BEARING_TEST = 60.0
        const val PITCH_TEST = 40.0
        const val ZOOM_TEST = 16.0
        const val WIDTH = 500
        const val HEIGHT = WIDTH
        val LATLNG_TEST = LatLng(12.0, 34.0)
    }

    @Before
    @UiThreadTest
    fun before() {
        val context = InstrumentationRegistry.getContext()
        nativeMapView = NativeMapView(context, false, null, null, DummyRenderer(context))
        nativeMapView.resizeView(WIDTH, HEIGHT)
    }

    @Test
    @UiThreadTest
    fun testBearing() {
        val expected = BEARING_TEST
        nativeMapView.bearing = expected
        val actual = nativeMapView.bearing
        assertEquals("Bearing should match", expected, actual, DELTA)
    }

    @Test
    @UiThreadTest
    fun testLatLng() {
        val expected = LATLNG_TEST
        nativeMapView.latLng = expected
        val actual = nativeMapView.latLng
        assertEquals("Latitude should match", expected.latitude, actual.latitude, DELTA)
        assertEquals("Longitude should match", expected.longitude, actual.longitude, DELTA)
    }

    @Test
    @UiThreadTest
    fun testLatLngDefault() {
        val expected = LatLng()
        val actual = nativeMapView.latLng
        assertEquals("Latitude should match", expected.latitude, actual.latitude, DELTA)
        assertEquals("Longitude should match", expected.longitude, actual.longitude, DELTA)
    }


    @Test
    @UiThreadTest
    fun testBearingDefault() {
        val expected = 0.0
        val actual = nativeMapView.bearing
        assertEquals("Bearing should match", expected, actual, DELTA)
    }

    @Test
    @UiThreadTest
    fun testPitch() {
        val expected = PITCH_TEST
        nativeMapView.pitch = expected
        val actual = nativeMapView.pitch
        assertEquals("Pitch should match", expected, actual, DELTA)
    }

    @Test
    @UiThreadTest
    fun testPitchDefault() {
        val expected = 0.0
        val actual = nativeMapView.pitch
        assertEquals("Pitch should match", expected, actual, DELTA)
    }

    @Test
    @UiThreadTest
    fun testZoom() {
        val expected = ZOOM_TEST
        nativeMapView.setZoom(expected, PointF(0.0f, 0.0f), 0)
        val actual = nativeMapView.zoom
        assertEquals("Zoom should match", expected, actual, DELTA)
    }

    @Test
    @UiThreadTest
    fun testZoomDefault() {
        val expected = 0.0
        val actual = nativeMapView.zoom
        assertEquals("Zoom should match", expected, actual, DELTA)
    }

    @Test
    @UiThreadTest
    fun testJumpTo() {
        val expected = CameraPosition.Builder()
                .bearing(BEARING_TEST)
                .target(LATLNG_TEST)
                .tilt(PITCH_TEST)
                .zoom(ZOOM_TEST)
                .build()
        nativeMapView.jumpTo(BEARING_TEST, LATLNG_TEST, PITCH_TEST, ZOOM_TEST)
        val actual = nativeMapView.cameraPosition
        assertEquals("Latitude should match", expected.target.latitude, actual.target.latitude, DELTA)
        assertEquals("Longitude should match", expected.target.longitude, actual.target.longitude, DELTA)
        assertEquals("Bearing should match", expected.bearing, actual.bearing, DELTA)
        assertEquals("Pitch should match", expected.tilt, actual.tilt, DELTA)
        assertEquals("Zoom should match", expected.zoom, actual.zoom, DELTA)
    }

    @Test
    @UiThreadTest
    fun testLatLngForPixel() {
        val expected = LATLNG_TEST
        nativeMapView.latLng = LATLNG_TEST
        val actual = nativeMapView.latLngForPixel(
                PointF((WIDTH / 2).toFloat(), (HEIGHT / 2).toFloat())
        )
        assertEquals("Latitude should match", expected.latitude, actual.latitude, DELTA_BIG)
        assertEquals("Longitude should match", expected.longitude, actual.longitude, DELTA_BIG)
    }

    @Test
    @UiThreadTest
    fun testPixelForLatLng() {
        val expected = PointF((WIDTH / 2).toFloat(), (HEIGHT / 2).toFloat())
        nativeMapView.latLng = LATLNG_TEST
        val actual = nativeMapView.pixelForLatLng(LATLNG_TEST)
        assertEquals("X should match", expected.x.toDouble(), actual.x.toDouble(), DELTA_BIG)
        assertEquals("Y should match", expected.y.toDouble(), actual.y.toDouble(), DELTA_BIG)
    }

    @Test
    @UiThreadTest
    fun testPrefetchTilesTrue(){
        val expected = true
        nativeMapView.prefetchesTiles = true
        val actual = nativeMapView.prefetchesTiles
        assertEquals("Flag should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testPrefetchTilesFalse(){
        val expected = false
        nativeMapView.prefetchesTiles = false
        val actual = nativeMapView.prefetchesTiles
        assertEquals("Flag should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testPrefetchTilesDefault(){
        val expected = true
        val actual = nativeMapView.prefetchesTiles
        assertEquals("Flag should match", expected, actual)
    }

    class DummyRenderer(context: Context) : MapRenderer(context, null) {

        override fun requestRender() {
            //no-op
        }

        override fun queueEvent(runnable: Runnable?) {
            //no-op
        }
    }
}