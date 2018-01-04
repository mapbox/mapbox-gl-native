package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.support.test.runner.AndroidJUnit4;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

/**
 * CRUD tests around Image
 */
@RunWith(AndroidJUnit4.class)
public class ImageTest extends BaseActivityTest {

  private static final String IMAGE_ID = "test.image";

  @Override
  protected Class getActivityClass() {
    return RuntimeStyleTestActivity.class;
  }

  @Test
  public void testAddGetImage() {
    validateTestSetup();
    MapboxMapAction.invoke(mapboxMap, (uiController, mapboxMap) -> {
      Drawable drawable = rule.getActivity().getResources().getDrawable(R.drawable.ic_launcher_round);
      assertTrue(drawable instanceof BitmapDrawable);

      Bitmap bitmapSet = ((BitmapDrawable) drawable).getBitmap();
      mapboxMap.addImage(IMAGE_ID, bitmapSet);

      Bitmap bitmapGet = mapboxMap.getImage(IMAGE_ID);
      assertTrue(bitmapGet.sameAs(bitmapSet));

      mapboxMap.removeImage(IMAGE_ID);
      assertNull(mapboxMap.getImage(IMAGE_ID));
    });
  }
}
