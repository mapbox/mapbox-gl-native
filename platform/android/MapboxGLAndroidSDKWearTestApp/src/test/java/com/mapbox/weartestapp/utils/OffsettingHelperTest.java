package com.mapbox.weartestapp.utils;

import android.view.View;

import org.junit.Test;
import org.mockito.InjectMocks;

import static junit.framework.Assert.assertEquals;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class OffsettingHelperTest {

  private static final double DELTA = 1e-15;

  @InjectMocks
  View view = mock(View.class);

  @Test
  public void testAnchorOffset() {
    float[] offset = new float[2];
    int viewHeight = 50;
    when(view.getHeight()).thenReturn(viewHeight);
    OffsettingHelper offsettingHelper = new OffsettingHelper();
    offsettingHelper.adjustAnchorOffsetXY(view, offset);
    assertEquals("Offset of " + viewHeight + " should be divided by 2: ", viewHeight / 2, offset[0], DELTA);
  }
}
