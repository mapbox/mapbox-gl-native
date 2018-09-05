package com.mapbox.mapboxsdk.plugins.locationlayer

import android.Manifest
import android.R
import android.content.Context
import android.location.Location
import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.IdlingRegistry
import android.support.test.espresso.UiController
import android.support.test.espresso.assertion.ViewAssertions.matches
import android.support.test.espresso.matcher.ViewMatchers.isDisplayed
import android.support.test.espresso.matcher.ViewMatchers.withId
import android.support.test.rule.GrantPermissionRule
import android.support.test.rule.GrantPermissionRule.grant
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.constants.Style
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerConstants.*
import com.mapbox.mapboxsdk.plugins.locationlayer.modes.RenderMode
import com.mapbox.mapboxsdk.plugins.utils.*
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.MAPBOX_HEAVY_STYLE
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.MAP_CONNECTION_DELAY
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.MAP_RENDER_DELAY
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.pushSourceUpdates
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest
import com.mapbox.mapboxsdk.testapp.activity.SingleActivity
import org.hamcrest.CoreMatchers.`is`
import org.hamcrest.CoreMatchers.notNullValue
import org.hamcrest.Matchers.equalTo
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertThat
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class LocationLayerTest : BaseActivityTest() {

  @Rule
  @JvmField
  val permissionRule: GrantPermissionRule = grant(Manifest.permission.ACCESS_FINE_LOCATION)

  override fun getActivityClass(): Class<*> {
    return SingleActivity::class.java
  }

  private lateinit var styleChangeIdlingResource: StyleChangeIdlingResource
  private val location: Location by lazy {
    val initLocation = Location("test")
    initLocation.latitude = 15.0
    initLocation.longitude = 17.0
    initLocation.bearing = 10f
    initLocation.accuracy = 150f
    initLocation
  }

  @Before
  override fun beforeTest() {
    super.beforeTest()
    styleChangeIdlingResource = StyleChangeIdlingResource()
    IdlingRegistry.getInstance().register(styleChangeIdlingResource)
  }

  //
  // Location Source
  //

  @Test
  fun renderModeNormal_sourceDoesGetAdded() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.renderMode = RenderMode.NORMAL
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.getSource(LOCATION_SOURCE), notNullValue())
      }
    }
    executePluginTest(pluginAction)
  }

  //
  // Location Layers
  //

  @Test
  fun renderModeNormal_trackingNormalLayersDoGetAdded() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.renderMode = RenderMode.NORMAL
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun renderModeCompass_bearingLayersDoGetAdded() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.renderMode = RenderMode.COMPASS
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun renderModeGps_navigationLayersDoGetAdded() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.renderMode = RenderMode.GPS
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun dontShowPuckWhenRenderModeSetAndPluginDisabled() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        plugin.deactivateLocationLayerPlugin()
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER, shouldDisappear = true)
        plugin.renderMode = RenderMode.GPS
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun whenLocationLayerPluginDisabled_doesSetAllLayersToVisibilityNone() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.renderMode = RenderMode.NORMAL
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        plugin.deactivateLocationLayerPlugin()
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER, shouldDisappear = true)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        // Check that all layers visibilities are set to none
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun onMapChange_locationLayerLayersDoGetRedrawn() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.renderMode = RenderMode.NORMAL
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        mapboxMap.setStyleUrl(Style.LIGHT)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(plugin.renderMode, `is`(equalTo(RenderMode.NORMAL)))

        // Check that the Source has been re-added to the new map style
        val source: GeoJsonSource? = mapboxMap.getSourceAs(LOCATION_SOURCE)
        assertThat(source, notNullValue())

        // Check that all layers visibilities are set to visible
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun whenStyleChanged_continuesUsingStaleIcons() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.applyStyle(LocationLayerOptions.builder(context).staleStateTimeout(100).build())
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        uiController.loopMainThreadForAtLeast(150)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(true))

        mapboxMap.setStyleUrl(Style.LIGHT)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun whenStyleChanged_staleStateChanges() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.applyStyle(LocationLayerOptions.builder(context).staleStateTimeout(1).build())
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        pushSourceUpdates(styleChangeIdlingResource) {
          plugin.forceLocationUpdate(location)
        }
      }
    }
    executePluginTest(pluginAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun whenStyleChanged_layerVisibilityUpdates() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        var show = true
        pushSourceUpdates(styleChangeIdlingResource) {
          if (show) {
            plugin.activateLocationLayerPlugin(context, false)
          } else {
            plugin.deactivateLocationLayerPlugin()
          }
          show = !show
        }

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun accuracy_visibleWithNewLocation() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(location), 16.0))
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        uiController.loopMainThreadForAtLeast(ACCURACY_RADIUS_ANIMATION_DURATION)

        assertEquals(Utils.calculateZoomLevelRadius(mapboxMap, location) /*meters projected to radius on zoom 16*/,
          mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0]
            .getNumberProperty(PROPERTY_ACCURACY_RADIUS).toFloat(), 0.1f)
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun accuracy_visibleWhenCameraEased() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        mapboxMap.easeCamera(CameraUpdateFactory.newLatLngZoom(LatLng(location), 16.0), 300)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY + 300)

        assertEquals(Utils.calculateZoomLevelRadius(mapboxMap, location) /*meters projected to radius on zoom 16*/,
          mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0]
            .getNumberProperty(PROPERTY_ACCURACY_RADIUS).toFloat(), 0.1f)
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun accuracy_visibleWhenCameraMoved() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(location), 16.0))
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY + 300)

        assertEquals(Utils.calculateZoomLevelRadius(mapboxMap, location) /*meters projected to radius on zoom 16*/,
          mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0]
            .getNumberProperty(PROPERTY_ACCURACY_RADIUS).toFloat(), 0.1f)
      }
    }
    executePluginTest(pluginAction)
  }

  @After
  override fun afterTest() {
    super.afterTest()
    IdlingRegistry.getInstance().unregister(styleChangeIdlingResource)
  }

  private fun executePluginTest(listener: LocationLayerPluginAction.OnPerformLocationLayerPluginAction) {
    onView(withId(R.id.content)).perform(LocationLayerPluginAction(mapboxMap, listener))
  }
}
