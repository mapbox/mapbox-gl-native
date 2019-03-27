package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import android.support.test.uiautomator.By
import android.support.test.uiautomator.SearchCondition
import android.support.test.uiautomator.UiSelector
import android.support.test.uiautomator.Until
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.activity.fragment.FragmentBackStackActivity
import com.mapbox.mapboxsdk.testapp.activity.maplayout.SimpleMapActivity
import kotlinx.android.synthetic.main.activity_backstack_fragment.view.*
import org.junit.Before
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