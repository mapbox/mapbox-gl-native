package com.mapbox.mapboxsdk.location

import android.animation.Animator
import android.location.Location
import android.util.SparseArray
import android.view.animation.LinearInterpolator
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_TRACKING_TILT_ANIM_DURATION
import com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_TRACKING_ZOOM_ANIM_DURATION
import com.mapbox.mapboxsdk.location.MapboxAnimator.*
import com.mapbox.mapboxsdk.location.modes.CameraMode
import com.mapbox.mapboxsdk.location.modes.RenderMode
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Projection
import io.mockk.*
import junit.framework.Assert.assertEquals
import junit.framework.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.Mockito
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class LocationAnimatorCoordinatorTest {

  private lateinit var locationAnimatorCoordinator: LocationAnimatorCoordinator
  private val cameraPosition: CameraPosition = CameraPosition.DEFAULT

  private val animatorProvider: MapboxAnimatorProvider = mockk()
  private val animatorSetProvider: MapboxAnimatorSetProvider = mockk()

  private val projection: Projection = mockk()

  @Before
  fun setUp() {
    locationAnimatorCoordinator = LocationAnimatorCoordinator(projection, animatorSetProvider, animatorProvider)
    configureAnimatorProvider()
    every { projection.getMetersPerPixelAtLatitude(any()) } answers { 1.0 }
    every { animatorSetProvider.startAnimation(any(), any(), any()) } answers {}
    locationAnimatorCoordinator.updateAnimatorListenerHolders(getListenerHoldersSet(
      ANIMATOR_LAYER_LATLNG,
      ANIMATOR_CAMERA_LATLNG,
      ANIMATOR_LAYER_GPS_BEARING,
      ANIMATOR_LAYER_COMPASS_BEARING,
      ANIMATOR_CAMERA_GPS_BEARING,
      ANIMATOR_CAMERA_COMPASS_BEARING,
      ANIMATOR_LAYER_ACCURACY,
      ANIMATOR_ZOOM,
      ANIMATOR_TILT
    ))
  }

  private fun configureAnimatorProvider() {
    // workaround https://github.com/mockk/mockk/issues/229#issuecomment-457816131
    registerInstanceFactory { AnimationsValueChangeListener<Float> {} }
    registerInstanceFactory { AnimationsValueChangeListener<LatLng> {} }
    val previousFloatSlot = slot<Float>()
    val targetFloatSlot = slot<Float>()
    val listenerSlot = slot<MapboxAnimator.AnimationsValueChangeListener<*>>()
    val maxFpsSlot = slot<Int>()
    every {
      animatorProvider.floatAnimator(capture(previousFloatSlot), capture(targetFloatSlot), capture(listenerSlot), capture(maxFpsSlot))
    } answers {
      MapboxFloatAnimator(previousFloatSlot.captured, targetFloatSlot.captured, listenerSlot.captured, maxFpsSlot.captured)
    }

    val previousLatLngSlot = slot<LatLng>()
    val targetLatLngSlot = slot<LatLng>()
    every {
      animatorProvider.latLngAnimator(capture(previousLatLngSlot), capture(targetLatLngSlot), capture(listenerSlot), capture(maxFpsSlot))
    } answers {
      MapboxLatLngAnimator(previousLatLngSlot.captured, targetLatLngSlot.captured, listenerSlot.captured, maxFpsSlot.captured)
    }

    val callback = slot<MapboxMap.CancelableCallback>()
    every {
      animatorProvider.cameraAnimator(capture(previousFloatSlot), capture(targetFloatSlot), capture(listenerSlot), capture(callback))
    } answers {
      MapboxCameraAnimatorAdapter(previousFloatSlot.captured, targetFloatSlot.captured, listenerSlot.captured, callback.captured)
    }
    every {
      animatorProvider.cameraAnimator(capture(previousFloatSlot), capture(targetFloatSlot), capture(listenerSlot), null)
    } answers {
      MapboxCameraAnimatorAdapter(previousFloatSlot.captured, targetFloatSlot.captured, listenerSlot.captured, null)
    }
  }

  @Test
  fun feedNewLocation_animatorsAreCreated() {
    locationAnimatorCoordinator.feedNewLocation(Location(""), cameraPosition, false)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_GPS_BEARING] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_LATLNG] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING] != null)
  }

  @Test
  fun feedNewLocation_animatorValue() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val cameraLatLngTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG]?.target as LatLng
    assertEquals(cameraLatLngTarget.latitude, cameraLatLngTarget.latitude)

    val layerLatLngTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_LATLNG]?.target as LatLng
    assertEquals(layerLatLngTarget.latitude, layerLatLngTarget.latitude)

    val cameraBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, cameraBearingTarget)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_animatorValue_correctRotation_1() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 0f

    val animator = mockk<MapboxFloatAnimator>(relaxed = true)
    every { animator.animatedValue } returns 270f
    locationAnimatorCoordinator.animatorArray.put(ANIMATOR_LAYER_GPS_BEARING, animator)

    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(360f, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_animatorValue_correctRotation_2() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 90f

    val animator = mockk<MapboxFloatAnimator>(relaxed = true)
    every { animator.animatedValue } returns 280f
    locationAnimatorCoordinator.animatorArray.put(ANIMATOR_LAYER_GPS_BEARING, animator)

    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(450f, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_animatorValue_correctRotation_3() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 300f

    val animator = mockk<MapboxFloatAnimator>(relaxed = true)
    every { animator.animatedValue } returns 450f
    locationAnimatorCoordinator.animatorArray.put(ANIMATOR_LAYER_GPS_BEARING, animator)

    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(-60f, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_animatorValue_correctRotation_4() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 350f

    val animator = mockk<MapboxFloatAnimator>(relaxed = true)
    every { animator.animatedValue } returns 10f
    locationAnimatorCoordinator.animatorArray.put(ANIMATOR_LAYER_GPS_BEARING, animator)

    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(-10f, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_animatorValue_correctRotation_5() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 90f

    val animator = mockk<MapboxFloatAnimator>(relaxed = true)
    every { animator.animatedValue } returns -280f
    locationAnimatorCoordinator.animatorArray.put(ANIMATOR_LAYER_GPS_BEARING, animator)

    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(90f, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_animatorValue_correctRotation_6() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 270f

    val animator = mockk<MapboxFloatAnimator>(relaxed = true)
    every { animator.animatedValue } returns -350f
    locationAnimatorCoordinator.animatorArray.put(ANIMATOR_LAYER_GPS_BEARING, animator)

    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(-90f, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_isNorth_animatorsAreCreated() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_GPS_BEARING] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_LATLNG] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING] != null)
  }

  @Test
  fun feedNewLocation_isNorth_animatorValue() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, true)

    val cameraLatLngTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG]?.target as LatLng
    assertEquals(cameraLatLngTarget.latitude, cameraLatLngTarget.latitude)

    val layerLatLngTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_LATLNG]?.target as LatLng
    assertEquals(layerLatLngTarget.latitude, layerLatLngTarget.latitude)

    val cameraBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_GPS_BEARING]?.target as Float
    assertEquals(0f, cameraBearingTarget)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, layerBearingTarget)
  }

  @Test
  fun feedNewCompassBearing_animatorsAreCreated() {
    locationAnimatorCoordinator.feedNewCompassBearing(77f, cameraPosition)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_COMPASS_BEARING] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_COMPASS_BEARING] != null)
  }

  @Test
  fun feedNewCompassBearing_animatorValue() {
    val bearing = 77f
    locationAnimatorCoordinator.feedNewCompassBearing(bearing, cameraPosition)

    val cameraBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_COMPASS_BEARING]?.target as Float
    assertEquals(bearing, cameraBearingTarget)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_COMPASS_BEARING]?.target as Float
    assertEquals(bearing, layerBearingTarget)
  }

  @Test
  fun feedNewAccuracyRadius_animatorsCreated() {
    locationAnimatorCoordinator.feedNewAccuracyRadius(150f, false)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY] != null)
  }

  @Test
  fun feedNewAccuracyRadius_animatorValue() {
    val accuracy = 150f
    locationAnimatorCoordinator.feedNewAccuracyRadius(accuracy, false)

    val layerAccuracy = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY]?.target as Float
    assertEquals(layerAccuracy, accuracy)
  }

  @Test
  fun feedNewAccuracyRadius_noAnimation_animatorsCreated() {
    locationAnimatorCoordinator.feedNewAccuracyRadius(150f, true)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY] != null)
  }

  @Test
  fun feedNewAccuracyRadius_noAnimation_animatorValue() {
    val accuracy = 150f
    locationAnimatorCoordinator.feedNewAccuracyRadius(accuracy, true)

    val layerAccuracy = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY]?.target as Float
    assertEquals(layerAccuracy, accuracy)
  }

  @Test
  fun feedNewZoomLevel_animatorsCreated() {
    locationAnimatorCoordinator.feedNewZoomLevel(
      15.0,
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_ZOOM] != null)
  }

  @Test
  fun feedNewZoomLevel_animatorValue() {
    val zoom = 15.0f
    locationAnimatorCoordinator.feedNewZoomLevel(
      zoom.toDouble(),
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )

    val animator = locationAnimatorCoordinator.animatorArray[ANIMATOR_ZOOM]
    assertEquals(zoom, animator.target)
    verify { animatorSetProvider.startAnimation(eq(listOf(animator)), any<LinearInterpolator>(), DEFAULT_TRACKING_ZOOM_ANIM_DURATION) }
  }

  @Test
  fun feedNewTiltLevel_animatorsCreated() {
    locationAnimatorCoordinator.feedNewTilt(
      30.0,
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_TILT] != null)
  }

  @Test
  fun feedNewTiltLevel_animatorValue() {
    val tilt = 30.0f
    locationAnimatorCoordinator.feedNewTilt(
      tilt.toDouble(),
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    val animator = locationAnimatorCoordinator.animatorArray[ANIMATOR_TILT]
    assertEquals(tilt, animator.target)
    verify { animatorSetProvider.startAnimation(eq(listOf(animator)), any<LinearInterpolator>(), DEFAULT_TRACKING_TILT_ANIM_DURATION) }
  }

  @Test
  fun cancelAllAnimators() {
    locationAnimatorCoordinator.feedNewLocation(Location(""), cameraPosition, true)
    locationAnimatorCoordinator.cancelAllAnimations()

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG] == null)
  }

  @Test
  fun cancelZoomAnimators() {
    locationAnimatorCoordinator.feedNewZoomLevel(
      15.0,
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )
    locationAnimatorCoordinator.cancelZoomAnimation()

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_ZOOM] == null)
  }

  @Test
  fun cancelTiltAnimation() {
    locationAnimatorCoordinator.feedNewTilt(
      30.0,
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    locationAnimatorCoordinator.cancelTiltAnimation()

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_TILT] == null)
  }

  @Test
  fun resetAllCameraAnimations_empty() {
    locationAnimatorCoordinator.resetAllCameraAnimations(cameraPosition, false)
    assertTrue(locationAnimatorCoordinator.animatorArray.size() == 0)
  }

  @Test
  fun resetAllLayerAnimations_empty() {
    locationAnimatorCoordinator.resetAllLayerAnimations()
    assertTrue(locationAnimatorCoordinator.animatorArray.size() == 0)
  }

  @Test
  fun addNewListener() {
    val listener = Mockito.mock(AnimationsValueChangeListener::class.java)
    val holder = AnimatorListenerHolder(RenderMode.NORMAL, listener)
    val set = HashSet<AnimatorListenerHolder>().also {
      it.add(holder)
    }
    locationAnimatorCoordinator.updateAnimatorListenerHolders(set)

    assertTrue(locationAnimatorCoordinator.listeners.contains(listener))
  }

  @Test
  fun updateListeners() {
    val listener = Mockito.mock(AnimationsValueChangeListener::class.java)
    val holder = AnimatorListenerHolder(RenderMode.NORMAL, listener)
    val set = HashSet<AnimatorListenerHolder>().also {
      it.add(holder)
    }
    locationAnimatorCoordinator.updateAnimatorListenerHolders(set)

    val listener2 = Mockito.mock(AnimationsValueChangeListener::class.java)
    val holder2 = AnimatorListenerHolder(RenderMode.NORMAL, listener2)
    val listener3 = Mockito.mock(AnimationsValueChangeListener::class.java)
    val holder3 = AnimatorListenerHolder(CameraMode.TRACKING_GPS, listener3)
    val set2 = HashSet<AnimatorListenerHolder>().also {
      it.add(holder2)
      it.add(holder3)
    }
    locationAnimatorCoordinator.updateAnimatorListenerHolders(set2)

    assertTrue(locationAnimatorCoordinator.listeners.size() == 2)
    assertTrue(locationAnimatorCoordinator.listeners.contains(listener2))
    assertTrue(locationAnimatorCoordinator.listeners.contains(listener3))
  }

  @Test
  fun feedNewCompassBearing_withAnimation() {
    locationAnimatorCoordinator.setCompassAnimationEnabled(true)
    locationAnimatorCoordinator.feedNewCompassBearing(77f, cameraPosition)

    val animators = mutableListOf<Animator>(
      locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_COMPASS_BEARING],
      locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_COMPASS_BEARING])

    verify(exactly = 1) { animatorSetProvider.startAnimation(eq(animators), ofType(LinearInterpolator::class), eq(LocationComponentConstants.COMPASS_UPDATE_RATE_MS)) }
  }

  @Test
  fun feedNewCompassBearing_withoutAnimation() {
    locationAnimatorCoordinator.setCompassAnimationEnabled(false)
    locationAnimatorCoordinator.feedNewCompassBearing(77f, cameraPosition)

    val animators = mutableListOf<Animator>(
      locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_COMPASS_BEARING],
      locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_COMPASS_BEARING])

    verify(exactly = 1) { animatorSetProvider.startAnimation(eq(animators), ofType(LinearInterpolator::class), eq(0)) }
  }

  @Test
  fun feedNewAccuracy_withAnimation() {
    locationAnimatorCoordinator.setAccuracyAnimationEnabled(true)
    locationAnimatorCoordinator.feedNewAccuracyRadius(150f, false)

    val animators = mutableListOf<Animator>(
      locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY])

    verify(exactly = 1) { animatorSetProvider.startAnimation(eq(animators), ofType(LinearInterpolator::class), eq(LocationComponentConstants.ACCURACY_RADIUS_ANIMATION_DURATION)) }
  }

  @Test
  fun feedNewAccuracy_withoutAnimation() {
    locationAnimatorCoordinator.setAccuracyAnimationEnabled(false)
    locationAnimatorCoordinator.feedNewAccuracyRadius(150f, false)

    val animators = mutableListOf<Animator>(
      locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY])

    verify(exactly = 1) { animatorSetProvider.startAnimation(eq(animators), ofType(LinearInterpolator::class), eq(0)) }
  }

  @Test
  fun maxFps_setter() {
    locationAnimatorCoordinator.setMaxAnimationFps(5)
    assertEquals(5, locationAnimatorCoordinator.maxAnimationFps)
  }

  @Test
  fun maxFps_moreThanZeroRequired() {
    locationAnimatorCoordinator.setMaxAnimationFps(0)
    assertEquals(Int.MAX_VALUE, locationAnimatorCoordinator.maxAnimationFps)
    locationAnimatorCoordinator.setMaxAnimationFps(-1)
    assertEquals(Int.MAX_VALUE, locationAnimatorCoordinator.maxAnimationFps)
  }

  @Test
  fun maxFps_givenToAnimator() {
    locationAnimatorCoordinator.setMaxAnimationFps(5)
    locationAnimatorCoordinator.feedNewLocation(Location(""), cameraPosition, false)
    verify { animatorProvider.latLngAnimator(any(), any(), any(), 5) }
    verify { animatorProvider.floatAnimator(any(), any(), any(), 5) }
  }

  private fun getListenerHoldersSet(vararg animatorTypes: Int): Set<AnimatorListenerHolder> {
    return HashSet<AnimatorListenerHolder>().also {
      for (type in animatorTypes) {
        it.add(AnimatorListenerHolder(type, mockk()))
      }
    }
  }
}

private fun <E> SparseArray<E>.contains(listener: AnimationsValueChangeListener<*>?): Boolean {
  for (i in 0 until this.size()) {
    val element = this.get(this.keyAt(i))
    if (element == listener) {
      return true
    }
  }
  return false
}