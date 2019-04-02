package com.mapbox.mapboxsdk

import com.mapbox.mapboxsdk.testapp.MapboxApplication

class InstrumentationApplication : MapboxApplication() {
  override fun initializeLeakCanary(): Boolean {
    // do not initialize leak canary during instrumentation tests
    return true
  }
}