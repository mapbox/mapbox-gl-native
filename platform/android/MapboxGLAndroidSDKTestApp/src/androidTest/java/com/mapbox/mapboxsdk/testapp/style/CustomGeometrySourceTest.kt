package com.mapbox.mapboxsdk.testapp.style

import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.matcher.ViewMatchers.isRoot
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource.THREAD_POOL_LIMIT
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource.THREAD_PREFIX
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke
import com.mapbox.mapboxsdk.testapp.action.OrientationChangeAction.orientationLandscape
import com.mapbox.mapboxsdk.testapp.action.OrientationChangeAction.orientationPortrait
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest
import com.mapbox.mapboxsdk.testapp.activity.style.GridSourceActivity
import com.mapbox.mapboxsdk.testapp.activity.style.GridSourceActivity.ID_GRID_LAYER
import com.mapbox.mapboxsdk.testapp.activity.style.GridSourceActivity.ID_GRID_SOURCE
import org.junit.Assert
import org.junit.Ignore
import org.junit.Test

class CustomGeometrySourceTest : BaseActivityTest() {

  override fun getActivityClass(): Class<*> = GridSourceActivity::class.java

  @Test
  fun sourceNotLeakingThreadsTest() {
    validateTestSetup()
    waitAction(4000)
    onView(isRoot()).perform(orientationLandscape())
    waitAction(2000)
    onView(isRoot()).perform(orientationPortrait())
    waitAction(2000)
    Assert.assertFalse("Threads should be shutdown when the source is destroyed.",
      Thread.getAllStackTraces().keys.filter {
        it.name.startsWith(THREAD_PREFIX)
      }.count() > THREAD_POOL_LIMIT)
  }

  @Test
  @Ignore
  fun threadsShutdownWhenSourceRemovedTest() {
    validateTestSetup()
    invoke(mapboxMap) { uiController, mapboxMap ->
      mapboxMap.removeLayer(ID_GRID_LAYER)
      uiController.loopMainThreadForAtLeast(3000)
      mapboxMap.removeSource(ID_GRID_SOURCE)
      uiController.loopMainThreadForAtLeast(1000)
      Assert.assertTrue("There should be no threads running when the source is removed.",
        Thread.getAllStackTraces().keys.filter {
          it.name.startsWith(CustomGeometrySource.THREAD_PREFIX)
        }.count() == 0)
    }
  }

  @Test
  @Ignore
  fun threadsRestartedWhenSourceReAddedTest() {
    validateTestSetup()
    invoke(mapboxMap) { uiController, mapboxMap ->
      mapboxMap.removeLayer((rule.activity as GridSourceActivity).layer)
      uiController.loopMainThreadForAtLeast(3000)
      mapboxMap.removeSource(ID_GRID_SOURCE)
      uiController.loopMainThreadForAtLeast(1000)
      mapboxMap.addSource((rule.activity as GridSourceActivity).source)
      mapboxMap.addLayer((rule.activity as GridSourceActivity).layer)
      uiController.loopMainThreadForAtLeast(1000)
      Assert.assertTrue("Threads should be restarted when the source is re-added to the map.",
        Thread.getAllStackTraces().keys.filter {
          it.name.startsWith(CustomGeometrySource.THREAD_PREFIX)
        }.count() == CustomGeometrySource.THREAD_POOL_LIMIT)
    }
  }
}