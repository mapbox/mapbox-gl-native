package com.mapbox.mapboxsdk.integration

import androidx.test.filters.LargeTest
import androidx.test.rule.ActivityTestRule
import androidx.test.runner.AndroidJUnit4
import androidx.test.uiautomator.By
import androidx.test.uiautomator.SearchCondition
import androidx.test.uiautomator.UiSelector
import androidx.test.uiautomator.Until
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