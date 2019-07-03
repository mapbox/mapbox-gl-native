package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import android.support.test.uiautomator.UiSelector
import com.mapbox.mapboxsdk.testapp.activity.fragment.FragmentBackStackActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates MapFragment integration on the backstack
 */
@RunWith(AndroidJUnit4::class)
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