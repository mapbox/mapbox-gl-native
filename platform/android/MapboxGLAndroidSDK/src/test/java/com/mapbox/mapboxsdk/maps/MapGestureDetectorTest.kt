package com.mapbox.mapboxsdk.maps

import android.graphics.PointF
import android.util.Pair
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.geometry.LatLngBounds
import com.mapbox.mapboxsdk.maps.MapGestureDetector.BOUND_REPEL_RATIO
import com.mapbox.mapboxsdk.utils.MathUtils
import io.mockk.every
import io.mockk.mockk
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class MapGestureDetectorTest {

  companion object {
    private val BOUNDS = LatLngBounds.Builder()
      .include(LatLng(10.0, -10.0))
      .include(LatLng(-10.0, 10.0))
      .build()
  }

  private lateinit var mapGestureDetector: MapGestureDetector
  private lateinit var transform: Transform
  private lateinit var projection: Projection
  private lateinit var uiSettings: UiSettings

  @Before
  fun setup() {
    transform = mockk(relaxed = true)
    projection = mockk(relaxed = true)
    uiSettings = mockk(relaxed = true)
    mapGestureDetector = MapGestureDetector(transform, projection, uiSettings, mockk(), mockk(), mockk())
  }

  @Test
  fun limitedOffset_returnWhenCenterOutside() {
    val outsideCoordinate = LatLng(25.0, 0.0)
    every { transform.getCenterCoordinate(false) } answers { outsideCoordinate }
    val limitedOffset =
      MapGestureDetector.findBoundsLimitedOffset(projection, transform, BOUNDS, 0f, 0f)
    assertNull(limitedOffset)
  }

  @Test
  fun limitedOffset_returnWhenWholeWorld() {
    val coordinate = LatLng(25.0, 0.0)
    every { transform.getCenterCoordinate(false) } answers { coordinate }
    val limitedOffset =
      MapGestureDetector.findBoundsLimitedOffset(projection, transform, LatLngBounds.world(), 0f, 0f)
    assertNull(limitedOffset)
  }

  @Test
  fun limitedOffset_referencePixelRepelledOfBound() {
    val coordinate = LatLng(10.0, 0.0)
    every { transform.getCenterCoordinate(false) } returns coordinate
    val referencePixel = PointF(100f, 200f)
    every { projection.toScreenLocationRaw(coordinate) } returns referencePixel
    val boundsCenterPoint = PointF(-100f, 300f)
    every { projection.toScreenLocationRaw(BOUNDS.center) } answers { boundsCenterPoint }

    val expected = PointF(
      referencePixel.x + (boundsCenterPoint.x - referencePixel.x) / BOUND_REPEL_RATIO,
      referencePixel.y + (boundsCenterPoint.y - referencePixel.y) / BOUND_REPEL_RATIO)
    MapGestureDetector.findBoundsLimitedOffset(projection, transform, BOUNDS, 0f, 0f)
    assertEquals(expected.x, referencePixel.x)
    assertEquals(expected.y, referencePixel.y)
  }

  @Test
  fun limitedOffset_noIntersection() {
    val coordinate = LatLng(1.0, -1.0)
    every { transform.getCenterCoordinate(false) } returns coordinate
    val referencePixel = PointF(50f, 50f)
    every { projection.toScreenLocationRaw(coordinate) } returns referencePixel
    val boundsCenterPoint = PointF(100f, 100f)
    every { projection.toScreenLocationRaw(BOUNDS.center) } returns boundsCenterPoint
    every { projection.toScreenLocationRaw(BOUNDS.northWest) } returns PointF(0f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.northEast) } returns PointF(200f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.southEast) } returns PointF(200f, 200f)
    every { projection.toScreenLocationRaw(BOUNDS.southWest) } returns PointF(0f, 200f)
    val acceptableVector =
      MapGestureDetector.findBoundsLimitedOffset(projection, transform, BOUNDS, 25f, 30f)
    val target = PointF(referencePixel.x + 25f, referencePixel.y + 30f)
    assertEquals(Pair(target.x - referencePixel.x, target.y - referencePixel.y), acceptableVector)
  }

  @Test
  fun limitedOffset_northIntersection() {
    val coordinate = LatLng(1.0, -1.0)
    every { transform.getCenterCoordinate(false) } returns coordinate
    val referencePixel = PointF(50f, 50f)
    every { projection.toScreenLocationRaw(coordinate) } returns referencePixel
    val boundsCenterPoint = PointF(100f, 100f)
    every { projection.toScreenLocationRaw(BOUNDS.center) } returns boundsCenterPoint
    every { projection.toScreenLocationRaw(BOUNDS.northWest) } returns PointF(0f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.northEast) } returns PointF(200f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.southEast) } returns PointF(200f, 200f)
    every { projection.toScreenLocationRaw(BOUNDS.southWest) } returns PointF(0f, 200f)
    val acceptableVector =
      MapGestureDetector.findBoundsLimitedOffset(projection, transform, BOUNDS, 25f, -200f)
    val target = PointF(referencePixel.x + 25f, referencePixel.y - 200f)
    val closestPoint = MathUtils.findSegmentsIntersection(
      referencePixel,
      target,
      projection.toScreenLocationRaw(BOUNDS.northWest),
      projection.toScreenLocationRaw(BOUNDS.northEast)
    )
    assertEquals(Pair(closestPoint!!.x - referencePixel.x, closestPoint.y - referencePixel.y), acceptableVector)
  }

  @Test
  fun limitedOffset_eastIntersection() {
    val coordinate = LatLng(1.0, -1.0)
    every { transform.getCenterCoordinate(false) } returns coordinate
    val referencePixel = PointF(50f, 50f)
    every { projection.toScreenLocationRaw(coordinate) } returns referencePixel
    val boundsCenterPoint = PointF(100f, 100f)
    every { projection.toScreenLocationRaw(BOUNDS.center) } returns boundsCenterPoint
    every { projection.toScreenLocationRaw(BOUNDS.northWest) } returns PointF(0f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.northEast) } returns PointF(200f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.southEast) } returns PointF(200f, 200f)
    every { projection.toScreenLocationRaw(BOUNDS.southWest) } returns PointF(0f, 200f)
    val acceptableVector =
      MapGestureDetector.findBoundsLimitedOffset(projection, transform, BOUNDS, 200f, 25f)
    val target = PointF(referencePixel.x + 200f, referencePixel.y + 25f)
    val closestPoint = MathUtils.findSegmentsIntersection(
      referencePixel,
      target,
      projection.toScreenLocationRaw(BOUNDS.northEast),
      projection.toScreenLocationRaw(BOUNDS.southEast)
    )
    assertEquals(Pair(closestPoint!!.x - referencePixel.x, closestPoint.y - referencePixel.y), acceptableVector)
  }

  @Test
  fun limitedOffset_southIntersection() {
    val coordinate = LatLng(1.0, -1.0)
    every { transform.getCenterCoordinate(false) } returns coordinate
    val referencePixel = PointF(50f, 50f)
    every { projection.toScreenLocationRaw(coordinate) } returns referencePixel
    val boundsCenterPoint = PointF(100f, 100f)
    every { projection.toScreenLocationRaw(BOUNDS.center) } returns boundsCenterPoint
    every { projection.toScreenLocationRaw(BOUNDS.northWest) } returns PointF(0f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.northEast) } returns PointF(200f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.southEast) } returns PointF(200f, 200f)
    every { projection.toScreenLocationRaw(BOUNDS.southWest) } returns PointF(0f, 200f)
    val acceptableVector =
      MapGestureDetector.findBoundsLimitedOffset(projection, transform, BOUNDS, 25f, 200f)
    val target = PointF(referencePixel.x + 25f, referencePixel.y + 200f)
    val closestPoint = MathUtils.findSegmentsIntersection(
      referencePixel,
      target,
      projection.toScreenLocationRaw(BOUNDS.southEast),
      projection.toScreenLocationRaw(BOUNDS.southWest)
    )
    assertEquals(Pair(closestPoint!!.x - referencePixel.x, closestPoint.y - referencePixel.y), acceptableVector)
  }

  @Test
  fun limitedOffset_westIntersection() {
    val coordinate = LatLng(1.0, -1.0)
    every { transform.getCenterCoordinate(false) } returns coordinate
    val referencePixel = PointF(50f, 50f)
    every { projection.toScreenLocationRaw(coordinate) } returns referencePixel
    val boundsCenterPoint = PointF(100f, 100f)
    every { projection.toScreenLocationRaw(BOUNDS.center) } returns boundsCenterPoint
    every { projection.toScreenLocationRaw(BOUNDS.northWest) } returns PointF(0f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.northEast) } returns PointF(200f, 0f)
    every { projection.toScreenLocationRaw(BOUNDS.southEast) } returns PointF(200f, 200f)
    every { projection.toScreenLocationRaw(BOUNDS.southWest) } returns PointF(0f, 200f)
    val acceptableVector =
      MapGestureDetector.findBoundsLimitedOffset(projection, transform, BOUNDS, -200f, 25f)
    val target = PointF(referencePixel.x - 200f, referencePixel.y + 25f)
    val closestPoint = MathUtils.findSegmentsIntersection(
      referencePixel,
      target,
      projection.toScreenLocationRaw(BOUNDS.southWest),
      projection.toScreenLocationRaw(BOUNDS.northWest)
    )
    assertEquals(Pair(closestPoint!!.x - referencePixel.x, closestPoint.y - referencePixel.y), acceptableVector)
  }
}