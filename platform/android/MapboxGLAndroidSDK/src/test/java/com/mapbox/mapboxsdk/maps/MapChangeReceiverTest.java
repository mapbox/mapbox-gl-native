package com.mapbox.mapboxsdk.maps;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static org.mockito.Mockito.verify;

/**
 * Tests integration of MapChangeDispatcher and see if events are correctly forwarded.
 */
public class MapChangeReceiverTest {

  private static final String TEST_STRING = "mapChangeRandom";

  private MapChangeReceiver mapChangeEventManager;

  @Mock
  private MapView.OnCameraWillChangeListener onCameraWillChangeListener;

  @Mock
  private MapView.OnCameraDidChangeListener onCameraDidChangeListener;

  @Mock
  private MapView.OnCameraIsChangingListener onCameraIsChangingListener;

  @Mock
  private MapView.OnWillStartLoadingMapListener onWillStartLoadingMapListener;

  @Mock
  private MapView.OnDidFinishLoadingMapListener onDidFinishLoadingMapListener;

  @Mock
  private MapView.OnDidFailLoadingMapListener onDidFailLoadingMapListener;

  @Mock
  private MapView.OnWillStartRenderingFrameListener onWillStartRenderingFrameListener;

  @Mock
  private MapView.OnDidFinishRenderingFrameListener onDidFinishRenderingFrameListener;

  @Mock
  private MapView.OnWillStartRenderingMapListener onWillStartRenderingMapListener;

  @Mock
  private MapView.OnDidFinishRenderingMapListener onDidFinishRenderingMapListener;

  @Mock
  private MapView.OnDidFinishLoadingStyleListener onDidFinishLoadingStyleListener;

  @Mock
  private MapView.OnSourceChangedListener onSourceChangedListener;

  @Before
  public void beforeTest() {
    MockitoAnnotations.initMocks(this);
    mapChangeEventManager = new MapChangeReceiver();
  }

  @Test
  public void testOnCameraRegionWillChangeListener() {
    mapChangeEventManager.addOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(false);
    verify(onCameraWillChangeListener).onCameraWillChange(false);
    mapChangeEventManager.removeOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(false);
    verify(onCameraWillChangeListener).onCameraWillChange(false);
  }

  @Test
  public void testOnCameraRegionWillChangeAnimatedListener() {
    mapChangeEventManager.addOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(true);
    verify(onCameraWillChangeListener).onCameraWillChange(true);
    mapChangeEventManager.removeOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(true);
    verify(onCameraWillChangeListener).onCameraWillChange(true);
  }

  @Test
  public void testOnCameraIsChangingListener() {
    mapChangeEventManager.addOnCameraIsChangingListener(onCameraIsChangingListener);
    mapChangeEventManager.onCameraIsChanging();
    verify(onCameraIsChangingListener).onCameraIsChanging();
    mapChangeEventManager.removeOnCameraIsChangingListener(onCameraIsChangingListener);
    mapChangeEventManager.onCameraIsChanging();
    verify(onCameraIsChangingListener).onCameraIsChanging();
  }

  @Test
  public void testOnCameraRegionDidChangeListener() {
    mapChangeEventManager.addOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(false);
    verify(onCameraDidChangeListener).onCameraDidChange(false);
    mapChangeEventManager.removeOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(false);
    verify(onCameraDidChangeListener).onCameraDidChange(false);
  }

  @Test
  public void testOnCameraRegionDidChangeAnimatedListener() {
    mapChangeEventManager.addOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(true);
    verify(onCameraDidChangeListener).onCameraDidChange(true);
    mapChangeEventManager.removeOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(true);
    verify(onCameraDidChangeListener).onCameraDidChange(true);
  }

  @Test
  public void testOnWillStartLoadingMapListener() {
    mapChangeEventManager.addOnWillStartLoadingMapListener(onWillStartLoadingMapListener);
    mapChangeEventManager.onWillStartLoadingMap();
    verify(onWillStartLoadingMapListener).onWillStartLoadingMap();
    mapChangeEventManager.removeOnWillStartLoadingMapListener(onWillStartLoadingMapListener);
    mapChangeEventManager.onWillStartLoadingMap();
    verify(onWillStartLoadingMapListener).onWillStartLoadingMap();
  }

