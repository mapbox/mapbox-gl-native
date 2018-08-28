package com.mapbox.mapboxsdk.plugins.utils

import android.content.Context
import android.support.test.espresso.UiController
import android.support.test.espresso.ViewAction
import android.support.test.espresso.matcher.ViewMatchers.isDisplayed
import android.view.View
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.plugins.utils.PluginGenerationUtil.Companion.MAP_RENDER_DELAY
import org.hamcrest.Matcher

class GenericPluginAction<T>(private val mapView: MapView, private val mapboxMap: MapboxMap, private val pluginProvider: PluginProvider<T>,
                             private val onPerformGenericPluginAction: OnPerformGenericPluginAction<T>) : ViewAction {

  override fun getConstraints(): Matcher<View> {
    return isDisplayed()
  }

  override fun getDescription(): String {
    return javaClass.simpleName
  }

  override fun perform(uiController: UiController, view: View) {
    val plugin = pluginProvider.providePlugin(mapView, mapboxMap, view.context)

    // ensuring that the asynchronous renderer has time to render data we want to test
    uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
    while (!pluginProvider.isPluginDataReady(plugin, mapboxMap)) {
      uiController.loopMainThreadForAtLeast(MAP_RENDER_DELAY)
    }

    onPerformGenericPluginAction.onGenericPluginAction(
      plugin,
      mapboxMap,
      uiController,
      view.context)
  }

  interface OnPerformGenericPluginAction<in T> {
    fun onGenericPluginAction(plugin: T, mapboxMap: MapboxMap, uiController: UiController, context: Context)
  }

  interface PluginProvider<T> {
    fun providePlugin(mapView: MapView, mapboxMap: MapboxMap, context: Context): T
    fun isPluginDataReady(plugin: T, mapboxMap: MapboxMap): Boolean
  }
}