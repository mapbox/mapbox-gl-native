package com.mapbox.mapboxsdk.plugins.utils

import android.arch.lifecycle.Lifecycle
import android.arch.lifecycle.LifecycleOwner
import android.arch.lifecycle.LifecycleRegistry

class TestLifecycleOwner : LifecycleOwner {
  private val lifecycleRegistry = LifecycleRegistry(this)

  override fun getLifecycle() = lifecycleRegistry

  fun markState(state: Lifecycle.State) {
    lifecycleRegistry.markState(state)
  }

  fun handleLifecycleEvent(event: Lifecycle.Event) {
    lifecycleRegistry.handleLifecycleEvent(event)
  }
}