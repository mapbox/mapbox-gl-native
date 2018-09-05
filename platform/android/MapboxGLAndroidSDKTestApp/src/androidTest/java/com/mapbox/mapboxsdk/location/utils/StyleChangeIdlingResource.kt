package com.mapbox.mapboxsdk.location.utils

import android.support.test.espresso.IdlingResource
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.maps.MapboxMap

/**
 * Resource, that's idling until the provided style is loaded.
 * Remember to add any espresso action (like view assertion) after the [waitForStyle] call
 * for the test to keep running.
 */
class StyleChangeIdlingResource : IdlingResource {

  private var callback: IdlingResource.ResourceCallback? = null
  private var isIdle = true

  override fun getName(): String {
    return javaClass.simpleName
  }

  override fun isIdleNow(): Boolean {
    return isIdle
  }

  override fun registerIdleTransitionCallback(callback: IdlingResource.ResourceCallback?) {
    this.callback = callback
  }

  private fun setIdle() {
    isIdle = true
    callback?.onTransitionToIdle()
  }

  fun waitForStyle(mapView: MapView, mapboxMap: MapboxMap, styleUrl: String) {
    isIdle = false
    mapView.addOnMapChangedListener(object : MapView.OnMapChangedListener {
      override fun onMapChanged(change: Int) {
        if (change == MapView.DID_FINISH_LOADING_STYLE) {
          mapView.removeOnMapChangedListener(this)
          setIdle()
        }
      }
    })
    mapboxMap.setStyleUrl(styleUrl)
  }
}