package com.mapbox.mapboxsdk.snapshotter

import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.testapp.activity.snapshot.MapSnapshotterMarkerActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Integration test that validates if a snapshot is created with MapSnapshotterMarkerActivity
 */
@RunWith(AndroidJUnit4::class)
class MapSnapshotterTest {

  @Rule
  @JvmField
  var rule = ActivityTestRule(MapSnapshotterMarkerActivity::class.java)

  @Test(timeout = 10000)
  fun mapSnapshotter() {
    while (rule.activity.mapSnapshot == null) {
      Thread.sleep(250)
    }
  }
}