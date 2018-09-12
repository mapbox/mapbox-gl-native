package com.mapbox.mapboxsdk.location

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
import com.mapbox.mapboxsdk.location.LocationComponentConstants.*
import com.mapbox.mapboxsdk.location.modes.RenderMode
import com.mapbox.mapboxsdk.location.utils.*
import com.mapbox.mapboxsdk.location.utils.MapboxTestingUtils.Companion.MAPBOX_HEAVY_STYLE
import com.mapbox.mapboxsdk.location.utils.MapboxTestingUtils.Companion.MAP_CONNECTION_DELAY
import com.mapbox.mapboxsdk.location.utils.MapboxTestingUtils.Companion.MAP_RENDER_DELAY
import com.mapbox.mapboxsdk.location.utils.MapboxTestingUtils.Companion.pushSourceUpdates
import com.mapbox.mapboxsdk.maps.MapboxMap
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
class LocationLayerControllerTest : BaseActivityTest() {

  @Rule
  @JvmField
  val permissionRule: GrantPermissionRule = grant(Manifest.permission.ACCESS_FINE_LOCATION)

  override fun getActivityClass(): Class<*> {
    return SingleActivity::class.java
  }

  private lateinit var styleChangeIdlingResource: StyleChangeIdlingResource
  private val location: Location by lazy {
    val initLocation = Location("")
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
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.renderMode = RenderMode.NORMAL
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.getSource(LOCATION_SOURCE), notNullValue())
      }
    }
    executeComponentTest(componentAction)
  }

  //
  // Location Layers
  //

  @Test
  fun renderModeNormal_trackingNormalLayersDoGetAdded() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.renderMode = RenderMode.NORMAL
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun renderModeCompass_bearingLayersDoGetAdded() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.renderMode = RenderMode.COMPASS
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun renderModeGps_navigationLayersDoGetAdded() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.renderMode = RenderMode.GPS
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun dontShowPuckWhenRenderModeSetAndComponentDisabled() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        component.isLocationComponentEnabled = false
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER, shouldDisappear = true)
        component.renderMode = RenderMode.GPS

        // waiting for layer to appear or timing out
        val renderCheck = {
          mapboxMap.isLayerVisible(FOREGROUND_LAYER)
        }
        waitForRenderResult(uiController, renderCheck, true)

        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun whenLocationComponentDisabled_doesSetAllLayersToVisibilityNone() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.renderMode = RenderMode.NORMAL
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        component.isLocationComponentEnabled = false
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER, shouldDisappear = true)

        // Check that all layers visibilities are set to none
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun onMapChange_locationComponentLayersDoGetRedrawn() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.renderMode = RenderMode.NORMAL
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        mapboxMap.setStyleUrl(Style.LIGHT)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(component.renderMode, `is`(equalTo(RenderMode.NORMAL)))

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
    executeComponentTest(componentAction)
  }

  @Test
  fun whenStyleChanged_continuesUsingStaleIcons() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.applyStyle(LocationComponentOptions.builder(context).staleStateTimeout(100).build())
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        uiController.loopMainThreadForAtLeast(150)

        val renderCheck = {
          mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE)
        }
        waitForRenderResult(uiController, renderCheck, true)

        assertThat(renderCheck.invoke(), `is`(true))

        mapboxMap.setStyleUrl(Style.LIGHT)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun whenStyleChanged_staleStateChanges() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.applyStyle(LocationComponentOptions.builder(context).staleStateTimeout(1).build())
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        pushSourceUpdates(styleChangeIdlingResource) {
          component.forceLocationUpdate(location)
        }
      }
    }
    executeComponentTest(componentAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun whenStyleChanged_layerVisibilityUpdates() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        var show = true
        component.activateLocationComponent(context, false)
        pushSourceUpdates(styleChangeIdlingResource) {
          component.isLocationComponentEnabled = show
          show = !show
        }

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executeComponentTest(componentAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun accuracy_visibleWithNewLocation() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(location), 16.0))
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        uiController.loopMainThreadForAtLeast(ACCURACY_RADIUS_ANIMATION_DURATION)

        assertEquals(Utils.calculateZoomLevelRadius(mapboxMap, location) /*meters projected to radius on zoom 16*/,
          mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0]
            .getNumberProperty(PROPERTY_ACCURACY_RADIUS).toFloat(), 0.1f)
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun accuracy_visibleWhenCameraEased() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val target = LatLng(location)
        val zoom = 16.0
        mapboxMap.easeCamera(CameraUpdateFactory.newLatLngZoom(target, zoom), 300)
        uiController.loopMainThreadForAtLeast(300)

        val cameraCheck = {
          Math.abs(zoom - mapboxMap.cameraPosition.zoom) < 0.1
            && Math.abs(target.latitude - mapboxMap.cameraPosition.target.latitude) < 0.1
            && Math.abs(target.longitude - mapboxMap.cameraPosition.target.longitude) < 0.1
        }
        waitForRenderResult(uiController, cameraCheck, true)

        val expectedRadius = Utils.calculateZoomLevelRadius(mapboxMap, location) /*meters projected to radius on zoom 16*/
        val renderCheck = {
          Math.abs(expectedRadius - mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getNumberProperty(PROPERTY_ACCURACY_RADIUS).toFloat()) < 0.1
        }
        waitForRenderResult(uiController, renderCheck, true)

        assertThat(renderCheck.invoke(), `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun accuracy_visibleWhenCameraMoved() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val target = LatLng(location)
        val zoom = 16.0
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(target, zoom))

        val cameraCheck = {
          Math.abs(zoom - mapboxMap.cameraPosition.zoom) < 0.1
            && Math.abs(target.latitude - mapboxMap.cameraPosition.target.latitude) < 0.1
            && Math.abs(target.longitude - mapboxMap.cameraPosition.target.longitude) < 0.1
        }
        waitForRenderResult(uiController, cameraCheck, true)

        val expectedRadius = Utils.calculateZoomLevelRadius(mapboxMap, location) /*meters projected to radius on zoom 16*/
        val renderCheck = {
          Math.abs(expectedRadius - mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getNumberProperty(PROPERTY_ACCURACY_RADIUS).toFloat()) < 0.1
        }
        waitForRenderResult(uiController, renderCheck, true)

        assertThat(renderCheck.invoke(), `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @After
  override fun afterTest() {
    super.afterTest()
    IdlingRegistry.getInstance().unregister(styleChangeIdlingResource)
  }

  private fun executeComponentTest(listener: LocationComponentAction.OnPerformLocationComponentAction) {
    onView(withId(R.id.content)).perform(LocationComponentAction(mapboxMap, listener))
  }
}
