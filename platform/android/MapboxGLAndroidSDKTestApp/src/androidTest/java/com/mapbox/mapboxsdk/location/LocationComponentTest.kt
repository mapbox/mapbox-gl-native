package com.mapbox.mapboxsdk.location

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
import android.support.test.runner.AndroidJUnit4
import android.support.v4.content.ContextCompat
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.constants.Style
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.location.LocationComponentConstants.*
import com.mapbox.mapboxsdk.location.modes.CameraMode
import com.mapbox.mapboxsdk.location.modes.RenderMode
import com.mapbox.mapboxsdk.location.utils.*
import com.mapbox.mapboxsdk.location.utils.MapboxTestingUtils.Companion.MAPBOX_HEAVY_STYLE
import com.mapbox.mapboxsdk.location.utils.MapboxTestingUtils.Companion.MAP_CONNECTION_DELAY
import com.mapbox.mapboxsdk.location.utils.MapboxTestingUtils.Companion.MAP_RENDER_DELAY
import com.mapbox.mapboxsdk.location.utils.MapboxTestingUtils.Companion.pushSourceUpdates
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest
import com.mapbox.mapboxsdk.testapp.activity.SingleActivity
import com.mapbox.mapboxsdk.utils.ColorUtils
import org.hamcrest.CoreMatchers.*
import org.junit.*
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class LocationComponentTest : BaseActivityTest() {

  @Rule
  @JvmField
  val permissionRule: GrantPermissionRule = GrantPermissionRule.grant(Manifest.permission.ACCESS_FINE_LOCATION)

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

  @Test
  fun locationComponent_initializesLocationEngineCorrectlyWhenOnesNotProvided() {
    validateTestSetup()
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context)
        component.isLocationComponentEnabled = true

        val locationEngine = component.locationEngine
        assertThat(locationEngine, notNullValue())

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(locationEngine?.isConnected, `is`(true))
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun locationComponent_initializesLocationEngineCorrectlyWhenOnesNotProvidedButHasOptions() {
    validateTestSetup()
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(
          context,
          LocationComponentOptions.builder(context)
            .staleStateTimeout(200)
            .enableStaleState(false)
            .accuracyAlpha(.5f)
            .accuracyColor(Color.BLUE)
            .build())
        component.isLocationComponentEnabled = true

        val locationEngine = component.locationEngine
        val componentOptions = component.locationComponentOptions

        assertThat(locationEngine, notNullValue())
        assertThat(componentOptions, notNullValue())

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(locationEngine?.isConnected, `is`(true))
        assertThat(componentOptions?.accuracyAlpha(), `is`(.5f))
        assertThat(componentOptions?.accuracyColor(), `is`(Color.BLUE))
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun locationComponent_doesntInitializeEngineWhenNullProvided() {
    validateTestSetup()
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(
          context,
          null,
          LocationComponentOptions.builder(context)
            .staleStateTimeout(200)
            .enableStaleState(false)
            .accuracyAlpha(.5f)
            .accuracyColor(Color.BLUE)
            .build())
        component.isLocationComponentEnabled = true

        val locationEngine = component.locationEngine
        val componentOptions = component.locationComponentOptions

        assertThat(locationEngine, nullValue())
        assertThat(componentOptions, notNullValue())

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(componentOptions?.accuracyAlpha(), `is`(.5f))
        assertThat(componentOptions?.accuracyColor(), `is`(Color.BLUE))
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun settingMapStyleImmediatelyBeforeLoadingComponent_doesStillLoadLayersProperly() {
    validateTestSetup()
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        mapboxMap.setStyle(Style.LIGHT)
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(component.renderMode, `is`(equalTo(RenderMode.NORMAL)))
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
  fun locationComponent_doesntShowUntilFirstLocationFix() {
    validateTestSetup()
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true

        // Source should be present but empty
        val mapView = (rule.activity as SingleActivity).mapView
        assertThat(mapboxMap.queryRenderedFeatures(
          RectF(0f, 0f, mapView.width.toFloat(), mapView.height.toFloat()), FOREGROUND_LAYER)
          .isEmpty(), `is`(true))

        // Force the first location update
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        // Check if the puck is visible
        assertThat(mapboxMap.queryRenderedFeatures(location, FOREGROUND_LAYER).isEmpty(), `is`(false))
      }
    }
    executeComponentTest(componentAction)
  }

  //
  // Location Layer Options
  //

  @Test
  fun locationComponentOptions_disablingStaleStateDoesWorkCorrectly() {
    validateTestSetup()
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context,
          null,
          LocationComponentOptions.builder(context)
            .staleStateTimeout(200)
            .enableStaleState(false)
            .build())
        component.isLocationComponentEnabled = true

        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        uiController.loopMainThreadForAtLeast(500)

        mapboxMap.querySourceFeatures(LOCATION_SOURCE).also {
          it.forEach {
            assertThat(it.getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))
          }
        }
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun locationComponentOptions_loadsForegroundBitmapFromNameOption() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context,
          null,
          LocationComponentOptions.builder(context)
            .foregroundName("custom-foreground-bitmap")
            .backgroundName("custom-background-bitmap")
            .foregroundStaleName("custom-foreground-stale-bitmap")
            .backgroundStaleName("custom-background-stale-bitmap")
            .bearingName("custom-bearing-bitmap")
            .build())
        component.isLocationComponentEnabled = true

        val foregroundDrawable = ContextCompat.getDrawable(context, R.drawable.ic_media_play)
        foregroundDrawable?.let {
          mapboxMap.addImageFromDrawable("custom-foreground-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-background-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-foreground-stale-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-background-stale-bitmap", it)
          mapboxMap.addImageFromDrawable("custom-bearing-bitmap", it)
        }

        component.forceLocationUpdate(location)
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

    executeComponentTest(componentAction)
  }

  @Test
  fun locationComponentOptions_loadsGpsNameWithGpsRenderMode() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context,
          null,
          LocationComponentOptions.builder(context)
            .foregroundName("custom-foreground-bitmap")
            .gpsName("custom-gps-bitmap")
            .build())
        component.isLocationComponentEnabled = true

        component.renderMode = RenderMode.GPS
        component.forceLocationUpdate(location)
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

    executeComponentTest(componentAction)
  }

  @Test
  fun locationComponentOptions_customIconNameRevertsToDefault() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context,
          null,
          LocationComponentOptions.builder(context)
            .foregroundName("custom-foreground-bitmap")
            .gpsName("custom-gps-bitmap")
            .build())
        component.isLocationComponentEnabled = true

        component.renderMode = RenderMode.GPS
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val foregroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(foregroundId, `is`(equalTo("custom-gps-bitmap")))

        component.applyStyle(LocationComponentOptions.builder(context).build())

        val renderCheck = {
          mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON) == FOREGROUND_ICON
        }
        waitForRenderResult(uiController, renderCheck, true)
        assertThat(renderCheck.invoke(), `is`(true))
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun locationComponentOptions_customGpsIconNameChangeBackWithMode() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {

        component.activateLocationComponent(context,
          null,
          LocationComponentOptions.builder(context)
            .gpsName("custom-gps-bitmap")
            .build())
        component.isLocationComponentEnabled = true

        component.renderMode = RenderMode.GPS
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val foregroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(foregroundId, `is`(equalTo("custom-gps-bitmap")))

        component.renderMode = RenderMode.NORMAL

        val renderCheck = {
          mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON) == FOREGROUND_ICON
        }
        waitForRenderResult(uiController, renderCheck, true)
        assertThat(renderCheck.invoke(), `is`(true))
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun stillStaleAfterResuming() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context,
          null,
          LocationComponentOptions.builder(context)
            .staleStateTimeout(200)
            .build())
        component.isLocationComponentEnabled = true

        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        uiController.loopMainThreadForAtLeast(250) // engaging stale state

        val renderCheck = {
          mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE)
        }
        waitForRenderResult(uiController, renderCheck, true)
        assertThat(renderCheck.invoke(), `is`(true))

        component.onStop()
        component.onStart()
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun stillNotStaleAfterResuming() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))

        component.onStop()
        component.onStart()
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun locationComponentOptions_accuracyRingWithColor() {
    val color = Color.parseColor("#4A90E2")
    val rgbaColor = ColorUtils.colorToRgbaString(color)

    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context,
          null,
          LocationComponentOptions.builder(context)
            .accuracyColor(color)
            .build())
        component.isLocationComponentEnabled = true

        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        // Check that the source property changes correctly
        mapboxMap.querySourceFeatures(LOCATION_SOURCE).also {
          it.forEach {
            assertThat(it.getStringProperty(PROPERTY_ACCURACY_COLOR), `is`(equalTo(rgbaColor)))
          }
        }
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun forceLocationUpdate_doesMoveLocationLayerIconToCorrectPosition() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val point: Point = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].geometry() as Point

        assertThat(component.locationEngine, nullValue())
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun disablingComponentHidesPuck() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val point: Point = mapboxMap.queryRenderedFeatures(location, FOREGROUND_LAYER)[0].geometry() as Point
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)

        component.isLocationComponentEnabled = false
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER, true)
        assertThat(mapboxMap.queryRenderedFeatures(location, FOREGROUND_LAYER).isEmpty(), `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun disablingComponentAndChangingStyleAllowsToEnableAgain() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        component.isLocationComponentEnabled = false
        mapboxMap.setStyle(Style.LIGHT)

        component.isLocationComponentEnabled = true
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun lifecycle_isDisabledOnStart() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        assertThat(component.isLocationComponentEnabled, `is`(false))
        component.onStop()
        component.onStart()
        assertThat(component.isLocationComponentEnabled, `is`(false))
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        assertThat(component.isLocationComponentEnabled, `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun lifecycle_keepsEnabledWhenStoppedAndStarted() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        assertThat(component.isLocationComponentEnabled, `is`(true))
        component.onStop()
        component.onStart()
        assertThat(component.isLocationComponentEnabled, `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun lifecycle_keepsDisabledWhenStoppedAndStarted() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.isLocationComponentEnabled = false
        assertThat(component.isLocationComponentEnabled, `is`(false))
        component.onStop()
        component.onStart()
        assertThat(component.isLocationComponentEnabled, `is`(false))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun lifecycle_ableToChangeStyleAfterResuming() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true

        component.onStop()
        component.onStart()

        mapboxMap.setStyle(Style.DARK)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun lifecycle_interruptedDuringStyleChange() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        mapboxMap.setStyle(Style.DARK)
        component.onStop()
        component.onStart()
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun lifecycle_forceLocationUpdateAfterStopped() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.onStop()
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE).isEmpty(), `is`(true))
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun lifecycle_acceptAndReuseLocationUpdatesBeforeLayerStarted() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.onStop()
        component.forceLocationUpdate(location)
        component.onStart()
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        val point: Point = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].geometry() as Point
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)
      }
    }
    executeComponentTest(componentAction)
  }

  @Test
  fun lifecycle_lifecycleChangeRightAfterStyleReload() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.forceLocationUpdate(location)
        mapboxMap.setStyle(Style.LIGHT)
        component.onStop()
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        component.onStart()
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
    executeComponentTest(componentAction)
  }

  @Test
  fun mapChange_settingComponentStyle() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        val options = LocationComponentOptions.builder(context)
          .accuracyColor(Color.RED)
          .build()

        pushSourceUpdates(styleChangeIdlingResource) {
          component.applyStyle(options)
        }

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executeComponentTest(componentAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun mapChange_forcingLocation() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)

        pushSourceUpdates(styleChangeIdlingResource) {
          component.forceLocationUpdate(location)
        }

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executeComponentTest(componentAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun mapChange_settingMapStyleBeforeComponentCreation() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        styleChangeIdlingResource.waitForStyle((rule.activity as SingleActivity).mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true

        val options = LocationComponentOptions.builder(context)
          .accuracyColor(Color.RED)
          .build()

        pushSourceUpdates(styleChangeIdlingResource) {
          component.forceLocationUpdate(location)
          component.applyStyle(options)
        }
      }
    }
    executeComponentTest(componentAction)

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun animators_layerBearingCorrect() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.renderMode = RenderMode.GPS
        location.bearing = 77f
        component.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(77.0, mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getNumberProperty(PROPERTY_GPS_BEARING) as Double, 0.1)

        location.bearing = 92f
        component.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY) // Waiting for the animation to finish
        assertEquals(92.0, mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getNumberProperty(PROPERTY_GPS_BEARING) as Double, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_cameraLatLngBearingCorrect() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING_GPS
        location.bearing = 77f
        component.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(77.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        component.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY) // Waiting for the animation to finish
        assertEquals(92.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_cameraBearingCorrect() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.NONE_GPS
        val latitude = mapboxMap.cameraPosition.target.latitude
        val longitude = mapboxMap.cameraPosition.target.longitude

        location.bearing = 77f
        component.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(77.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        component.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(92.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_cameraNoneCorrect() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.NONE
        val latitude = mapboxMap.cameraPosition.target.latitude
        val longitude = mapboxMap.cameraPosition.target.longitude
        val bearing = mapboxMap.cameraPosition.bearing

        location.bearing = 77f
        component.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)
        assertEquals(bearing, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        component.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY) // Waiting for the animation to finish
        assertEquals(bearing, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_focalPointAdjustment() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING
        component.cameraMode = CameraMode.NONE
        component.forceLocationUpdate(location)
        mapboxMap.waitForLayer(uiController, location, FOREGROUND_LAYER)

        assertThat(mapboxMap.uiSettings.focalPoint, nullValue())
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_dontZoomWhileNotTracking() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.NONE
        val zoom = mapboxMap.cameraPosition.zoom
        component.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIM_DURATION)

        assertEquals(zoom, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_zoomWhileTracking() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING
        component.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIM_DURATION)

        assertEquals(10.0, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  @Ignore
  fun animators_zoomWhileTrackingCanceledOnModeChange() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING
        component.zoomWhileTracking(15.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIM_DURATION / 2)
        component.cameraMode = CameraMode.NONE
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIM_DURATION / 2)

        assertEquals(15.0 / 2.0, mapboxMap.cameraPosition.zoom, 3.0)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_dontZoomWhileStopped() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true

        component.cameraMode = CameraMode.TRACKING
        val zoom = mapboxMap.cameraPosition.zoom

        component.onStop()
        component.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIM_DURATION)

        assertEquals(zoom, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  @Ignore
  fun animators_cancelZoomWhileTracking() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING
        component.zoomWhileTracking(15.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIM_DURATION / 2)
        component.cancelZoomWhileTrackingAnimation()
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIM_DURATION / 2)

        assertEquals(15.0 / 2.0, mapboxMap.cameraPosition.zoom, 3.0)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_dontTiltWhileNotTracking() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.NONE
        val tilt = mapboxMap.cameraPosition.tilt
        component.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIM_DURATION)

        assertEquals(tilt, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_tiltWhileTracking() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING
        component.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIM_DURATION)

        assertEquals(30.0, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  @Ignore
  fun animators_tiltWhileTrackingCanceledOnModeChange() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING
        component.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIM_DURATION / 2)
        component.cameraMode = CameraMode.NONE
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIM_DURATION / 2)

        assertEquals(30.0 / 2.0, mapboxMap.cameraPosition.tilt, 3.0)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun animators_dontTiltWhileStopped() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING
        val tilt = mapboxMap.cameraPosition.tilt

        component.onStop()
        component.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIM_DURATION)

        assertEquals(tilt, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  @Ignore
  fun animators_cancelTiltWhileTracking() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING
        component.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIM_DURATION / 2)
        component.cancelTiltWhileTrackingAnimation()
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIM_DURATION / 2)

        assertEquals(30.0 / 2.0, mapboxMap.cameraPosition.tilt, 3.0)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun cameraPositionAdjustedToTrackingModeWhenComponentEnabled() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        component.cameraMode = CameraMode.TRACKING_GPS
        component.forceLocationUpdate(location)
        component.isLocationComponentEnabled = false
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(LatLng(51.0, 17.0)))
        mapboxMap.moveCamera(CameraUpdateFactory.bearingTo(90.0))
        component.isLocationComponentEnabled = true
        uiController.loopMainThreadForAtLeast(MAX_ANIMATION_DURATION_MS + MAP_RENDER_DELAY)

        assertEquals(location.bearing.toDouble(), mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun compassEngine_onComponentInitializedDefaultIsProvided() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
        assertTrue(component.compassEngine is LocationComponentCompassEngine)
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun compassEngine_changesWhenNewProvided() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context, false)
        component.isLocationComponentEnabled = true
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

        component.compassEngine = engine
        assertThat(component.compassEngine, notNullValue())
        assertThat(component.compassEngine, `is`(equalTo(engine)))
      }
    }

    executeComponentTest(componentAction)
  }

  @Test
  fun defaultLocationEngine_deactivatedWhenDestroyed() {
    val componentAction = object : LocationComponentAction.OnPerformLocationComponentAction {
      override fun onLocationComponentAction(component: LocationComponent, mapboxMap: MapboxMap,
                                             uiController: UiController, context: Context) {
        component.activateLocationComponent(context)
        component.isLocationComponentEnabled = true
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(component.locationEngine?.isConnected, `is`(true))

        component.onStop()
        component.onDestroy()
        assertThat(component.locationEngine?.isConnected, `is`(false))
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