package com.mapbox.mapboxsdk.testapp.feature;

import android.graphics.PointF;
import android.support.test.espresso.UiController;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.style.CircleLayerActivity;

import org.junit.Test;

import timber.log.Timber;

public class QueryRenderedFeaturesExecutionTimeTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return CircleLayerActivity.class;
  }

  @Test
  public void testQueryRenderedFeaturesExecutionTime() {
    validateTestSetup();
    MapboxMapAction.invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        PointF point = new PointF(519.4336f, 1086.6211f);

        long startTime = System.currentTimeMillis();
        mapboxMap.queryRenderedFeatures(point, true);
        long stopTime = System.currentTimeMillis();
        long elapsedTime = stopTime - startTime;
        Timber.e("Execution time with testQueryRenderedFeaturesExecutionTime: %s", elapsedTime);

        startTime = System.currentTimeMillis();
        mapboxMap.queryRenderedFeatures(point, false);
        stopTime = System.currentTimeMillis();
        elapsedTime = stopTime - startTime;
        Timber.e("Execution time with testQueryRenderedFeaturesWithoutGeometryExecutionTime: %s", elapsedTime);
      }
    });
  }
}
