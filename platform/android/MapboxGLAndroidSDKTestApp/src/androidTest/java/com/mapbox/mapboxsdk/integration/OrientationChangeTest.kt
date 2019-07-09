package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.testapp.activity.maplayout.SimpleMapActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
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