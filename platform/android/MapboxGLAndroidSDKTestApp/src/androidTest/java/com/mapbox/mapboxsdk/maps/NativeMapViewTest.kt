package com.mapbox.mapboxsdk.maps

import android.content.Context
import android.graphics.PointF
import android.support.test.InstrumentationRegistry
import android.support.test.annotation.UiThreadTest
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.AppCenter
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.geometry.LatLngBounds
import com.mapbox.mapboxsdk.geometry.ProjectedMeters
import com.mapbox.mapboxsdk.maps.renderer.MapRenderer
import com.mapbox.mapboxsdk.style.layers.TransitionOptions
import com.mapbox.mapboxsdk.testapp.utils.TestConstants
import junit.framework.Assert.*
import org.junit.After
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class NativeMapViewTest : AppCenter() {

    private lateinit var nativeMapView: NativeMap

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
        nativeMapView = NativeMapView(context, 2.0f, false, null, null, DummyRenderer(context))
        nativeMapView.resizeView(WIDTH, HEIGHT)
    }

    @After
    @UiThreadTest
    fun after() {
        nativeMapView.destroy()
    }

    @Test
    @UiThreadTest
    fun testSetStyleUrl() {
        val expected = Style.DARK
        nativeMapView.styleUri = expected
        val actual = nativeMapView.styleUri
        assertEquals("Style URL should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testSetStyleJson() {
        val expected = "{}"
        nativeMapView.styleJson = expected
        val actual = nativeMapView.styleJson
        assertEquals("Style JSON should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testBearing() {
        val expected = BEARING_TEST
        nativeMapView.setBearing(expected, 0)
        val actual = nativeMapView.bearing
        assertEquals("Bearing should match", expected, actual, DELTA)
    }

    @Test
    @UiThreadTest
    fun testLatLng() {
        val expected = LATLNG_TEST
        nativeMapView.setLatLng(expected, 0)
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
        nativeMapView.setPitch(expected, 0)
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
        nativeMapView.jumpTo(LATLNG_TEST, ZOOM_TEST, PITCH_TEST, BEARING_TEST)
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
        nativeMapView.setLatLng(LATLNG_TEST,0)
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
        nativeMapView.setLatLng(LATLNG_TEST, 0)
        val actual = nativeMapView.pixelForLatLng(LATLNG_TEST)
        assertEquals("X should match", expected.x.toDouble(), actual.x.toDouble(), DELTA_BIG)
        assertEquals("Y should match", expected.y.toDouble(), actual.y.toDouble(), DELTA_BIG)
    }

    @Test
    @UiThreadTest
    fun testPrefetchTilesTrue() {
        val expected = true
        nativeMapView.prefetchTiles = true
        val actual = nativeMapView.prefetchTiles
        assertEquals("Flag should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testPrefetchTilesFalse() {
        val expected = false
        nativeMapView.prefetchTiles = false
        val actual = nativeMapView.prefetchTiles
        assertEquals("Flag should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testPrefetchTilesDefault() {
        val expected = true
        val actual = nativeMapView.prefetchTiles
        assertEquals("Flag should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testSetContentPadding() {
        val expected = floatArrayOf(1.0f, 2.0f, 3.0f, 4.0f)
        nativeMapView.contentPadding = expected
        val actual = nativeMapView.contentPadding
        assertEquals("Left should match", expected[0], actual[0])
        assertEquals("Top should match", expected[1], actual[1])
        assertEquals("Right should match", expected[2], actual[2])
        assertEquals("Bottom should match", expected[3], actual[3])
    }

    @Test
    @UiThreadTest
    fun testSetMinZoom() {
        val expected = 12.0
        nativeMapView.minZoom = expected
        val actual = nativeMapView.minZoom
        assertEquals("Min zoom should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testSetMaxZoom() {
        val expected = 12.0
        nativeMapView.maxZoom = expected
        val actual = nativeMapView.maxZoom
        assertEquals("Max zoom should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testGetProjectedMetersAtLatitude() {
        val expected = 38986.83510557766
        val actual = nativeMapView.getMetersPerPixelAtLatitude(5.0)
        assertEquals("Get projected meters should match", expected, actual)
    }

    @Test
    @UiThreadTest
    fun testLatLngForProjectedMeters() {
        val expected = LatLng(0.01796630538796444, 0.02694945852363162)
        val actual = nativeMapView.latLngForProjectedMeters(ProjectedMeters(2000.0, 3000.0))
        assertEquals("Lat for projected meters", expected.latitude, actual.latitude, DELTA)
        assertEquals("Lng for projected meters", expected.longitude, actual.longitude, DELTA)
    }

    @Test
    @UiThreadTest
    fun testFlyTo() {
        val expected = CameraPosition.Builder()
                .zoom(12.0)
                .tilt(30.0)
                .target(LatLng(12.0, 14.0))
                .bearing(20.0)
                .build()
        nativeMapView.flyTo(expected.target, expected.zoom, expected.bearing, expected.tilt, 0)
        val actual = nativeMapView.cameraPosition
        assertEquals("Bearing should match", expected.bearing, actual.bearing, TestConstants.BEARING_DELTA)
        assertEquals("Latitude should match", expected.target.latitude, actual.target.latitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Longitude should match", expected.target.longitude, actual.target.longitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Tilt should match", expected.tilt, actual.tilt, TestConstants.TILT_DELTA)
        assertEquals("Zoom should match", expected.zoom, actual.zoom, TestConstants.ZOOM_DELTA)
    }

    @Test
    @UiThreadTest
    fun testEaseTo() {
        val expected = CameraPosition.Builder()
                .zoom(12.0)
                .tilt(30.0)
                .target(LatLng(12.0, 14.0))
                .bearing(20.0)
                .build()
        nativeMapView.easeTo(expected.target, expected.zoom, expected.bearing, expected.tilt, 0, false)
        val actual = nativeMapView.cameraPosition
        assertEquals("Bearing should match", expected.bearing, actual.bearing, TestConstants.BEARING_DELTA)
        assertEquals("Latitude should match", expected.target.latitude, actual.target.latitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Longitude should match", expected.target.longitude, actual.target.longitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Tilt should match", expected.tilt, actual.tilt, TestConstants.TILT_DELTA)
        assertEquals("Zoom should match", expected.zoom, actual.zoom, TestConstants.ZOOM_DELTA)
    }

    @Test
    @UiThreadTest
    fun testResetPosition() {
        val expected = CameraPosition.Builder()
                .zoom(0.0)
                .tilt(0.0)
                .target(LatLng(0.0, 0.0))
                .bearing(0.0)
                .build()
        nativeMapView.jumpTo(LatLng(1.0, 2.0), 12.0, 23.0, 1.0)
        nativeMapView.resetPosition()
        val actual = nativeMapView.cameraPosition
        assertEquals("Bearing should match", expected.bearing, actual.bearing, TestConstants.BEARING_DELTA)
        assertEquals("Latitude should match", expected.target.latitude, actual.target.latitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Longitude should match", expected.target.longitude, actual.target.longitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Tilt should match", expected.tilt, actual.tilt, TestConstants.TILT_DELTA)
        assertEquals("Zoom should match", expected.zoom, actual.zoom, TestConstants.ZOOM_DELTA)
    }

    @Test
    @UiThreadTest
    fun testGetCameraForLatLngBounds() {
        val expected = CameraPosition.Builder()
                .zoom(3.5258764777024005)
                .tilt(0.0)
                .target(LatLng(23.182767623652808, 13.999999999994088))
                .bearing(0.0)
                .build()
        val actual = nativeMapView.getCameraForLatLngBounds(
                LatLngBounds.from(30.0, 16.0, 16.0, 12.0),
                intArrayOf(0, 0, 0, 0),
                0.0,
                0.0
        )
        assertEquals("Bearing should match", expected.bearing, actual.bearing, TestConstants.BEARING_DELTA)
        assertEquals("Latitude should match", expected.target.latitude, actual.target.latitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Longitude should match", expected.target.longitude, actual.target.longitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Tilt should match", expected.tilt, actual.tilt, TestConstants.TILT_DELTA)
        assertEquals("Zoom should match", expected.zoom, actual.zoom, TestConstants.ZOOM_DELTA)
    }

    @Test
    @UiThreadTest
    fun testMoveBy() {
        val expected = CameraPosition.Builder()
                .zoom(0.0)
                .tilt(0.0)
                .target(LatLng(4.21494310024160, -4.218749958739409))
                .bearing(0.0)
                .build()
        nativeMapView.moveBy(12.0, 12.0, 0)
        val actual = nativeMapView.cameraPosition
        assertEquals("Bearing should match", expected.bearing, actual.bearing, TestConstants.BEARING_DELTA)
        assertEquals("Latitude should match", expected.target.latitude, actual.target.latitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Longitude should match", expected.target.longitude, actual.target.longitude, TestConstants.LAT_LNG_DELTA)
        assertEquals("Tilt should match", expected.tilt, actual.tilt, TestConstants.TILT_DELTA)
        assertEquals("Zoom should match", expected.zoom, actual.zoom, TestConstants.ZOOM_DELTA)
    }

    @Test
    @UiThreadTest
    fun testTransitionOptions() {
        val transitionOptions = TransitionOptions(500, 500)
        nativeMapView.transitionOptions = transitionOptions
        assertTrue(transitionOptions.isEnablePlacementTransitions)
        assertEquals(transitionOptions, nativeMapView.transitionOptions)
    }

    @Test
    @UiThreadTest
    fun testTransitionOptions_disablePlacementTransitions() {
        val transitionOptions = TransitionOptions(500, 500, false)
        nativeMapView.transitionOptions = transitionOptions
        assertFalse(transitionOptions.isEnablePlacementTransitions)
        assertEquals(transitionOptions, nativeMapView.transitionOptions)
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