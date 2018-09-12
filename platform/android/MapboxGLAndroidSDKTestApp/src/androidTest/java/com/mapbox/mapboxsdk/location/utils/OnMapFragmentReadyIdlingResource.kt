package com.mapbox.mapboxsdk.location.utils

import android.os.Handler
import android.os.Looper
import android.support.test.espresso.IdlingResource

import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback
import com.mapbox.mapboxsdk.maps.SupportMapFragment

class OnMapFragmentReadyIdlingResource(fragment: SupportMapFragment?) : IdlingResource, OnMapReadyCallback {

  lateinit var mapboxMap: MapboxMap

  private var resourceCallback: IdlingResource.ResourceCallback? = null

  init {
    Handler(Looper.getMainLooper()).post {
      fragment?.getMapAsync(this)
    }
  }

  override fun getName(): String {
    return javaClass.simpleName
  }

  override fun isIdleNow(): Boolean {
    return this::mapboxMap.isInitialized
  }

  override fun registerIdleTransitionCallback(resourceCallback: IdlingResource.ResourceCallback) {
    this.resourceCallback = resourceCallback
  }

  override fun onMapReady(mapboxMap: MapboxMap) {
    this.mapboxMap = mapboxMap
    resourceCallback?.onTransitionToIdle()
  }
}