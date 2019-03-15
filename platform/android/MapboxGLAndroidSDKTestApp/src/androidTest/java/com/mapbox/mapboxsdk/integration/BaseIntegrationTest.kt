package com.mapbox.mapboxsdk.integration

import android.content.Intent
import android.support.test.InstrumentationRegistry
import android.support.test.uiautomator.*
import org.hamcrest.CoreMatchers
import org.hamcrest.MatcherAssert
import org.junit.Before

const val LAUNCH_TIMEOUT = 2500L
private const val TEST_APP_PACKAGE = "com.mapbox.mapboxsdk.testapp"

abstract class BaseIntegrationTest {

  protected lateinit var device: UiDevice

  @Before
  open fun beforeTest() {
    // Initialize UiDevice instance
    device = UiDevice.getInstance(InstrumentationRegistry.getInstrumentation())

    // Start from the home screen
    device.pressHome()

    // Wait for launcher
    val launcherPackage: String = device.launcherPackageName
    MatcherAssert.assertThat(launcherPackage, CoreMatchers.notNullValue())
    device.wait(Until.hasObject(By.pkg(launcherPackage).depth(0)), LAUNCH_TIMEOUT)

    // Launch the app
    val context = InstrumentationRegistry.getInstrumentation().context
    val intent = context.packageManager.getLaunchIntentForPackage(TEST_APP_PACKAGE).apply {
      // Clear out any previous instances
      addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK)
    }
    context.startActivity(intent)

    // Wait for the app to appear
    device.wait(
      Until.hasObject(By.pkg(TEST_APP_PACKAGE).depth(0)),
      LAUNCH_TIMEOUT
    )
  }

  fun openFeature(featureName: String) {
    scrollRecyclerViewTo(featureName)
    device.findObject(UiSelector().text(featureName)).clickAndWaitForNewWindow()
    device.waitForIdle(LAUNCH_TIMEOUT)
  }

  fun scrollRecyclerViewTo(recycleItem: String) {
    val appView = UiScrollable(UiSelector().scrollable(true))
    appView.scrollIntoView(UiSelector().text(recycleItem))
  }

  fun pressHomeReturnWithRecentApps(){
    device.pressRecentApps()
    device.findObject(UiSelector().text(InstrumentationRegistry.getTargetContext().getString(InstrumentationRegistry.getTargetContext().applicationInfo.labelRes))).click()
  }
}