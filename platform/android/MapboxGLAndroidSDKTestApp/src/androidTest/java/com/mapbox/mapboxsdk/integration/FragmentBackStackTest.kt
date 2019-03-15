package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.runner.AndroidJUnit4
import android.support.test.uiautomator.UiSelector
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates MapFragment integration on the backstack
 */
@RunWith(AndroidJUnit4::class)
class FragmentBackStackTest : BaseIntegrationTest() {

  @Before
  override fun beforeTest() {
    super.beforeTest()
    openFeature("Backstack Map Fragment")
  }

  @Test
  @LargeTest
  fun backPressedOnBackStackResumed(){
    device.waitForIdle()
    clickReplaceFragmentButton()
    device.waitForIdle()
    pressHomeReturnWithRecentApps()
    device.waitForIdle()
    backPressBackStack()
  }

  private fun clickReplaceFragmentButton(){
    device.findObject(UiSelector().text("REPLACE WITH EMPTY FRAGMENT")).click()
  }

  private fun backPressBackStack(){
    device.pressBack() // pops fragment, showing map
    device.pressBack() // finish activity
  }
}