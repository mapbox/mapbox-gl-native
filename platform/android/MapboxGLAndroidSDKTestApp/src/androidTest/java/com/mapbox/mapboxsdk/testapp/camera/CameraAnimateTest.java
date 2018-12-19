package com.mapbox.mapboxsdk.testapp.camera;

import android.support.test.espresso.Espresso;
import android.support.test.espresso.IdlingRegistry;
import android.support.test.espresso.idling.CountingIdlingResource;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.DeviceIndependentTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;

import org.junit.Test;

import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.assertEquals;

public class CameraAnimateTest extends BaseActivityTest {

  private final CountingIdlingResource animationIdlingResource =
    new CountingIdlingResource("animation_idling_resource");

  @Override
  protected Class getActivityClass() {
    return DeviceIndependentTestActivity.class;
  }

  @Override
  public void beforeTest() {
    super.beforeTest();
    IdlingRegistry.getInstance().register(animationIdlingResource);
  }

  @Test
  public void testAnimateToCameraPositionTarget() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      float zoom = 1.0f;
      LatLng moveTarget = new LatLng(1, 1);
      CameraPosition initialPosition = new CameraPosition.Builder().target(
        new LatLng()).zoom(zoom).bearing(0).tilt(0).build();
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Default camera position should match default", cameraPosition, initialPosition);

