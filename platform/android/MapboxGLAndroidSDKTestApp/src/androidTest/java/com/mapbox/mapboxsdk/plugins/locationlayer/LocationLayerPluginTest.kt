package com.mapbox.mapboxsdk.plugins.locationlayer

import android.Manifest
import android.R
import android.content.Context
import android.graphics.Color
import android.graphics.RectF
import android.location.Location
import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.IdlingRegistry
import android.support.test.espresso.UiController
import android.support.test.espresso.assertion.ViewAssertions.matches
import android.support.test.espresso.matcher.ViewMatchers.*
import android.support.test.rule.GrantPermissionRule
import android.support.v4.content.ContextCompat
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.constants.Style
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerConstants.*
import com.mapbox.mapboxsdk.plugins.locationlayer.modes.CameraMode
import com.mapbox.mapboxsdk.plugins.locationlayer.modes.RenderMode
import com.mapbox.mapboxsdk.plugins.utils.*
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.MAPBOX_HEAVY_STYLE
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.MAP_CONNECTION_DELAY
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.MAP_RENDER_DELAY
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.pushSourceUpdates
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest
import com.mapbox.mapboxsdk.testapp.activity.SingleActivity
import com.mapbox.mapboxsdk.utils.ColorUtils
import org.hamcrest.CoreMatchers.*
import org.junit.*
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue

class LocationLayerPluginTest : BaseActivityTest() {

  @Rule
  @JvmField
  val permissionRule: GrantPermissionRule = GrantPermissionRule.grant(Manifest.permission.ACCESS_FINE_LOCATION)

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

    location.latitude = 15.0
    location.longitude = 17.0
    location.bearing = 10f
    location.accuracy = 150f

