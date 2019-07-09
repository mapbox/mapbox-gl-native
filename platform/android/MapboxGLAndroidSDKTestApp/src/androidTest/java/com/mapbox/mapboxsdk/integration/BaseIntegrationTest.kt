package com.mapbox.mapboxsdk.integration

import android.content.Context
import android.content.Intent
import android.content.Intent.FLAG_ACTIVITY_NEW_TASK
import android.support.test.InstrumentationRegistry
import android.support.test.uiautomator.*
import com.mapbox.mapboxsdk.AppCenter
import org.junit.Before

const val TIMEOUT_UI_SEARCH_WAIT = 5000L

abstract class BaseIntegrationTest : AppCenter() {

  protected lateinit var device: UiDevice

  @Before
  open fun beforeTest() {
    device = UiDevice.getInstance(InstrumentationRegistry.getInstrumentation())
  }
}

/**
 * Launches an activity with FLAG_ACTIVITY_NEW_TASK.
 * <p>
 * To resume an activity, you need to add a single instance launchmode to your manifest configuration.
 * <p>
 */
fun UiDevice.launchActivity(context: Context, clazz: Class<*>) {
  val applicationPackage = InstrumentationRegistry.getTargetContext().packageName
  val intent = Intent(context, clazz)
  intent.addFlags(FLAG_ACTIVITY_NEW_TASK)
  InstrumentationRegistry.getContext().startActivity(intent)
  wait(Until.hasObject(By.pkg(applicationPackage).depth(0)), TIMEOUT_UI_SEARCH_WAIT)
}

fun UiDevice.scrollRecyclerViewTo(recycleItem: String) {
  val appView = UiScrollable(UiSelector().scrollable(true))
  appView.scrollIntoView(UiSelector().text(recycleItem))
}