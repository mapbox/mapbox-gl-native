package com.mapbox.mapboxsdk.plugins.locationlayer

import android.Manifest
import android.R
import android.arch.lifecycle.Lifecycle
import android.content.Context
import android.graphics.Color
import android.graphics.RectF
import android.location.Location
import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.IdlingRegistry
import android.support.test.espresso.UiController
import android.support.test.espresso.assertion.ViewAssertions.matches
import android.support.test.espresso.matcher.ViewMatchers.*
import android.support.test.filters.LargeTest
import android.support.test.rule.ActivityTestRule
import android.support.test.rule.GrantPermissionRule
import android.support.test.runner.AndroidJUnit4
import android.support.v4.content.ContextCompat
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.constants.Style
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.MapboxMapOptions
import com.mapbox.mapboxsdk.maps.SupportMapFragment
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerConstants.*
import com.mapbox.mapboxsdk.plugins.locationlayer.modes.CameraMode
import com.mapbox.mapboxsdk.plugins.locationlayer.modes.RenderMode
import com.mapbox.mapboxsdk.plugins.utils.*
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.MAPBOX_HEAVY_STYLE
import com.mapbox.mapboxsdk.plugins.utils.MapboxTestingUtils.Companion.pushSourceUpdates
import com.mapbox.mapboxsdk.plugins.utils.PluginGenerationUtil.Companion.MAP_CONNECTION_DELAY
import com.mapbox.mapboxsdk.plugins.utils.PluginGenerationUtil.Companion.MAP_RENDER_DELAY
import com.mapbox.mapboxsdk.style.layers.PropertyFactory
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.activity.SingleFragmentActivity
import org.hamcrest.CoreMatchers.*
import org.junit.*
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.rules.TestName
import org.junit.runner.RunWith
import timber.log.Timber

/**
 * Test class that uses a map fragment to keep onMapReady actions isolated to within the test
 */
@RunWith(AndroidJUnit4::class)
@LargeTest
class LocationLayerPluginTest {

  @Rule
  @JvmField
  val activityRule = ActivityTestRule(SingleFragmentActivity::class.java, true, true)

  @Rule
  @JvmField
  val nameRule = TestName()

  @Rule
  @JvmField
  val permissionRule: GrantPermissionRule = GrantPermissionRule.grant(Manifest.permission.ACCESS_FINE_LOCATION)

  private lateinit var idlingResource: OnMapFragmentReadyIdlingResource
  private lateinit var styleChangeIdlingResource: StyleChangeIdlingResource
  private lateinit var fragment: SupportMapFragment
  private lateinit var mapboxMap: MapboxMap
  private val location: Location by lazy {
    val initLocation = Location("test")
    initLocation.latitude = 15.0
    initLocation.longitude = 17.0
    initLocation
  }

  @Before
  fun beforeTest() {

    // Create a default support map fragment and pass it into the empty activity
    val options = MapboxMapOptions()
      .camera(CameraPosition.Builder().zoom(2.0).build()) // to match plugins min zoom
    fragment = SupportMapFragment.newInstance(options)
    activityRule.activity.setFragment(fragment)

    Timber.e("@Before: ${nameRule.methodName} - register idle resource")
    // If idlingResource is null, throw Kotlin exception
    idlingResource = OnMapFragmentReadyIdlingResource(fragment)
    styleChangeIdlingResource = StyleChangeIdlingResource()
    IdlingRegistry.getInstance().register(idlingResource)
    IdlingRegistry.getInstance().register(styleChangeIdlingResource)
    onView(withId(R.id.content)).check(matches(isDisplayed()))
    mapboxMap = idlingResource.mapboxMap
  }

