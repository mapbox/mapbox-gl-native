package com.mapbox.mapboxsdk.location

import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.style.layers.Layer
import io.mockk.mockk
import io.mockk.verify
import junit.framework.Assert.assertFalse
import junit.framework.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.junit.MockitoJUnitRunner

@RunWith(MockitoJUnitRunner::class)
class LocationComponentPositionManagerTest {

  private lateinit var style: Style
  private lateinit var layer: Layer

  @Before
  fun setup() {
    style = mockk(relaxed = true)
    layer = mockk(relaxed = true)
  }

  @Test
  fun update_noChange_null() {
    val positionManager = LocationComponentPositionManager(style, null, null)
    val requiresUpdate = positionManager.update(null, null)
    assertFalse(requiresUpdate)
  }

  @Test
  fun update_noChange_above() {
    val positionManager = LocationComponentPositionManager(style, "above", null)
    val requiresUpdate = positionManager.update("above", null)
    assertFalse(requiresUpdate)
  }

  @Test
  fun update_noChange_below() {
    val positionManager = LocationComponentPositionManager(style, null, "below")
    val requiresUpdate = positionManager.update(null, "below")
    assertFalse(requiresUpdate)
  }

  @Test
  fun update_fromNull_above() {
    val positionManager = LocationComponentPositionManager(style, null, null)
    val requiresUpdate = positionManager.update("above", null)
    assertTrue(requiresUpdate)
  }

  @Test
  fun update_fromNull_below() {
    val positionManager = LocationComponentPositionManager(style, null, null)
    val requiresUpdate = positionManager.update(null, "below")
    assertTrue(requiresUpdate)
  }

  @Test
  fun update_toNull_above() {
    val positionManager = LocationComponentPositionManager(style, "above", null)
    val requiresUpdate = positionManager.update(null, null)
    assertTrue(requiresUpdate)
  }

  @Test
  fun update_toNull_below() {
    val positionManager = LocationComponentPositionManager(style, null, "below")
    val requiresUpdate = positionManager.update(null, null)
    assertTrue(requiresUpdate)
  }

  @Test
  fun update_fromValue_above() {
    val positionManager = LocationComponentPositionManager(style, "above1", null)
    val requiresUpdate = positionManager.update("above2", null)
    assertTrue(requiresUpdate)
  }

  @Test
  fun update_fromValue_below() {
    val positionManager = LocationComponentPositionManager(style, null, "below1")
    val requiresUpdate = positionManager.update(null, "below2")
    assertTrue(requiresUpdate)
  }

  @Test
  fun addLayer_noModifier() {
    val positionManager = LocationComponentPositionManager(style, null, null)
    positionManager.addLayerToMap(layer)

    verify { style.addLayer(layer) }
  }

  @Test
  fun addLayer_above() {
    val positionManager = LocationComponentPositionManager(style, "above", null)
    positionManager.addLayerToMap(layer)

    verify { style.addLayerAbove(layer, "above") }
  }

  @Test
  fun addLayer_below() {
    val positionManager = LocationComponentPositionManager(style, null, "below")
    positionManager.addLayerToMap(layer)

    verify { style.addLayerBelow(layer, "below") }
  }

  @Test
  fun addLayer_afterUpdate_above() {
    val positionManager = LocationComponentPositionManager(style, null, null)
    positionManager.update("above", null)
    positionManager.addLayerToMap(layer)

    verify { style.addLayerAbove(layer, "above") }
  }

  @Test
  fun addLayer_afterUpdate_below() {
    val positionManager = LocationComponentPositionManager(style, null, null)
    positionManager.update(null, "below")
    positionManager.addLayerToMap(layer)

    verify { style.addLayerBelow(layer, "below") }
  }
}