  @Test
  public void testOnDidFinishLoadingMapListener() {
    mapChangeEventManager.addOnDidFinishLoadingMapListener(onDidFinishLoadingMapListener);
    mapChangeEventManager.onDidFinishLoadingMap();
    verify(onDidFinishLoadingMapListener).onDidFinishLoadingMap();
    mapChangeEventManager.removeOnDidFinishLoadingMapListener(onDidFinishLoadingMapListener);
    mapChangeEventManager.onDidFinishLoadingMap();
    verify(onDidFinishLoadingMapListener).onDidFinishLoadingMap();
  }

  @Test
  public void testOnDidFailLoadingMapListener() {
    mapChangeEventManager.addOnDidFailLoadingMapListener(onDidFailLoadingMapListener);
    mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
    verify(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);
    mapChangeEventManager.removeOnDidFailLoadingMapListener(onDidFailLoadingMapListener);
    mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
    verify(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);
  }

  @Test
  public void testOnWillStartRenderingFrameListener() {
    mapChangeEventManager.addOnWillStartRenderingFrameListener(onWillStartRenderingFrameListener);
    mapChangeEventManager.onWillStartRenderingFrame();
    verify(onWillStartRenderingFrameListener).onWillStartRenderingFrame();
    mapChangeEventManager.removeOnWillStartRenderingFrameListener(onWillStartRenderingFrameListener);
    mapChangeEventManager.onWillStartRenderingFrame();
    verify(onWillStartRenderingFrameListener).onWillStartRenderingFrame();
  }

  @Test
  public void testOnDidFinishRenderingFrameListener() {
    mapChangeEventManager.addOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(true);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);
    mapChangeEventManager.removeOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(true);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);
  }

  @Test
  public void testOnDidFinishRenderingFrameFullyRenderedListener() {
    mapChangeEventManager.addOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(false);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);
    mapChangeEventManager.removeOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(false);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);
  }

  @Test
  public void testOnWillStartRenderingMapListener() {
    mapChangeEventManager.addOnWillStartRenderingMapListener(onWillStartRenderingMapListener);
    mapChangeEventManager.onWillStartRenderingMap();
    verify(onWillStartRenderingMapListener).onWillStartRenderingMap();
    mapChangeEventManager.removeOnWillStartRenderingMapListener(onWillStartRenderingMapListener);
    mapChangeEventManager.onWillStartRenderingMap();
    verify(onWillStartRenderingMapListener).onWillStartRenderingMap();
  }

  @Test
  public void testOnDidFinishRenderingMapListener() {
    mapChangeEventManager.addOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(true);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);
    mapChangeEventManager.removeOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(true);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);
  }

  @Test
  public void testOnDidFinishRenderingMapFullyRenderedListener() {
    mapChangeEventManager.addOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(false);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);
    mapChangeEventManager.removeOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(false);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);
  }

  @Test
  public void testOnDidFinishLoadingStyleListener() {
    mapChangeEventManager.addOnDidFinishLoadingStyleListener(onDidFinishLoadingStyleListener);
    mapChangeEventManager.onDidFinishLoadingStyle();
    verify(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();
    mapChangeEventManager.removeOnDidFinishLoadingStyleListener(onDidFinishLoadingStyleListener);
    mapChangeEventManager.onDidFinishLoadingStyle();
    verify(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();
  }

  @Test
  public void testOnSourceChangedListener() {
    mapChangeEventManager.addOnSourceChangedListener(onSourceChangedListener);
    mapChangeEventManager.onSourceChanged(TEST_STRING);
    verify(onSourceChangedListener).onSourceChangedListener(TEST_STRING);
    mapChangeEventManager.removeOnSourceChangedListener(onSourceChangedListener);
    mapChangeEventManager.onSourceChanged(TEST_STRING);
    verify(onSourceChangedListener).onSourceChangedListener(TEST_STRING);
  }
}