      animationIdlingResource.increment();
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLng(moveTarget), new MapboxMap.CancelableCallback() {
        @Override
        public void onCancel() {
          verifyCameraPosition(mapboxMap, moveTarget, zoom, 0, 0);
          animationIdlingResource.decrement();
        }

        @Override
        public void onFinish() {
          verifyCameraPosition(mapboxMap, moveTarget, zoom, 0, 0);
          animationIdlingResource.decrement();
        }
      });
    });

    Espresso.onIdle();
  }

  @Test
  public void testAnimateToCameraPositionTargetZoom() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final float moveZoom = 15.5f;
      final LatLng moveTarget = new LatLng(1.0000000001, 1.0000000003);

      animationIdlingResource.increment();
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(moveTarget, moveZoom), new MapboxMap
        .CancelableCallback() {
        @Override
        public void onCancel() {
          verifyCameraPosition(mapboxMap, moveTarget, moveZoom, 0, 0);
          animationIdlingResource.decrement();
        }

        @Override
        public void onFinish() {
          verifyCameraPosition(mapboxMap, moveTarget, moveZoom, 0, 0);
          animationIdlingResource.decrement();
        }
      });
    });

    Espresso.onIdle();
  }

  @Test
  public void testAnimateToCameraPosition() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final LatLng moveTarget = new LatLng(1.0000000001, 1.0000000003);
      final float moveZoom = 15.5f;
      final float moveTilt = 45.5f;
      final float moveBearing = 12.5f;

      animationIdlingResource.increment();
      mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(
        new CameraPosition.Builder()
          .target(moveTarget)
          .zoom(moveZoom)
          .tilt(moveTilt)
          .bearing(moveBearing)
          .build()),
        new MapboxMap.CancelableCallback() {
          @Override
          public void onCancel() {
            verifyCameraPosition(mapboxMap, moveTarget, moveZoom, moveBearing, moveTilt);
            animationIdlingResource.decrement();
          }

          @Override
          public void onFinish() {
            verifyCameraPosition(mapboxMap, moveTarget, moveZoom, moveBearing, moveTilt);
            animationIdlingResource.decrement();
          }
        });
    });

    Espresso.onIdle();
  }

  @Test
  public void testAnimateToBounds() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final LatLng centerBounds = new LatLng(1, 1);
      LatLng cornerOne = new LatLng();
      LatLng cornerTwo = new LatLng(2, 2);
      final LatLngBounds.Builder builder = new LatLngBounds.Builder();
      builder.include(cornerOne);
      builder.include(cornerTwo);

      animationIdlingResource.increment();
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngBounds(builder.build(), 0), new MapboxMap
        .CancelableCallback() {
        @Override
        public void onCancel() {
          verifyCameraPosition(mapboxMap, centerBounds, mapboxMap.getCameraPosition().zoom, 0, 0);
          animationIdlingResource.decrement();
        }

        @Override
        public void onFinish() {
          verifyCameraPosition(mapboxMap, centerBounds, mapboxMap.getCameraPosition().zoom, 0, 0);
          animationIdlingResource.decrement();
        }
      });
    });

    Espresso.onIdle();
  }

  @Test
  public void testAnimateToZoomIn() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      float zoom = 1.0f;

      animationIdlingResource.increment();
      mapboxMap.animateCamera(CameraUpdateFactory.zoomIn(), new MapboxMap.CancelableCallback() {
        @Override
        public void onCancel() {
          verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoom + 1, 0, 0);
          animationIdlingResource.decrement();
        }

        @Override
        public void onFinish() {
          verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoom + 1, 0, 0);
          animationIdlingResource.decrement();
        }
      });
    });

    Espresso.onIdle();
  }

  @Test
  public void testAnimateToZoomOut() {
    validateTestSetup();
    float zoom = 10.0f;
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      animationIdlingResource.increment();
      mapboxMap.animateCamera(
        CameraUpdateFactory.newLatLngZoom(new LatLng(), zoom),
        new MapboxMap.CancelableCallback() {
          @Override
          public void onCancel() {
            verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoom, 0, 0);
            animationIdlingResource.decrement();
          }

          @Override
          public void onFinish() {
            verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoom, 0, 0);
            animationIdlingResource.decrement();
          }
        });
    });

    invoke(mapboxMap, ((uiController, mapboxMap1) -> {
      animationIdlingResource.increment();
      mapboxMap.animateCamera(CameraUpdateFactory.zoomOut(), new MapboxMap.CancelableCallback() {
        @Override
        public void onCancel() {
          verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoom - 1, 0, 0);
          animationIdlingResource.decrement();
        }

        @Override
        public void onFinish() {
          verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoom - 1, 0, 0);
          animationIdlingResource.decrement();
        }
      });
    }));

    Espresso.onIdle();
  }

  @Test
  public void testAnimateToZoomBy() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      float zoom = 1.0f;
      final float zoomBy = 2.45f;

      animationIdlingResource.increment();
      mapboxMap.animateCamera(CameraUpdateFactory.zoomBy(zoomBy), new MapboxMap.CancelableCallback() {
        @Override
        public void onCancel() {
          verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoom + zoomBy, 0, 0);
          animationIdlingResource.decrement();
        }

        @Override
        public void onFinish() {
          verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoom + zoomBy, 0, 0);
          animationIdlingResource.decrement();
        }
      });
    });

    Espresso.onIdle();
  }

  @Test
  public void testAnimateToZoomTo() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final float zoomTo = 2.45f;

      animationIdlingResource.increment();
      mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(zoomTo), new MapboxMap.CancelableCallback() {
        @Override
        public void onCancel() {
          verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoomTo, 0, 0);
          animationIdlingResource.decrement();
        }

        @Override
        public void onFinish() {
          verifyCameraPosition(mapboxMap, mapboxMap.getCameraPosition().target, zoomTo, 0, 0);
          animationIdlingResource.decrement();
        }
      });
    });

    Espresso.onIdle();
  }

  @Override
  public void afterTest() {
    super.afterTest();
    IdlingRegistry.getInstance().unregister(animationIdlingResource);
  }

  private void verifyCameraPosition(MapboxMap mapboxMap, LatLng moveTarget, double moveZoom, double moveBearing,
                                    double moveTilt) {
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
      moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA);
    assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
      moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA);
    assertEquals("Moved zoom should match", cameraPosition.zoom, moveZoom, TestConstants.ZOOM_DELTA);
    assertEquals("Moved zoom should match", cameraPosition.tilt, moveTilt, TestConstants.TILT_DELTA);
    assertEquals("Moved bearing should match", cameraPosition.bearing, moveBearing, TestConstants.BEARING_DELTA);
  }
}