    styleChangeIdlingResource = StyleChangeIdlingResource()
    IdlingRegistry.getInstance().register(styleChangeIdlingResource)
  }

  @Test
  fun locationLayerPlugin_initializesLocationEngineCorrectlyWhenOnesNotProvided() {
    validateTestSetup()
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context)

        val locationEngine = plugin.locationEngine
        assertThat(locationEngine, notNullValue())

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(locationEngine?.isConnected, `is`(true))
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun locationLayerPlugin_initializesLocationEngineCorrectlyWhenOnesNotProvidedButHasOptions() {
    validateTestSetup()
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(
          context,
          LocationLayerOptions.builder(context)
            .staleStateTimeout(200)
            .enableStaleState(false)
            .accuracyAlpha(.5f)
            .accuracyColor(Color.BLUE)
            .build())

        val locationEngine = plugin.locationEngine
        val pluginOptions = plugin.locationLayerOptions

        assertThat(locationEngine, notNullValue())
        assertThat(pluginOptions, notNullValue())

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(locationEngine?.isConnected, `is`(true))
        assertThat(pluginOptions?.accuracyAlpha(), `is`(.5f))
        assertThat(pluginOptions?.accuracyColor(), `is`(Color.BLUE))
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun locationLayerPlugin_doesntInitializeEngineWhenNullProvided() {
    validateTestSetup()
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(
          null,
          LocationLayerOptions.builder(context)
            .staleStateTimeout(200)
            .enableStaleState(false)
            .accuracyAlpha(.5f)
            .accuracyColor(Color.BLUE)
            .build())

        val locationEngine = plugin.locationEngine
        val pluginOptions = plugin.locationLayerOptions

        assertThat(locationEngine, nullValue())
        assertThat(pluginOptions, notNullValue())

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(pluginOptions?.accuracyAlpha(), `is`(.5f))
        assertThat(pluginOptions?.accuracyColor(), `is`(Color.BLUE))
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun settingMapStyleImmediatelyBeforeLoadingPlugin_doesStillLoadLayersProperly() {
    validateTestSetup()
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        mapboxMap.setStyle(Style.LIGHT)
        plugin.activateLocationLayerPlugin(context, false)
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(plugin.renderMode, `is`(equalTo(RenderMode.NORMAL)))
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
  fun locationLayer_doesntShowUntilFirstLocationFix() {
    validateTestSetup()
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)

        // Source should be present but empty
        val mapView = (rule.activity as SingleActivity).mapView
        assertThat(mapboxMap.queryRenderedFeatures(
          RectF(0f, 0f, mapView.width.toFloat(), mapView.height.toFloat()), FOREGROUND_LAYER)
          .isEmpty(), `is`(true))

        // Force the first location update
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        // Check if the puck is visible
        assertThat(mapboxMap.queryRenderedFeatures(location, FOREGROUND_LAYER).isEmpty(), `is`(false))
      }
    }
    executePluginTest(pluginAction)
  }

  //
  // Location Layer Options
  //

  @Test
  fun locationLayerOptions_disablingStaleStateDoesWorkCorrectly() {
    validateTestSetup()
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context,
          LocationLayerOptions.builder(context)
            .staleStateTimeout(200)
            .enableStaleState(false)
            .build())

        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        uiController.loopMainThreadForAtLeast(200)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        mapboxMap.querySourceFeatures(LOCATION_SOURCE).also {
          it.forEach {
            assertThat(it.getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))
          }
        }
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun locationLayerOptions_loadsForegroundBitmapFromNameOption() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context,
          LocationLayerOptions.builder(context)
            .foregroundName("custom-foreground-bitmap")
            .backgroundName("custom-background-bitmap")
            .foregroundStaleName("custom-foreground-stale-bitmap")
            .backgroundStaleName("custom-background-stale-bitmap")
            .bearingName("custom-bearing-bitmap")
            .build())

        val foregroundDrawable = ContextCompat.getDrawable(context, R.drawable.ic_media_play)
        foregroundDrawable?.let {
          mapboxMap.addImageFromDrawable("custom-foreground-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-background-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-foreground-stale-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-background-stale-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-bearing-bitmap", it)
        }

        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.queryRenderedFeatures(location, FOREGROUND_LAYER).isEmpty(), `is`(false))

        val feature = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0]
        assertThat(feature.getStringProperty(PROPERTY_FOREGROUND_ICON), `is`(equalTo("custom-foreground-bitmap")))
        assertThat(feature.getStringProperty(PROPERTY_BACKGROUND_ICON), `is`(equalTo("custom-background-bitmap")))
        assertThat(feature.getStringProperty(PROPERTY_FOREGROUND_STALE_ICON), `is`(equalTo("custom-foreground-stale-bitmap")))
        assertThat(feature.getStringProperty(PROPERTY_BACKGROUND_STALE_ICON), `is`(equalTo("custom-background-stale-bitmap")))
        assertThat(feature.getStringProperty(PROPERTY_BEARING_ICON), `is`(equalTo("custom-bearing-bitmap")))
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun locationLayerOptions_loadsGpsNameWithGpsRenderMode() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context,
          LocationLayerOptions.builder(context)
            .foregroundName("custom-foreground-bitmap")
            .gpsName("custom-gps-bitmap")
            .build())

        plugin.renderMode = RenderMode.GPS
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        val foregroundDrawable = ContextCompat.getDrawable(context, R.drawable.ic_media_play)
        foregroundDrawable?.let {
          mapboxMap.addImageFromDrawable("custom-foreground-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-gps-bitmap", it)
        }

        val foregroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(foregroundId, `is`(equalTo("custom-gps-bitmap")))
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun locationLayerOptions_customIconNameRevertsToDefault() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context,
          LocationLayerOptions.builder(context)
            .foregroundName("custom-foreground-bitmap")
            .gpsName("custom-gps-bitmap")
            .build())

        plugin.renderMode = RenderMode.GPS
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val foregroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(foregroundId, `is`(equalTo("custom-gps-bitmap")))

        plugin.applyStyle(LocationLayerOptions.builder(context).build())
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        val revertedForegroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(revertedForegroundId, `is`(equalTo(FOREGROUND_ICON)))
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun locationLayerOptions_customGpsIconNameChangeBackWithMode() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {

        plugin.activateLocationLayerPlugin(context,
          LocationLayerOptions.builder(context)
            .gpsName("custom-gps-bitmap")
            .build())

        plugin.renderMode = RenderMode.GPS
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val foregroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(foregroundId, `is`(equalTo("custom-gps-bitmap")))

        plugin.renderMode = RenderMode.NORMAL
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        val revertedForegroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(revertedForegroundId, `is`(equalTo(FOREGROUND_ICON)))
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun stillStaleAfterResuming() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context,
          LocationLayerOptions.builder(context)
            .staleStateTimeout(200)
            .build())

        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        uiController.loopMainThreadForAtLeast(250) // engaging stale state
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(true))

        plugin.onStop()
        plugin.onStart()
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun stillNotStaleAfterResuming() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))

        plugin.onStop()
        plugin.onStart()
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun locationLayerOptions_accuracyRingWithColor() {
    val color = Color.parseColor("#4A90E2")
    val rgbaColor = ColorUtils.colorToRgbaString(color)

    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context,
          LocationLayerOptions.builder(context)
            .accuracyColor(color)
            .build())

        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        // Check that the source property changes correctly
        mapboxMap.querySourceFeatures(LOCATION_SOURCE).also {
          it.forEach {
            assertThat(it.getStringProperty(PROPERTY_ACCURACY_COLOR), `is`(equalTo(rgbaColor)))
          }
        }
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun forceLocationUpdate_doesMoveLocationLayerIconToCorrectPosition() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val point: Point = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].geometry() as Point

        assertThat(plugin.locationEngine, nullValue())
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun disablingPluginHidesPuck() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val point: Point = mapboxMap.queryRenderedFeatures(location, FOREGROUND_LAYER)[0].geometry() as Point
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)

        plugin.deactivateLocationLayerPlugin()
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.queryRenderedFeatures(location, FOREGROUND_LAYER).isEmpty(), `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun disablingPluginAndChangingStyleAllowsToEnableAgain() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        plugin.deactivateLocationLayerPlugin()
        mapboxMap.setStyle(Style.LIGHT)

        plugin.activateLocationLayerPlugin(context, false)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_isDisabledOnStart() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        assertThat(plugin.isLocationLayerEnabled, `is`(false))
        plugin.onStop()
        plugin.onStart()
        assertThat(plugin.isLocationLayerEnabled, `is`(false))
        plugin.activateLocationLayerPlugin(context, false)
        assertThat(plugin.isLocationLayerEnabled, `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_keepsEnabledWhenStoppedAndStarted() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        assertThat(plugin.isLocationLayerEnabled, `is`(true))
        plugin.onStop()
        plugin.onStart()
        assertThat(plugin.isLocationLayerEnabled, `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_keepsDisabledWhenStoppedAndStarted() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.deactivateLocationLayerPlugin()
        assertThat(plugin.isLocationLayerEnabled, `is`(false))
        plugin.onStop()
        plugin.onStart()
        assertThat(plugin.isLocationLayerEnabled, `is`(false))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_ableToChangeStyleAfterResuming() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)

        plugin.onStop()
        plugin.onStart()

        mapboxMap.setStyle(Style.DARK)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_interruptedDuringStyleChange() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        mapboxMap.setStyle(Style.DARK)
        plugin.onStop()
        plugin.onStart()
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_forceLocationUpdateAfterStopped() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.onStop()
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE).isEmpty(), `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_acceptAndReuseLocationUpdatesBeforeLayerStarted() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.onStop()
        plugin.forceLocationUpdate(location)
        plugin.onStart()
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val point: Point = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].geometry() as Point
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_lifecycleChangeRightAfterStyleReload() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.forceLocationUpdate(location)
        mapboxMap.setStyle(Style.LIGHT)
        plugin.onStop()
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        plugin.onStart()
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val point: Point = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].geometry() as Point
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)

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
  fun mapChange_settingPluginStyle() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        val options = LocationLayerOptions.builder(context)
          .accuracyColor(Color.RED)
          .build()

        pushSourceUpdates(styleChangeIdlingResource) {
          plugin.applyStyle(options)
        }

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun mapChange_forcingLocation() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)

        pushSourceUpdates(styleChangeIdlingResource) {
          plugin.forceLocationUpdate(location)
        }

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun mapChange_settingMapStyleBeforePluginCreation() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        plugin.activateLocationLayerPlugin(context, false)

        val options = LocationLayerOptions.builder(context)
          .accuracyColor(Color.RED)
          .build()

        pushSourceUpdates(styleChangeIdlingResource) {
          plugin.forceLocationUpdate(location)
          plugin.applyStyle(options)
        }
      }
    }
    executePluginTest(pluginAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun animators_layerBearingCorrect() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.renderMode = RenderMode.GPS
        location.bearing = 77f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(77.0, mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getNumberProperty(PROPERTY_GPS_BEARING) as Double, 0.1)

        location.bearing = 92f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY) // Waiting for the animation to finish
        assertEquals(92.0, mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getNumberProperty(PROPERTY_GPS_BEARING) as Double, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_cameraLatLngBearingCorrect() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING_GPS
        location.bearing = 77f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(77.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY) // Waiting for the animation to finish
        assertEquals(92.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_cameraBearingCorrect() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.NONE_GPS
        val latitude = mapboxMap.cameraPosition.target.latitude
        val longitude = mapboxMap.cameraPosition.target.longitude

        location.bearing = 77f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(77.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(92.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_cameraNoneCorrect() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.NONE
        val latitude = mapboxMap.cameraPosition.target.latitude
        val longitude = mapboxMap.cameraPosition.target.longitude
        val bearing = mapboxMap.cameraPosition.bearing

        location.bearing = 77f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(bearing, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY) // Waiting for the animation to finish
        assertEquals(bearing, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_focalPointAdjustment() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING
        plugin.cameraMode = CameraMode.NONE
        plugin.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.uiSettings.focalPoint, nullValue())
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_dontZoomWhileNotTracking() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.NONE
        val zoom = mapboxMap.cameraPosition.zoom
        plugin.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION)

        assertEquals(zoom, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_zoomWhileTracking() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING
        plugin.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION)

        assertEquals(10.0, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  @Ignore
  fun animators_zoomWhileTrackingCanceledOnModeChange() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING
        plugin.zoomWhileTracking(15.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION / 2)
        plugin.cameraMode = CameraMode.NONE
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION / 2)

        assertEquals(15.0 / 2.0, mapboxMap.cameraPosition.zoom, 3.0)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_dontZoomWhileStopped() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)

        plugin.cameraMode = CameraMode.TRACKING
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        val zoom = mapboxMap.cameraPosition.zoom

        plugin.onStop()
        plugin.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION)

        assertEquals(zoom, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  @Ignore
  fun animators_cancelZoomWhileTracking() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING
        plugin.zoomWhileTracking(15.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION / 2)
        plugin.cancelZoomWhileTrackingAnimation()
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION / 2)

        assertEquals(15.0 / 2.0, mapboxMap.cameraPosition.zoom, 3.0)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_dontTiltWhileNotTracking() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.NONE
        val tilt = mapboxMap.cameraPosition.tilt
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION)

        assertEquals(tilt, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_tiltWhileTracking() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION)

        assertEquals(30.0, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  @Ignore
  fun animators_tiltWhileTrackingCanceledOnModeChange() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION / 2)
        plugin.cameraMode = CameraMode.NONE
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION / 2)

        assertEquals(30.0 / 2.0, mapboxMap.cameraPosition.tilt, 3.0)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun animators_dontTiltWhileStopped() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING
        val tilt = mapboxMap.cameraPosition.tilt

        plugin.onStop()
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION)

        assertEquals(tilt, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  @Ignore
  fun animators_cancelTiltWhileTracking() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION / 2)
        plugin.cancelTiltWhileTrackingAnimation()
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION / 2)

        assertEquals(30.0 / 2.0, mapboxMap.cameraPosition.tilt, 3.0)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun cameraPositionAdjustedToTrackingModeWhenPluginEnabled() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        plugin.cameraMode = CameraMode.TRACKING_GPS
        plugin.forceLocationUpdate(location)
        plugin.deactivateLocationLayerPlugin()
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(LatLng(51.0, 17.0)))
        mapboxMap.moveCamera(CameraUpdateFactory.bearingTo(90.0))
        plugin.activateLocationLayerPlugin(context, false)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)

        assertEquals(location.bearing.toDouble(), mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun compassEngine_onPluginInitializedDefaultIsProvided() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        assertTrue(plugin.compassEngine is LocationLayerCompassEngine)
      }
    }

    executePluginTest(pluginAction)
  }

  @Test
  fun compassEngine_changesWhenNewProvided() {
    val pluginAction = object : LocationLayerPluginAction.OnPerformLocationLayerPluginAction {
      override fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                               uiController: UiController, context: Context) {
        plugin.activateLocationLayerPlugin(context, false)
        val engine: CompassEngine = object : CompassEngine {
          override fun addCompassListener(compassListener: CompassListener) {
          }

          override fun removeCompassListener(compassListener: CompassListener) {
          }

          override fun getLastHeading(): Float {
            return 0f
          }

          override fun getLastAccuracySensorStatus(): Int {
            return 0
          }

          override fun onStart() {
          }

          override fun onStop() {
          }
        }

        plugin.compassEngine = engine
        assertThat(plugin.compassEngine, notNullValue())
        assertThat(plugin.compassEngine, `is`(equalTo(engine)))
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