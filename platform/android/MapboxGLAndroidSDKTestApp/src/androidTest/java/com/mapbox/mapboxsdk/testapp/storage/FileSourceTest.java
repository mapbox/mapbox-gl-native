package com.mapbox.mapboxsdk.testapp.storage;

import android.support.test.InstrumentationRegistry;
import android.support.test.annotation.UiThreadTest;
import android.support.test.runner.AndroidJUnit4;
import com.mapbox.mapboxsdk.storage.FileSource;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static junit.framework.TestCase.assertFalse;
import static junit.framework.TestCase.assertTrue;

@RunWith(AndroidJUnit4.class)
public class FileSourceTest {

  private FileSource fileSource;

  @Before
  @UiThreadTest
  public void setUp() {
    fileSource = FileSource.getInstance(InstrumentationRegistry.getContext());
  }

  @Test
  @UiThreadTest
  public void testDefault() {
    assertFalse("FileSource should not be active", fileSource.isActivated());
  }

  @Test
  @UiThreadTest
  public void testActivateDeactivate() {
    assertFalse("1) FileSource should not be active", fileSource.isActivated());
    fileSource.activate();
    assertTrue("2) FileSource should be active", fileSource.isActivated());
    fileSource.deactivate();
    assertFalse("3) FileSource should not be active", fileSource.isActivated());
  }
}