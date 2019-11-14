package com.mapbox.mapboxsdk.integration

import androidx.test.filters.LargeTest
import androidx.test.rule.ActivityTestRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.uiautomator.UiSelector
import com.mapbox.mapboxsdk.testapp.activity.fragment.FragmentBackStackActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates MapFragment integration on the backstack
 */
@RunWith(androidx.test.ext.junit.runners.AndroidJUnit4::class)
class FragmentBackStackTest : BaseIntegrationTest() {

  @get:Rule
  var activityRule: ActivityTestRule<FragmentBackStackActivity> = ActivityTestRule(FragmentBackStackActivity::class.java)

  @Test
  @LargeTest
  fun backPressedOnBackStackResumed() {
    device.waitForIdle()
    clickReplaceFragmentButton()
    device.pressHome()
    device.waitForIdle()
    device.launchActivity(activityRule.activity.applicationContext, FragmentBackStackActivity::class.java)
    backPressBackStack()
    device.waitForIdle()
  }

  private fun clickReplaceFragmentButton() {
    device.findObject(UiSelector().description(textDescription)).click()
  }

  private fun backPressBackStack() {
    device.pressBack() // pops fragment, showing map
    device.pressBack() // finish activity
  }

  private companion object {
    const val textDescription = "btn_change_fragment"
  }
}