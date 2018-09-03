package com.mapbox.mapboxsdk.plugins.utils

import android.content.Context
import android.support.test.espresso.UiController
import android.support.test.espresso.ViewAction
import android.support.test.espresso.matcher.ViewMatchers.isDisplayed
import android.view.View
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerPlugin
import org.hamcrest.Matcher

class LocationLayerPluginAction(private val mapboxMap: MapboxMap,
                                private val onPerformLocationLayerPluginAction: OnPerformLocationLayerPluginAction) : ViewAction {

  override fun getConstraints(): Matcher<View> {
    return isDisplayed()
  }

  override fun getDescription(): String {
    return javaClass.simpleName
  }

  override fun perform(uiController: UiController, view: View) {
    val plugin = mapboxMap.locationLayerPlugin

    while (mapboxMap.getSource("mapbox-location-source") == null) {
      uiController.loopMainThreadForAtLeast(MapboxTestingUtils.MAP_RENDER_DELAY)
    }

    onPerformLocationLayerPluginAction.onLocationLayerPluginAction(
      plugin,
      mapboxMap,
      uiController,
      view.context)
  }

  interface OnPerformLocationLayerPluginAction {
    fun onLocationLayerPluginAction(plugin: LocationLayerPlugin, mapboxMap: MapboxMap, uiController: UiController, context: Context)
  }
}