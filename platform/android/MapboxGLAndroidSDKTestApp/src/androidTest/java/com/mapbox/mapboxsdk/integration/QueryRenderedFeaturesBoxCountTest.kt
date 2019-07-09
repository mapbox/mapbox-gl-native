package com.mapbox.mapboxsdk.integration

import android.support.test.filters.LargeTest
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.testapp.activity.feature.QueryRenderedFeaturesBoxCountActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates reopening an Activity and querying the map before surface recreation #14394
 */
@RunWith(AndroidJUnit4::class)
class QueryRenderedFeaturesBoxCountTest : BaseIntegrationTest() {

  @get:Rule
  var activityRule: ActivityTestRule<QueryRenderedFeaturesBoxCountActivity> =
    ActivityTestRule(QueryRenderedFeaturesBoxCountActivity::class.java)

  @Test
  @LargeTest
  fun reopenQueryRendererFeaturesActivity() {
    device.waitForIdle()
    device.pressHome()
    device.waitForIdle()
    device.launchActivity(activityRule.activity, QueryRenderedFeaturesBoxCountActivity::class.java)
    device.waitForIdle()
  }
}