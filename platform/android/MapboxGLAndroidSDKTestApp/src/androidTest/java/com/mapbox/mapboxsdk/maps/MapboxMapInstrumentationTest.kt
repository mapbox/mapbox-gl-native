package com.mapbox.mapboxsdk.maps

import android.graphics.PointF
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.testapp.activity.BaseTest
import com.mapbox.mapboxsdk.testapp.activity.espresso.DeviceIndependentTestActivity
import org.junit.Assert.assertArrayEquals
import org.junit.Assert.assertEquals
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class MapboxMapInstrumentationTest : BaseTest() {

  override fun getActivityClass(): Class<*> {
    return DeviceIndependentTestActivity::class.java
  }

  @Test
  fun setPadding_cameraInvalidated() {
    rule.runOnUiThread {
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 0.0), 10.0))
      val initialCameraPosition = mapboxMap.cameraPosition
      val initialPoint = mapboxMap.projection.toScreenLocation(initialCameraPosition.target)

      val bottomPadding = mapView.height / 4
      val leftPadding = mapView.width / 4
      mapboxMap.setPadding(leftPadding, 0, 0, bottomPadding)

      val resultingCameraPosition = mapboxMap.cameraPosition
      assertArrayEquals(intArrayOf(leftPadding, 0, 0, bottomPadding), mapboxMap.padding)
      assertEquals(initialCameraPosition, resultingCameraPosition)
      assertEquals(
        PointF(initialPoint.x + leftPadding / 2, initialPoint.y - bottomPadding / 2),
        mapboxMap.projection.toScreenLocation(resultingCameraPosition.target)
      )
    }
  }
}