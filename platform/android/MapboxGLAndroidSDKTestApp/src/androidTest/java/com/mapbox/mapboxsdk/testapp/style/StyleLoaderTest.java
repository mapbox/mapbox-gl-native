package com.mapbox.mapboxsdk.testapp.style;


import android.support.test.espresso.UiController;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;

import org.junit.Test;

import java.io.IOException;

import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.assertEquals;

/**
 * Tests around style loading
 */
public class StyleLoaderTest extends BaseActivityTest {


  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  public void testSetGetStyleJsonString() throws Exception {
    validateTestSetup();
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        try {
          String expected = ResourceUtils.readRawResource(rule.getActivity(), R.raw.local_style);
          mapboxMap.setStyleJson(expected);
          String actual = mapboxMap.getStyleJson();
          assertEquals("Style json should match", expected, actual);
        } catch (IOException exception) {
          exception.printStackTrace();
        }
      }
    });
  }

  @Test
  public void testDefaultStyleLoadWithActivityLifecycleChange() throws Exception {
    validateTestSetup();
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        try {
          String expected = ResourceUtils.readRawResource(rule.getActivity(), R.raw.local_style);
          mapboxMap.setStyleJson(expected);

          // fake activity stop/start
          MapView mapView = (MapView) rule.getActivity().findViewById(R.id.mapView);
          mapView.onPause();
          mapView.onStop();

          mapView.onStart();
          mapView.onResume();

          String actual = mapboxMap.getStyleJson();
          assertEquals("Style URL should be empty", "", mapboxMap.getStyleUrl());
          assertEquals("Style json should match", expected, actual);
        } catch (IOException exception) {
          exception.printStackTrace();
        }
      }
    });
  }
}