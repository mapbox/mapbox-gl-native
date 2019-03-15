package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.runner.AndroidJUnit4
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class OrientationChangeTest : BaseIntegrationTest() {

  @Before
  override fun beforeTest() {
    super.beforeTest()
    openFeature("Simple Map")
  }

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
  }
}