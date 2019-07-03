package com.mapbox.mapboxsdk

import android.app.Application
import android.content.Context
import android.support.test.runner.AndroidJUnitRunner

class InstrumentationRunner : AndroidJUnitRunner() {
  override fun newApplication(cl: ClassLoader?, className: String?, context: Context?): Application {
    return super.newApplication(cl, InstrumentationApplication::class.java.name, context)
  }
}