  @Test
  fun locationLayerPlugin_initializesLocationEngineCorrectlyWhenOnesNotProvided() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        val locationEngine = plugin.locationEngine
        assertThat(locationEngine, notNullValue())

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(locationEngine?.isConnected, `is`(true))
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, true))
  }

  @Test
  fun locationLayerPlugin_initializesLocationEngineCorrectlyWhenOnesNotProvidedButHasOptions() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

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

    val options = LocationLayerOptions.builder(fragment.activity)
      .staleStateTimeout(200)
      .enableStaleState(false)
      .accuracyAlpha(.5f)
      .accuracyColor(Color.BLUE)
      .build()

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(
      activityRule.activity, true, null, options))
  }

  @Test
  fun settingMapStyleImmediatelyBeforeLoadingPlugin_doesStillLoadLayersProperly() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)

        assertThat(plugin.renderMode, `is`(equalTo(RenderMode.NORMAL)))
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BACKGROUND_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(SHADOW_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
        assertThat(mapboxMap.isLayerVisible(BEARING_LAYER), `is`(false))
      }
    }

    executePluginTest(pluginAction, object : GenericPluginAction.PluginProvider<LocationLayerPlugin> {
      override fun providePlugin(mapView: MapView, mapboxMap: MapboxMap, context: Context): LocationLayerPlugin {
        // changing the style just before instantiating the plugin
        mapboxMap.setStyleUrl(Style.LIGHT)
        val plugin =
          PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false)
            .providePlugin(mapView, mapboxMap, context)
        plugin.forceLocationUpdate(location)
        return plugin
      }

      override fun isPluginDataReady(plugin: LocationLayerPlugin, mapboxMap: MapboxMap): Boolean {
        val source = mapboxMap.getSource(LOCATION_SOURCE)
        return source != null && (source as GeoJsonSource).querySourceFeatures(null).isNotEmpty()
      }
    })
  }

  @Test
  fun locationLayer_doesntShowUntilFirstLocationFix() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        // Source should be present but empty
        val mapView = fragment.view as MapView
        assertThat(mapboxMap.queryRenderedFeatures(
          RectF(0f, 0f, mapView.width.toFloat(), mapView.height.toFloat()), FOREGROUND_LAYER)
          .isEmpty(), `is`(true))

        // Force the first location update
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)

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
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(200)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        mapboxMap.querySourceFeatures(LOCATION_SOURCE).also {
          it.forEach {
            assertThat(it.getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))
          }
        }
      }
    }

    val options = LocationLayerOptions.builder(fragment.activity)
      .staleStateTimeout(200)
      .enableStaleState(false)
      .build()
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, options))
  }

  @Test
  fun locationLayerOptions_loadsForegroundBitmapFromNameOption() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        val foregroundDrawable = ContextCompat.getDrawable(context, R.drawable.ic_media_play)
        mapboxMap.addImageFromDrawable("custom-foreground-bitmap", foregroundDrawable!!)
        mapboxMap.addImageFromDrawable("custom-background-bitmap", foregroundDrawable)
        mapboxMap.addImageFromDrawable("custom-foreground-stale-bitmap", foregroundDrawable)
        mapboxMap.addImageFromDrawable("custom-background-stale-bitmap", foregroundDrawable)
        mapboxMap.addImageFromDrawable("custom-bearing-bitmap", foregroundDrawable)

        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        assertThat(mapboxMap.queryRenderedFeatures(location, FOREGROUND_LAYER).isEmpty(), `is`(false))

        val feature = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0]
        assertThat(feature.getStringProperty(PROPERTY_FOREGROUND_ICON), `is`(equalTo("custom-foreground-bitmap")))
        assertThat(feature.getStringProperty(PROPERTY_BACKGROUND_ICON), `is`(equalTo("custom-background-bitmap")))
        assertThat(feature.getStringProperty(PROPERTY_FOREGROUND_STALE_ICON), `is`(equalTo("custom-foreground-stale-bitmap")))
        assertThat(feature.getStringProperty(PROPERTY_BACKGROUND_STALE_ICON), `is`(equalTo("custom-background-stale-bitmap")))
        assertThat(feature.getStringProperty(PROPERTY_BEARING_ICON), `is`(equalTo("custom-bearing-bitmap")))
      }
    }

    val options = LocationLayerOptions.builder(fragment.activity)
      .foregroundName("custom-foreground-bitmap")
      .backgroundName("custom-background-bitmap")
      .foregroundStaleName("custom-foreground-stale-bitmap")
      .backgroundStaleName("custom-background-stale-bitmap")
      .bearingName("custom-bearing-bitmap")
      .build()
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, options))
  }

  @Test
  fun locationLayerOptions_loadsGpsNameWithGpsRenderMode() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.renderMode = RenderMode.GPS
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        val foregroundDrawable = ContextCompat.getDrawable(context, R.drawable.ic_media_play)
        mapboxMap.addImageFromDrawable("custom-foreground-bitmap", foregroundDrawable!!)
        mapboxMap.addImageFromDrawable("custom-gps-bitmap", foregroundDrawable)

        val foregroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(foregroundId, `is`(equalTo("custom-gps-bitmap")))
      }
    }

    val options = LocationLayerOptions.builder(fragment.activity)
      .foregroundName("custom-foreground-bitmap")
      .gpsName("custom-gps-bitmap")
      .build()
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, options))
  }

  @Test
  fun locationLayerOptions_customIconNameRevertsToDefault() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.renderMode = RenderMode.GPS
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        val foregroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(foregroundId, `is`(equalTo("custom-gps-bitmap")))

        plugin.applyStyle(LocationLayerOptions.builder(fragment.activity).build())
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        val revertedForegroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(revertedForegroundId, `is`(equalTo(FOREGROUND_ICON)))
      }
    }

    val options = LocationLayerOptions.builder(fragment.activity)
      .foregroundName("custom-foreground-bitmap")
      .gpsName("custom-gps-bitmap")
      .build()
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, options))
  }

  @Test
  fun locationLayerOptions_customGpsIconNameChangeBackWithMode() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.renderMode = RenderMode.GPS
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        val foregroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(foregroundId, `is`(equalTo("custom-gps-bitmap")))

        plugin.renderMode = RenderMode.NORMAL
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        val revertedForegroundId = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getStringProperty(PROPERTY_FOREGROUND_ICON)
        assertThat(revertedForegroundId, `is`(equalTo(FOREGROUND_ICON)))
      }
    }

    val options = LocationLayerOptions.builder(fragment.activity)
      .gpsName("custom-gps-bitmap")
      .build()
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, options))
  }

  @Test
  fun stillStaleAfterResuming() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(300) // engaging stale state
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(true))

        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(true))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(false))
      }
    }
    val options = LocationLayerOptions.builder(fragment.activity)
      .staleStateTimeout(200)
      .build()
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, options, false))
  }

  @Test
  fun stillNotStaleAfterResuming() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))

        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getBooleanProperty(PROPERTY_LOCATION_STALE), `is`(false))
        assertThat(mapboxMap.isLayerVisible(ACCURACY_LAYER), `is`(true))
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun locationLayerOptions_accuracyRingWithColor() {
    val color = Color.parseColor("#4A90E2")
    val rgbaColor = PropertyFactory.colorToRgbaString(color)

    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        // Check that the source property changes correctly
        mapboxMap.querySourceFeatures(LOCATION_SOURCE).also {
          it.forEach {
            assertThat(it.getStringProperty(PROPERTY_ACCURACY_COLOR), `is`(equalTo(rgbaColor)))
          }
        }
      }
    }

    val options = LocationLayerOptions.builder(fragment.activity)
      .accuracyColor(color)
      .build()
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, options))
  }

  @Test
  fun forceLocationUpdate_doesMoveLocationLayerIconToCorrectPosition() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        plugin.forceLocationUpdate(location)

        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

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
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        val point: Point = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].geometry() as Point
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)

        plugin.isLocationLayerEnabled = false
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE).isEmpty(), `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun disablingPluginAndChangingStyleAllowsToEnableAgain() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        plugin.isLocationLayerEnabled = false
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        mapboxMap.setStyle(Style.LIGHT)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        plugin.isLocationLayerEnabled = true
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        assertThat(mapboxMap.isLayerVisible(FOREGROUND_LAYER), `is`(true))
      }
    }
    executePluginTest(pluginAction)
  }

  @Test
  fun lifecycle_keepsEnabledWhenStoppedAndStarted() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        assertThat(plugin.isLocationLayerEnabled, `is`(true))
        testLifecycleOwner.handleLifecycleEvent(Lifecycle.Event.ON_PAUSE)
        testLifecycleOwner.handleLifecycleEvent(Lifecycle.Event.ON_STOP)
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        assertThat(plugin.isLocationLayerEnabled, `is`(true))
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  fun lifecycle_keepsDisabledWhenStoppedAndStarted() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.isLocationLayerEnabled = false

        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        assertThat(plugin.isLocationLayerEnabled, `is`(false))
        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        assertThat(plugin.isLocationLayerEnabled, `is`(false))
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  fun lifecycle_ableToChangeStyleAfterResuming() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)

        mapboxMap.setStyle(Style.DARK)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  fun lifecycle_interruptedDuringStyleChange() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)
        mapboxMap.setStyle(Style.DARK)

        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  fun lifecycle_forceLocationUpdateAfterStopped() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        assertThat(mapboxMap.querySourceFeatures(LOCATION_SOURCE).isEmpty(), `is`(true))
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  fun lifecycle_acceptAndReuseLocationUpdatesBeforeLayerStarted() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        plugin.forceLocationUpdate(location)
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        val point: Point = mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].geometry() as Point
        assertEquals(point.latitude(), location.latitude, 0.1)
        assertEquals(point.longitude(), location.longitude, 0.1)
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  fun lifecycle_lifecycleChangeRightAfterStyleReload() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        plugin.forceLocationUpdate(location)
        mapboxMap.setStyle(Style.LIGHT)
        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

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
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  fun mapChange_settingPluginStyle() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        styleChangeIdlingResource.waitForStyle(fragment.view as MapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        val options = LocationLayerOptions.builder(fragment.activity)
          .accuracyColor(Color.RED)
          .build()

        pushSourceUpdates(styleChangeIdlingResource) {
          plugin.applyStyle(options)
        }

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun mapChange_forcingLocation() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        styleChangeIdlingResource.waitForStyle(fragment.view as MapView, mapboxMap, MAPBOX_HEAVY_STYLE)

        pushSourceUpdates(styleChangeIdlingResource) {
          plugin.forceLocationUpdate(location)
        }

        uiController.loopMainThreadForAtLeast(MAP_CONNECTION_DELAY)
      }
    }
    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun mapChange_settingMapStyleBeforePluginCreation() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        val options = LocationLayerOptions.builder(fragment.activity)
          .accuracyColor(Color.RED)
          .build()

        pushSourceUpdates(styleChangeIdlingResource) {
          plugin.forceLocationUpdate(location)
          plugin.applyStyle(options)
        }
      }
    }

    executePluginTest(pluginAction, object : GenericPluginAction.PluginProvider<LocationLayerPlugin> {
      override fun providePlugin(mapView: MapView, mapboxMap: MapboxMap, context: Context): LocationLayerPlugin {
        // changing the style just before instantiating the plugin
        styleChangeIdlingResource.waitForStyle(mapView, mapboxMap, MAPBOX_HEAVY_STYLE)
        return PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false)
          .providePlugin(mapView, mapboxMap, context)
      }

      override fun isPluginDataReady(plugin: LocationLayerPlugin, mapboxMap: MapboxMap): Boolean {
        return true
      }
    })

    // Waiting for style to finish loading while pushing updates
    onView(withId(R.id.content)).check(matches(isDisplayed()))
  }

  @Test
  fun animators_layerBearingCorrect() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.renderMode = RenderMode.GPS
        location.bearing = 77f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(1000)
        assertEquals(77.0, mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getNumberProperty(PROPERTY_GPS_BEARING) as Double, 0.1)

        location.bearing = 92f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(2000) // Waiting for the animation to finish
        assertEquals(92.0, mapboxMap.querySourceFeatures(LOCATION_SOURCE)[0].getNumberProperty(PROPERTY_GPS_BEARING) as Double, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_cameraLatLngBearingCorrect() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING_GPS
        location.bearing = 77f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(1000)
        assertEquals(77.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(2000) // Waiting for the animation to finish
        assertEquals(92.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_cameraBearingCorrect() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.NONE_GPS
        val latitude = mapboxMap.cameraPosition.target.latitude
        val longitude = mapboxMap.cameraPosition.target.longitude

        location.bearing = 77f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(1000)
        assertEquals(77.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(2000) // Waiting for the animation to finish
        assertEquals(92.0, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_cameraNoneCorrect() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.NONE
        val latitude = mapboxMap.cameraPosition.target.latitude
        val longitude = mapboxMap.cameraPosition.target.longitude
        val bearing = mapboxMap.cameraPosition.bearing

        location.bearing = 77f
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(1000)
        assertEquals(bearing, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)

        location.bearing = 92f
        location.latitude = 30.0
        location.longitude = 35.0
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(2000) // Waiting for the animation to finish
        assertEquals(bearing, mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_focalPointAdjustment() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING
        plugin.cameraMode = CameraMode.NONE
        plugin.forceLocationUpdate(location)
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        assertThat(mapboxMap.uiSettings.focalPoint, nullValue())
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_dontZoomWhileNotTracking() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.NONE
        val zoom = mapboxMap.cameraPosition.zoom
        plugin.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION)

        assertEquals(zoom, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_zoomWhileTracking() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING
        plugin.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION)

        assertEquals(10.0, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  @Ignore
  fun animators_zoomWhileTrackingCanceledOnModeChange() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING
        plugin.zoomWhileTracking(15.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION / 2)
        plugin.cameraMode = CameraMode.NONE
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION / 2)

        assertEquals(15.0 / 2.0, mapboxMap.cameraPosition.zoom, 3.0)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_dontZoomWhileStopped() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        plugin.cameraMode = CameraMode.TRACKING
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
        val zoom = mapboxMap.cameraPosition.zoom

        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        plugin.zoomWhileTracking(10.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION)

        assertEquals(zoom, mapboxMap.cameraPosition.zoom, 0.1)
      }
    }

    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  @Ignore
  fun animators_cancelZoomWhileTracking() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING
        plugin.zoomWhileTracking(15.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION / 2)
        plugin.cancelZoomWhileTrackingAnimation()
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_ZOOM_ANIMATION_DURATION / 2)

        assertEquals(15.0 / 2.0, mapboxMap.cameraPosition.zoom, 3.0)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_dontTiltWhileNotTracking() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.NONE
        val tilt = mapboxMap.cameraPosition.tilt
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION)

        assertEquals(tilt, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_tiltWhileTracking() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION)

        assertEquals(30.0, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  @Ignore
  fun animators_tiltWhileTrackingCanceledOnModeChange() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION / 2)
        plugin.cameraMode = CameraMode.NONE
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION / 2)

        assertEquals(30.0 / 2.0, mapboxMap.cameraPosition.tilt, 3.0)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun animators_dontTiltWhileStopped() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {

        val testLifecycleOwner = TestLifecycleOwner()
        testLifecycleOwner.markState(Lifecycle.State.RESUMED)
        testLifecycleOwner.lifecycle.addObserver(plugin)

        plugin.cameraMode = CameraMode.TRACKING
        val tilt = mapboxMap.cameraPosition.tilt

        testLifecycleOwner.markState(Lifecycle.State.CREATED)
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION)

        assertEquals(tilt, mapboxMap.cameraPosition.tilt, 0.1)
      }
    }

    executePluginTest(pluginAction,
      PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity, false, null, null, false))
  }

  @Test
  @Ignore
  fun animators_cancelTiltWhileTracking() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING
        plugin.tiltWhileTracking(30.0)
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION / 2)
        plugin.cancelTiltWhileTrackingAnimation()
        uiController.loopMainThreadForAtLeast(DEFAULT_TRACKING_TILT_ANIMATION_DURATION / 2)

        assertEquals(30.0 / 2.0, mapboxMap.cameraPosition.tilt, 3.0)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun cameraPositionAdjustedToTrackingModeWhenPluginEnabled() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        plugin.cameraMode = CameraMode.TRACKING_GPS
        plugin.forceLocationUpdate(location)
        plugin.isLocationLayerEnabled = false
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(LatLng(51.0, 17.0)))
        mapboxMap.moveCamera(CameraUpdateFactory.bearingTo(90.0))
        plugin.isLocationLayerEnabled = true
        uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)

        assertEquals(location.bearing.toDouble(), mapboxMap.cameraPosition.bearing, 0.1)
        assertEquals(location.latitude, mapboxMap.cameraPosition.target.latitude, 0.1)
        assertEquals(location.longitude, mapboxMap.cameraPosition.target.longitude, 0.1)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @Test
  fun onPluginInitialized_defaultCompassEngineIsProvided() {
    val pluginAction = object : GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin> {
      override fun onGenericPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap,
                                         uiController: UiController, context: Context) {
        assertTrue(plugin.compassEngine is LocationLayerCompassEngine)
      }
    }

    executePluginTest(pluginAction, PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity))
  }

  @After
  fun afterTest() {
    Timber.e("@After: ${nameRule.methodName} - unregister idle resource")
    IdlingRegistry.getInstance().unregister(idlingResource)
    IdlingRegistry.getInstance().unregister(styleChangeIdlingResource)
  }

  private fun executePluginTest(listener: GenericPluginAction.OnPerformGenericPluginAction<LocationLayerPlugin>,
                                pluginProvider: GenericPluginAction.PluginProvider<LocationLayerPlugin> = PluginGenerationUtil.getLocationLayerPluginProvider(activityRule.activity)) {
    onView(withId(R.id.content)).perform(GenericPluginAction(fragment.view as MapView, mapboxMap, pluginProvider, listener))
  }
}