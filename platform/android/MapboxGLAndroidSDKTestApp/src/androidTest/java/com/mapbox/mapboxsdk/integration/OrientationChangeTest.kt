package com.mapbox.mapboxsdk.integration

import androidx.test.filters.LargeTest
import androidx.test.rule.ActivityTestRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.mapbox.mapboxsdk.testapp.activity.maplayout.SimpleMapActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(androidx.test.ext.junit.runners.AndroidJUnit4::class)
class OrientationChangeTest : BaseIntegrationTest() {

  @get:Rule
  var activityRule: ActivityTestRule<SimpleMapActivity> = ActivityTestRule(SimpleMapActivity::class.java)

  @Test
  @LargeTest
  fun rotateSimpleMap() {
    device.setOrientationLeft()
    device.waitForIdle()
    device.setOrientationNatural()
    device.waitForIdle()
    device.setOrientationRight()
    device.waitForIdle()
    device.setOrientationNatural()
    device.setOrientationLeft()
    device.setOrientationNatural()
    device.setOrientationRight()
    device.setOrientationNatural()
  }
}