package com.mapbox.mapboxsdk

import com.microsoft.appcenter.espresso.Factory
import org.junit.After
import org.junit.Rule

abstract class AppCenter {
    @get:Rule
    var reportHelper = Factory.getReportHelper()!!

    @After
    open fun afterTest() {
        reportHelper.label(javaClass.simpleName)
    }
}