package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertNotSame;
import static org.mockito.Mockito.when;

public class IconTest {

  @Mock
  Bitmap bitmap;

  @Before
  public void beforeTest() {
    MockitoAnnotations.initMocks(this);
    when(bitmap.getConfig()).thenReturn(Bitmap.Config.ARGB_8888);
  }

  @Test
  public void testId() {
    String id = "test";
    Icon icon = IconFactory.recreate(id, Bitmap.createBitmap(0, 0, Bitmap.Config.ALPHA_8));
    assertEquals("id should match", id, icon.getId());
  }

  @Test
  public void testBitmap() {
    Icon icon = IconFactory.recreate("test", bitmap);
    assertEquals("bitmap should match", bitmap, icon.getBitmap());
  }

  @Test
  public void testEquals() {
    Icon icon1 = IconFactory.recreate("test", bitmap);
    Icon icon2 = IconFactory.recreate("test", bitmap);
    assertEquals("icons should not match", icon1, icon2);
  }

  @Test
  public void testEqualsObject() {
    Icon icon = IconFactory.recreate("test", Bitmap.createBitmap(0, 0, Bitmap.Config.ALPHA_8));
    assertNotSame("icon should not match", new Object(), icon);
  }

  @Test
  public void testHashcode() {
    Icon icon = IconFactory.recreate("test", bitmap);
    long expectedHashcode = 31 * bitmap.hashCode() + "test".hashCode();
    assertEquals("hashcode should match", expectedHashcode, icon.hashCode());
  }
}
