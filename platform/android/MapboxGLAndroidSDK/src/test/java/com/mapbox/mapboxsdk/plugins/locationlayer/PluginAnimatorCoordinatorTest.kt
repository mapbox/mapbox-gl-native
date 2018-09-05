package com.mapbox.mapboxsdk.plugins.locationlayer

import android.location.Location
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerConstants.DEFAULT_TRACKING_TILT_ANIM_DURATION
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerConstants.DEFAULT_TRACKING_ZOOM_ANIM_DURATION
import com.mapbox.mapboxsdk.plugins.locationlayer.PluginAnimator.*
import junit.framework.Assert.assertEquals
import junit.framework.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.Mockito
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class PluginAnimatorCoordinatorTest {

  private lateinit var pluginAnimatorCoordinator: PluginAnimatorCoordinator
  private val cameraPosition: CameraPosition = CameraPosition.DEFAULT

  @Before
  fun setUp() {
    pluginAnimatorCoordinator = PluginAnimatorCoordinator()
  }

  @Test
  fun feedNewLocation_animatorsAreCreated() {
    pluginAnimatorCoordinator.feedNewLocation(Location(""), cameraPosition, false)

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_LATLNG] != null)
    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_GPS_BEARING] != null)
    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_LATLNG] != null)
    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_GPS_BEARING] != null)
  }

  @Test
  fun feedNewLocation_animatorValue() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    pluginAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val cameraLatLngTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_LATLNG]?.target as LatLng
    assertEquals(cameraLatLngTarget.latitude, cameraLatLngTarget.latitude)

    val layerLatLngTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_LATLNG]?.target as LatLng
    assertEquals(layerLatLngTarget.latitude, layerLatLngTarget.latitude)

    val cameraBearingTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, cameraBearingTarget)

    val layerBearingTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_isNorth_animatorsAreCreated() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    pluginAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_LATLNG] != null)
    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_GPS_BEARING] != null)
    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_LATLNG] != null)
    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_GPS_BEARING] != null)
  }

  @Test
  fun feedNewLocation_isNorth_animatorValue() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    pluginAnimatorCoordinator.feedNewLocation(location, cameraPosition, true)

    val cameraLatLngTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_LATLNG]?.target as LatLng
    assertEquals(cameraLatLngTarget.latitude, cameraLatLngTarget.latitude)

    val layerLatLngTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_LATLNG]?.target as LatLng
    assertEquals(layerLatLngTarget.latitude, layerLatLngTarget.latitude)

    val cameraBearingTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_GPS_BEARING]?.target as Float
    assertEquals(0f, cameraBearingTarget)

    val layerBearingTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, layerBearingTarget)
  }

  @Test
  fun feedNewCompassBearing_animatorsAreCreated() {
    pluginAnimatorCoordinator.feedNewCompassBearing(77f, cameraPosition)

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_COMPASS_BEARING] != null)
    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_COMPASS_BEARING] != null)
  }

  @Test
  fun feedNewCompassBearing_animatorValue() {
    val bearing = 77f
    pluginAnimatorCoordinator.feedNewCompassBearing(bearing, cameraPosition)

    val cameraBearingTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_COMPASS_BEARING]?.target as Float
    assertEquals(bearing, cameraBearingTarget)

    val layerBearingTarget = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_COMPASS_BEARING]?.target as Float
    assertEquals(bearing, layerBearingTarget)
  }

  @Test
  fun feedNewAccuracyRadius_animatorsCreated() {
    pluginAnimatorCoordinator.feedNewAccuracyRadius(150f, false)

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_ACCURACY] != null)
  }

  @Test
  fun feedNewAccuracyRadius_animatorValue() {
    val accuracy = 150f
    pluginAnimatorCoordinator.feedNewAccuracyRadius(accuracy, false)

    val layerAccuracy = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_ACCURACY]?.target as Float
    assertEquals(layerAccuracy, accuracy)

    val animationDuration = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_ACCURACY]?.duration as Long
    assertEquals(LocationLayerConstants.ACCURACY_RADIUS_ANIMATION_DURATION, animationDuration)
  }

  @Test
  fun feedNewAccuracyRadius_noAnimation_animatorsCreated() {
    pluginAnimatorCoordinator.feedNewAccuracyRadius(150f, true)

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_ACCURACY] != null)
  }

  @Test
  fun feedNewAccuracyRadius_noAnimation_animatorValue() {
    val accuracy = 150f
    pluginAnimatorCoordinator.feedNewAccuracyRadius(accuracy, true)

    val layerAccuracy = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_ACCURACY]?.target as Float
    assertEquals(layerAccuracy, accuracy)

    val animationDuration = pluginAnimatorCoordinator.animatorMap[ANIMATOR_LAYER_ACCURACY]?.duration as Long
    assertEquals(0L, animationDuration)
  }

  @Test
  fun feedNewZoomLevel_animatorsCreated() {
    pluginAnimatorCoordinator.feedNewZoomLevel(
      15.0,
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_ZOOM] != null)
  }

  @Test
  fun feedNewZoomLevel_animatorValue() {
    val zoom = 15.0f
    pluginAnimatorCoordinator.feedNewZoomLevel(
      zoom.toDouble(),
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )

    val animationDuration = pluginAnimatorCoordinator.animatorMap[ANIMATOR_ZOOM]?.duration as Long
    assertEquals(DEFAULT_TRACKING_ZOOM_ANIM_DURATION, animationDuration)

    val target = pluginAnimatorCoordinator.animatorMap[ANIMATOR_ZOOM]?.target as Float
    assertEquals(zoom, target)
  }

  @Test
  fun feedNewTiltLevel_animatorsCreated() {
    pluginAnimatorCoordinator.feedNewTilt(
      30.0,
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_TILT] != null)
  }

  @Test
  fun feedNewTiltLevel_animatorValue() {
    val tilt = 30.0f
    pluginAnimatorCoordinator.feedNewTilt(
      tilt.toDouble(),
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    val animationDuration = pluginAnimatorCoordinator.animatorMap[ANIMATOR_TILT]?.duration as Long
    assertEquals(DEFAULT_TRACKING_TILT_ANIM_DURATION, animationDuration)

    val target = pluginAnimatorCoordinator.animatorMap[ANIMATOR_TILT]?.target as Float
    assertEquals(tilt, target)
  }

  @Test
  fun cancelAllAnimators() {
    pluginAnimatorCoordinator.feedNewLocation(Location(""), cameraPosition, true)
    pluginAnimatorCoordinator.cancelAllAnimations()

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_CAMERA_LATLNG] == null)
  }

  @Test
  fun cancelZoomAnimators() {
    pluginAnimatorCoordinator.feedNewZoomLevel(
      15.0,
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )
    pluginAnimatorCoordinator.cancelZoomAnimation()

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_ZOOM] == null)
  }

  @Test
  fun cancelTiltAnimation() {
    pluginAnimatorCoordinator.feedNewTilt(
      30.0,
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    pluginAnimatorCoordinator.cancelTiltAnimation()

    assertTrue(pluginAnimatorCoordinator.animatorMap[ANIMATOR_TILT] == null)
  }

  @Test
  fun resetAllCameraAnimations_empty() {
    pluginAnimatorCoordinator.resetAllCameraAnimations(cameraPosition, false)
    assertTrue(pluginAnimatorCoordinator.animatorMap.isEmpty())
  }

  @Test
  fun addLayerListener() {
    val layerListener = Mockito.mock(OnLayerAnimationsValuesChangeListener::class.java)
    pluginAnimatorCoordinator.addLayerListener(layerListener)

    assertTrue(pluginAnimatorCoordinator.layerListeners.contains(layerListener))
  }

  @Test
  fun removeLayerListener() {
    val layerListener = Mockito.mock(OnLayerAnimationsValuesChangeListener::class.java)
    pluginAnimatorCoordinator.addLayerListener(layerListener)
    pluginAnimatorCoordinator.removeLayerListener(layerListener)

    assertTrue(pluginAnimatorCoordinator.layerListeners.isEmpty())
  }

  @Test
  fun addCameraListener() {
    val cameraListener = Mockito.mock(OnCameraAnimationsValuesChangeListener::class.java)
    pluginAnimatorCoordinator.addCameraListener(cameraListener)

    assertTrue(pluginAnimatorCoordinator.cameraListeners.contains(cameraListener))
  }

  @Test
  fun removeCameraListener() {
    val cameraListener = Mockito.mock(OnCameraAnimationsValuesChangeListener::class.java)
    pluginAnimatorCoordinator.addCameraListener(cameraListener)
    pluginAnimatorCoordinator.removeCameraListener(cameraListener)

    assertTrue(pluginAnimatorCoordinator.cameraListeners.isEmpty())
  }
}