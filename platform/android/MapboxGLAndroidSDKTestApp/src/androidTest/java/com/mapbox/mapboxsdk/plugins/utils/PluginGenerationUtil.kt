package com.mapbox.mapboxsdk.plugins.utils

import android.content.Context
import android.support.v7.app.AppCompatActivity
import com.mapbox.android.core.location.LocationEngine
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerOptions
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerPlugin

class PluginGenerationUtil {
  companion object {
    fun getLocationLayerPluginProvider(activity: AppCompatActivity,
                                       useDefaultEngine: Boolean = false,
                                       engine: LocationEngine? = null,
                                       options: LocationLayerOptions? = null,
                                       registerLifecycleObserver: Boolean = true)
      : GenericPluginAction.PluginProvider<LocationLayerPlugin> {
      return object : GenericPluginAction.PluginProvider<LocationLayerPlugin> {
        override fun providePlugin(mapView: MapView, mapboxMap: MapboxMap, context: Context): LocationLayerPlugin {
          val plugin = if (useDefaultEngine) {
              if (options != null) {
                  LocationLayerPlugin(mapView, mapboxMap, options)
              } else {
                  LocationLayerPlugin(mapView, mapboxMap)
              }
          } else {
              if (options != null) {
                  LocationLayerPlugin(mapView, mapboxMap, engine, options)
              } else {
                  LocationLayerPlugin(mapView, mapboxMap, engine)
              }
          }

          if (registerLifecycleObserver) {
            activity.lifecycle.addObserver(plugin)
          }

          return plugin
        }

        override fun isPluginDataReady(plugin: LocationLayerPlugin, mapboxMap: MapboxMap): Boolean {
          return mapboxMap.getSource("mapbox-location-source") != null
        }
      }
    }

    const val MAP_RENDER_DELAY = 250L
    const val MAP_CONNECTION_DELAY = 1000L
  }
}