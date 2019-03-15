package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.runner.AndroidJUnit4
import android.support.test.uiautomator.UiSelector
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates MapFragment integration with a ViewPager
 */
@RunWith(AndroidJUnit4::class)
class ViewPagerScrollTest : BaseIntegrationTest() {

  @Before
  override fun beforeTest() {
    super.beforeTest()
    openFeature("ViewPager")
  }

  @Test
  @LargeTest
  fun scrollViewPager() {
    for (i in 1..5) {
      clickTab(i)
    }

    for (i in 4 downTo 0) {
      clickTab(i)
    }
  }

  private fun clickTab(index: Int) {
    device.findObject(UiSelector().text("Page $index")).click()
  }
}