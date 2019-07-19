package com.mapbox.mapboxsdk.maps

import android.graphics.PointF
import android.support.test.espresso.UiController
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke
import com.mapbox.mapboxsdk.testapp.activity.BaseTest
import com.mapbox.mapboxsdk.testapp.activity.espresso.PixelTestActivity
import org.junit.Assert.*
import org.junit.Test

class VisibleRegionTest : BaseTest() {

  override fun getActivityClass(): Class<*> {
    return PixelTestActivity::class.java
  }

  override
  fun beforeTest() {
    super.beforeTest()
    mapView = (rule.activity as PixelTestActivity).mapView
  }

  @Test
  fun visibleRegionTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 0.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f)
      )
      val visibleRegion = mapboxMap.projection.visibleRegion
      assertTrue(latLngs.all { visibleRegion.latLngBounds.contains(it) })
    }
  }

  @Test
  fun paddedVisibleRegionTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 0.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f)
      )

      mapboxMap.setPadding(
        mapView.width / 4,
        mapView.height / 4,
        mapView.width / 4,
        mapView.height / 4)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(1, filtered.size)
      assertTrue(filtered.contains(mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f)))
    }
  }

  @Test
  fun paddedLeftVisibleRegionTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 0.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 4f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width * 3f / 4f, 0f)
      )

      mapboxMap.setPadding(mapView.width / 4, 0, 0, 0)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter {
        visibleRegion.latLngBounds.contains(it)
      }
      assertEquals(5, filtered.size)
      assertFalse(filtered.contains(mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f)))
    }
  }

  @Test
  fun paddedTopVisibleRegionTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 0.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 4f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height * 3f / 4f)
      )

      mapboxMap.setPadding(0, mapView.height / 4, 0, 0)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(5, filtered.size)
      assertFalse(filtered.contains(mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f)))
    }
  }

  @Test
  fun paddedRightVisibleRegionTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 0.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 4f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width * 3f / 4f, 0f)
      )

      mapboxMap.setPadding(0, 0, mapView.width / 4, 0)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(5, filtered.size)
      assertFalse(filtered.contains(mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f)))
    }
  }

  @Test
  fun paddedBottomVisibleRegionTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 0.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 4f),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height * 3f / 4f)
      )

      mapboxMap.setPadding(0, 0, 0, mapView.height / 4)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(5, filtered.size)
      assertFalse(filtered.contains(mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat())))
    }
  }

  @Test
  fun visibleRegionOverDatelineTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 180.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat())
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f)
      )
      val visibleRegion = mapboxMap.projection.visibleRegion
      assertTrue(latLngs.all { visibleRegion.latLngBounds.contains(it) })
    }
  }

  @Test
  fun paddedVisibleRegionOverDatelineTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 180.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f)
      )

      mapboxMap.setPadding(
        mapView.width / 4,
        mapView.height / 4,
        mapView.width / 4,
        mapView.height / 4)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(1, filtered.size)
      assertTrue(filtered.contains(mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f)))
    }
  }

  @Test
  fun paddedLeftVisibleRegionOverDatelineTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 180.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat())
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 4f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width * 3f / 4f, mapView.height / 2f)
          .also { it.longitude += 360 }
      )

      mapboxMap.setPadding(mapView.width / 4, 0, 0, 0)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(5, filtered.size)
      assertFalse(filtered.contains(mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f)))
    }
  }

  @Test
  fun paddedTopVisibleRegionOverDatelineTest() {
    validateTestSetup()
    invoke(mapboxMap) { ui: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 180.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat())
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 4f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height * 3f / 4f)
          .also { it.longitude += 360 }
      )

      mapboxMap.setPadding(0, mapView.height / 4, 0, 0)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(5, filtered.size)
      assertFalse(filtered.contains(mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f)))
    }
  }

  @Test
  fun paddedRightVisibleRegionOverDatelineTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 180.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat())
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 4f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width * 3f / 4f, mapView.height / 2f)
          .also { it.longitude += 360 }
      )

      mapboxMap.setPadding(0, 0, mapView.width / 4, 0)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(5, filtered.size)
      assertFalse(filtered.contains(mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f)))
    }
  }

  @Test
  fun paddedBottomVisibleRegionOverDatelineTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 180.0), 8.0))
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(0f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, 0f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), 0f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height / 2f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height.toFloat())
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height.toFloat()),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(0f, mapView.height / 4f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width.toFloat(), mapView.height * 3f / 4f)
          .also { it.longitude += 360 }
      )

      mapboxMap.setPadding(0, 0, 0, mapView.height / 4)

      val visibleRegion = mapboxMap.projection.getVisibleRegion(false)
      val filtered = latLngs.filter { visibleRegion.latLngBounds.contains(it) }
      assertEquals(5, filtered.size)
      assertFalse(filtered.contains(mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height.toFloat())))
    }
  }

  @Test
  fun visibleRotatedRegionTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 0.0), 8.0))
      val d = Math.min(mapboxMap.width, mapboxMap.height) / 4
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f - d / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f + d / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f - d / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f + d / 2f)
      )

      for (bearing in 45 until 360 step 45) {
        mapboxMap.moveCamera(CameraUpdateFactory.bearingTo(bearing.toDouble()))
        val visibleRegion = mapboxMap.projection.visibleRegion
        assertTrue(latLngs.all { visibleRegion.latLngBounds.contains(it) })
      }
    }
  }

  @Test
  fun visibleRotatedRegionOverDatelineTest() {
    validateTestSetup()
    invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(0.0, 180.0), 8.0))
      val d = Math.min(mapboxMap.width, mapboxMap.height) / 4
      val latLngs = listOf(
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f - d / 2f, mapView.height / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f + d / 2f, mapView.height / 2f)
          .also { it.longitude += 360 },
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f - d / 2f),
        mapboxMap.getLatLngFromScreenCoords(mapView.width / 2f, mapView.height / 2f + d / 2f)
      )

      for (bearing in 45 until 360 step 45) {
        mapboxMap.moveCamera(CameraUpdateFactory.bearingTo(bearing.toDouble()))
        val visibleRegion = mapboxMap.projection.visibleRegion
        assertTrue(latLngs.all { visibleRegion.latLngBounds.contains(it) })
      }
    }
  }

  private fun MapboxMap.getLatLngFromScreenCoords(x: Float, y: Float): LatLng {
    return this.projection.fromScreenLocation(PointF(x, y))
  }
}