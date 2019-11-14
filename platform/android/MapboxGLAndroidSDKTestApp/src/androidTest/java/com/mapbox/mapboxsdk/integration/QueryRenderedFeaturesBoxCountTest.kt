package com.mapbox.mapboxsdk.integration

import androidx.test.filters.LargeTest
import androidx.test.rule.ActivityTestRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.mapbox.mapboxsdk.testapp.activity.feature.QueryRenderedFeaturesBoxCountActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates reopening an Activity and querying the map before surface recreation #14394
 */
@RunWith(androidx.test.ext.junit.runners.AndroidJUnit4::class)
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