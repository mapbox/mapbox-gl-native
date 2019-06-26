package com.mapbox.mapboxsdk.testapp.fragment

import android.support.test.espresso.Espresso
import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.action.ViewActions.click
import android.support.test.espresso.matcher.ViewMatchers.withId
import android.support.test.filters.LargeTest
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.AppCenter
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.action.WaitAction
import com.mapbox.mapboxsdk.testapp.activity.maplayout.MapInDialogActivity
import org.junit.Ignore
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Regression test that validates that a map inside a DialogFragment can be opened and closed.
 */
@RunWith(AndroidJUnit4::class)
@LargeTest
class MapDialogFragmentTest : AppCenter() {

    @get:Rule
    var activityRule: ActivityTestRule<MapInDialogActivity> = ActivityTestRule(MapInDialogActivity::class.java)

    @Test
    @Ignore
    fun openCloseDialog() {
        onView(withId(R.id.button_open_dialog)).perform(click())
        Thread.sleep(2500)
        Espresso.pressBack()
    }
}