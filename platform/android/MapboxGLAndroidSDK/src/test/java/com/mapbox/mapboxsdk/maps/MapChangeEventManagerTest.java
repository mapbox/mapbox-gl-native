package com.mapbox.mapboxsdk.maps;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

/**
 * Tests integration of MapChangeDispatcher and see if events are correctly forwarded.
 */
public class MapChangeEventManagerTest {

  private static final String TEST_STRING = "mapChangeRandom";

  private MapChangeEventManager mapChangeEventManager;

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

  @Mock
  private MapView.OnMapChangedListener onMapChangedListener;

  @Mock
  private MapView.MapChangeInternalHandler mapCallback;

  @Before
  public void beforeTest() {
    MockitoAnnotations.initMocks(this);
    mapChangeEventManager = new MapChangeEventManager();
    mapChangeEventManager.addOnMapChangedListener(onMapChangedListener);
    mapChangeEventManager.bind(mapCallback);
  }

  @Test
  public void testOnCameraRegionWillChangeListener() throws Exception {
    mapChangeEventManager.addOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(false);
    verify(onCameraWillChangeListener).onCameraWillChange(false);
    verify(onMapChangedListener).onMapChanged(MapView.REGION_WILL_CHANGE);
    mapChangeEventManager.removeOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(false);
    verify(onCameraWillChangeListener).onCameraWillChange(false);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.REGION_WILL_CHANGE);
  }

  @Test
  public void testOnCameraRegionWillChangeAnimatedListener() throws Exception {
    mapChangeEventManager.addOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(true);
    verify(onCameraWillChangeListener).onCameraWillChange(true);
    verify(onMapChangedListener).onMapChanged(MapView.REGION_WILL_CHANGE_ANIMATED);
    mapChangeEventManager.removeOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(true);
    verify(onCameraWillChangeListener).onCameraWillChange(true);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.REGION_WILL_CHANGE_ANIMATED);
  }

  @Test
  public void testOnCameraIsChangingListener() throws Exception {
    mapChangeEventManager.addOnCameraIsChangingListener(onCameraIsChangingListener);
    mapChangeEventManager.onCameraIsChanging();
    verify(onCameraIsChangingListener).onCameraIsChanging();
    verify(onMapChangedListener).onMapChanged(MapView.REGION_IS_CHANGING);
    verify(mapCallback).onCameraIsChanging();
    mapChangeEventManager.removeOnCameraIsChangingListener(onCameraIsChangingListener);
    mapChangeEventManager.onCameraIsChanging();
    verify(onCameraIsChangingListener).onCameraIsChanging();
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.REGION_IS_CHANGING);
  }

  @Test
  public void testOnCameraRegionDidChangeListener() throws Exception {
    mapChangeEventManager.addOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(false);
    verify(onCameraDidChangeListener).onCameraDidChange(false);
    verify(onMapChangedListener).onMapChanged(MapView.REGION_DID_CHANGE);
    verify(mapCallback).onCameraDidChange(false);
    mapChangeEventManager.removeOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(false);
    verify(onCameraDidChangeListener).onCameraDidChange(false);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.REGION_DID_CHANGE);
  }

  @Test
  public void testOnCameraRegionDidChangeAnimatedListener() throws Exception {
    mapChangeEventManager.addOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(true);
    verify(onCameraDidChangeListener).onCameraDidChange(true);
    verify(onMapChangedListener).onMapChanged(MapView.REGION_DID_CHANGE_ANIMATED);
    verify(mapCallback).onCameraDidChange(true);
    mapChangeEventManager.removeOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(true);
    verify(onCameraDidChangeListener).onCameraDidChange(true);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.REGION_DID_CHANGE_ANIMATED);
    verify(mapCallback, times(2)).onCameraDidChange(true);
  }

  @Test
  public void testOnWillStartLoadingMapListener() throws Exception {
    mapChangeEventManager.addOnWillStartLoadingMapListener(onWillStartLoadingMapListener);
    mapChangeEventManager.onWillStartLoadingMap();
    verify(onWillStartLoadingMapListener).onWillStartLoadingMap();
    verify(onMapChangedListener).onMapChanged(MapView.WILL_START_LOADING_MAP);
    mapChangeEventManager.removeOnWillStartLoadingMapListener(onWillStartLoadingMapListener);
    mapChangeEventManager.onWillStartLoadingMap();
    verify(onWillStartLoadingMapListener).onWillStartLoadingMap();
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.WILL_START_LOADING_MAP);
  }

  @Test
  public void testOnDidFinishLoadingMapListener() throws Exception {
    mapChangeEventManager.addOnDidFinishLoadingMapListener(onDidFinishLoadingMapListener);
    mapChangeEventManager.onDidFinishLoadingMap();
    verify(onDidFinishLoadingMapListener).onDidFinishLoadingMap();
    verify(onMapChangedListener).onMapChanged(MapView.DID_FINISH_LOADING_MAP);
    verify(mapCallback).onDidFinishLoadingMap();
    mapChangeEventManager.removeOnDidFinishLoadingMapListener(onDidFinishLoadingMapListener);
    mapChangeEventManager.onDidFinishLoadingMap();
    verify(onDidFinishLoadingMapListener).onDidFinishLoadingMap();
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.DID_FINISH_LOADING_MAP);
    verify(mapCallback, times(2)).onDidFinishLoadingMap();
  }

  @Test
  public void testOnDidFailLoadingMapListener() throws Exception {
    mapChangeEventManager.addOnDidFailLoadingMapListener(onDidFailLoadingMapListener);
    mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
    verify(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);
    verify(onMapChangedListener).onMapChanged(MapView.DID_FAIL_LOADING_MAP);
    mapChangeEventManager.removeOnDidFailLoadingMapListener(onDidFailLoadingMapListener);
    mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
    verify(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.DID_FAIL_LOADING_MAP);
  }

  @Test
  public void testOnWillStartRenderingFrameListener() throws Exception {
    mapChangeEventManager.addOnWillStartRenderingFrameListener(onWillStartRenderingFrameListener);
    mapChangeEventManager.onWillStartRenderingFrame();
    verify(onWillStartRenderingFrameListener).onWillStartRenderingFrame();
    verify(onMapChangedListener).onMapChanged(MapView.WILL_START_RENDERING_FRAME);
    mapChangeEventManager.removeOnWillStartRenderingFrameListener(onWillStartRenderingFrameListener);
    mapChangeEventManager.onWillStartRenderingFrame();
    verify(onWillStartRenderingFrameListener).onWillStartRenderingFrame();
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.WILL_START_RENDERING_FRAME);
  }

  @Test
  public void testOnDidFinishRenderingFrameListener() throws Exception {
    mapChangeEventManager.addOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(true);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);
    verify(onMapChangedListener).onMapChanged(MapView.DID_FINISH_RENDERING_FRAME);
    verify(mapCallback).onDidFinishRenderingFrame(true);
    mapChangeEventManager.removeOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(true);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.DID_FINISH_RENDERING_FRAME);
    verify(mapCallback, times(2)).onDidFinishRenderingFrame(true);
  }

  @Test
  public void testOnDidFinishRenderingFrameFullyRenderedListener() throws Exception {
    mapChangeEventManager.addOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(false);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);
    verify(onMapChangedListener).onMapChanged(MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED);
    verify(mapCallback).onDidFinishRenderingFrame(false);
    mapChangeEventManager.removeOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(false);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED);
    verify(mapCallback, times(2)).onDidFinishRenderingFrame(false);
  }

  @Test
  public void testOnWillStartRenderingMapListener() throws Exception {
    mapChangeEventManager.addOnWillStartRenderingMapListener(onWillStartRenderingMapListener);
    mapChangeEventManager.onWillStartRenderingMap();
    verify(onWillStartRenderingMapListener).onWillStartRenderingMap();
    verify(onMapChangedListener).onMapChanged(MapView.WILL_START_RENDERING_MAP);
    mapChangeEventManager.removeOnWillStartRenderingMapListener(onWillStartRenderingMapListener);
    mapChangeEventManager.onWillStartRenderingMap();
    verify(onWillStartRenderingMapListener).onWillStartRenderingMap();
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.WILL_START_RENDERING_MAP);
  }

  @Test
  public void testOnDidFinishRenderingMapListener() throws Exception {
    mapChangeEventManager.addOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(true);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);
    verify(onMapChangedListener).onMapChanged(MapView.DID_FINISH_RENDERING_MAP);
    mapChangeEventManager.removeOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(true);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.DID_FINISH_RENDERING_MAP);
  }

  @Test
  public void testOnDidFinishRenderingMapFullyRenderedListener() throws Exception {
    mapChangeEventManager.addOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(false);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);
    verify(onMapChangedListener).onMapChanged(MapView.DID_FINISH_RENDERING_MAP_FULLY_RENDERED);
    mapChangeEventManager.removeOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(false);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.DID_FINISH_RENDERING_MAP_FULLY_RENDERED);
  }

  @Test
  public void testOnDidFinishLoadingStyleListener() throws Exception {
    mapChangeEventManager.addOnDidFinishLoadingStyleListener(onDidFinishLoadingStyleListener);
    mapChangeEventManager.onDidFinishLoadingStyle();
    verify(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();
    verify(onMapChangedListener).onMapChanged(MapView.DID_FINISH_LOADING_STYLE);
    verify(mapCallback).onDidFinishLoadingStyle();
    mapChangeEventManager.removeOnDidFinishLoadingStyleListener(onDidFinishLoadingStyleListener);
    mapChangeEventManager.onDidFinishLoadingStyle();
    verify(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.DID_FINISH_LOADING_STYLE);
    verify(mapCallback, times(2)).onDidFinishLoadingStyle();
  }

  @Test
  public void testOnSourceChangedListener() throws Exception {
    mapChangeEventManager.addOnSourceChangedListener(onSourceChangedListener);
    mapChangeEventManager.onSourceChanged(TEST_STRING);
    verify(onSourceChangedListener).onSourceChangedListener(TEST_STRING);
    verify(onMapChangedListener).onMapChanged(MapView.SOURCE_DID_CHANGE);
    mapChangeEventManager.removeOnSourceChangedListener(onSourceChangedListener);
    mapChangeEventManager.onSourceChanged(TEST_STRING);
    verify(onSourceChangedListener).onSourceChangedListener(TEST_STRING);
    verify(onMapChangedListener, times(2)).onMapChanged(MapView.SOURCE_DID_CHANGE);
  }
}