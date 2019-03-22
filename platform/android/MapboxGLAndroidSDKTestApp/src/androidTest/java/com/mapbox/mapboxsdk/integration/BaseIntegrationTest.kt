package com.mapbox.mapboxsdk.integration

import android.content.Context
import android.content.Intent
import android.content.Intent.FLAG_ACTIVITY_SINGLE_TOP
import android.support.test.InstrumentationRegistry
import android.support.test.uiautomator.*
import org.junit.Before

abstract class BaseIntegrationTest {

  protected lateinit var device: UiDevice

  @Before
  open fun beforeTest() {
    device = UiDevice.getInstance(InstrumentationRegistry.getInstrumentation())
  }
}

fun UiDevice.launchActivity(context: Context, clazz: Class<*>) {
  val applicationPackage = InstrumentationRegistry.getTargetContext().packageName
  val intent = Intent(context, clazz)
  intent.addFlags(FLAG_ACTIVITY_SINGLE_TOP)
  InstrumentationRegistry.getContext().startActivity(intent)
  wait(Until.hasObject(By.pkg(applicationPackage).depth(0)), 5000)
}

fun UiDevice.scrollRecyclerViewTo(recycleItem: String) {
  val appView = UiScrollable(UiSelector().scrollable(true))
  appView.scrollIntoView(UiSelector().text(recycleItem))
}