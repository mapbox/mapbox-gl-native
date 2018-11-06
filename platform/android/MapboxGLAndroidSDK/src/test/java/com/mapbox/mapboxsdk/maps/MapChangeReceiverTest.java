package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.log.LoggerDefinition;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.doThrow;
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

  @Mock
  private LoggerDefinition loggerDefinition;

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

    mapChangeEventManager.addOnCameraWillChangeListener(onCameraWillChangeListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onCameraWillChangeListener).onCameraWillChange(false);
    mapChangeEventManager.onCameraWillChange(false);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnCameraRegionWillChangeAnimatedListener() {
    mapChangeEventManager.addOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(true);
    verify(onCameraWillChangeListener).onCameraWillChange(true);
    mapChangeEventManager.removeOnCameraWillChangeListener(onCameraWillChangeListener);
    mapChangeEventManager.onCameraWillChange(true);
    verify(onCameraWillChangeListener).onCameraWillChange(true);

    mapChangeEventManager.addOnCameraWillChangeListener(onCameraWillChangeListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onCameraWillChangeListener).onCameraWillChange(true);
    mapChangeEventManager.onCameraWillChange(true);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnCameraIsChangingListener() {
    mapChangeEventManager.addOnCameraIsChangingListener(onCameraIsChangingListener);
    mapChangeEventManager.onCameraIsChanging();
    verify(onCameraIsChangingListener).onCameraIsChanging();
    mapChangeEventManager.removeOnCameraIsChangingListener(onCameraIsChangingListener);
    mapChangeEventManager.onCameraIsChanging();
    verify(onCameraIsChangingListener).onCameraIsChanging();

    mapChangeEventManager.addOnCameraIsChangingListener(onCameraIsChangingListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onCameraIsChangingListener).onCameraIsChanging();
    mapChangeEventManager.onCameraIsChanging();
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnCameraRegionDidChangeListener() {
    mapChangeEventManager.addOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(false);
    verify(onCameraDidChangeListener).onCameraDidChange(false);
    mapChangeEventManager.removeOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(false);
    verify(onCameraDidChangeListener).onCameraDidChange(false);

    mapChangeEventManager.addOnCameraDidChangeListener(onCameraDidChangeListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onCameraDidChangeListener).onCameraDidChange(false);
    mapChangeEventManager.onCameraDidChange(false);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnCameraRegionDidChangeAnimatedListener() {
    mapChangeEventManager.addOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(true);
    verify(onCameraDidChangeListener).onCameraDidChange(true);
    mapChangeEventManager.removeOnCameraDidChangeListener(onCameraDidChangeListener);
    mapChangeEventManager.onCameraDidChange(true);
    verify(onCameraDidChangeListener).onCameraDidChange(true);

    mapChangeEventManager.addOnCameraDidChangeListener(onCameraDidChangeListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onCameraDidChangeListener).onCameraDidChange(true);
    mapChangeEventManager.onCameraDidChange(true);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnWillStartLoadingMapListener() {
    mapChangeEventManager.addOnWillStartLoadingMapListener(onWillStartLoadingMapListener);
    mapChangeEventManager.onWillStartLoadingMap();
    verify(onWillStartLoadingMapListener).onWillStartLoadingMap();
    mapChangeEventManager.removeOnWillStartLoadingMapListener(onWillStartLoadingMapListener);
    mapChangeEventManager.onWillStartLoadingMap();
    verify(onWillStartLoadingMapListener).onWillStartLoadingMap();

    mapChangeEventManager.addOnWillStartLoadingMapListener(onWillStartLoadingMapListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onWillStartLoadingMapListener).onWillStartLoadingMap();
    mapChangeEventManager.onWillStartLoadingMap();
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnDidFinishLoadingMapListener() {
    mapChangeEventManager.addOnDidFinishLoadingMapListener(onDidFinishLoadingMapListener);
    mapChangeEventManager.onDidFinishLoadingMap();
    verify(onDidFinishLoadingMapListener).onDidFinishLoadingMap();
    mapChangeEventManager.removeOnDidFinishLoadingMapListener(onDidFinishLoadingMapListener);
    mapChangeEventManager.onDidFinishLoadingMap();
    verify(onDidFinishLoadingMapListener).onDidFinishLoadingMap();

    mapChangeEventManager.addOnDidFinishLoadingMapListener(onDidFinishLoadingMapListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onDidFinishLoadingMapListener).onDidFinishLoadingMap();
    mapChangeEventManager.onDidFinishLoadingMap();
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnDidFailLoadingMapListener() {
    mapChangeEventManager.addOnDidFailLoadingMapListener(onDidFailLoadingMapListener);
    mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
    verify(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);
    mapChangeEventManager.removeOnDidFailLoadingMapListener(onDidFailLoadingMapListener);
    mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
    verify(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);

    mapChangeEventManager.addOnDidFailLoadingMapListener(onDidFailLoadingMapListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);
    mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnWillStartRenderingFrameListener() {
    mapChangeEventManager.addOnWillStartRenderingFrameListener(onWillStartRenderingFrameListener);
    mapChangeEventManager.onWillStartRenderingFrame();
    verify(onWillStartRenderingFrameListener).onWillStartRenderingFrame();
    mapChangeEventManager.removeOnWillStartRenderingFrameListener(onWillStartRenderingFrameListener);
    mapChangeEventManager.onWillStartRenderingFrame();
    verify(onWillStartRenderingFrameListener).onWillStartRenderingFrame();

    mapChangeEventManager.addOnWillStartRenderingFrameListener(onWillStartRenderingFrameListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onWillStartRenderingFrameListener).onWillStartRenderingFrame();
    mapChangeEventManager.onWillStartRenderingFrame();
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnDidFinishRenderingFrameListener() {
    mapChangeEventManager.addOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(true);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);
    mapChangeEventManager.removeOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(true);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);

    mapChangeEventManager.addOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);
    mapChangeEventManager.onDidFinishRenderingFrame(true);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnDidFinishRenderingFrameFullyRenderedListener() {
    mapChangeEventManager.addOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(false);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);
    mapChangeEventManager.removeOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    mapChangeEventManager.onDidFinishRenderingFrame(false);
    verify(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);

    mapChangeEventManager.addOnDidFinishRenderingFrameListener(onDidFinishRenderingFrameListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);
    mapChangeEventManager.onDidFinishRenderingFrame(false);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnWillStartRenderingMapListener() {
    mapChangeEventManager.addOnWillStartRenderingMapListener(onWillStartRenderingMapListener);
    mapChangeEventManager.onWillStartRenderingMap();
    verify(onWillStartRenderingMapListener).onWillStartRenderingMap();
    mapChangeEventManager.removeOnWillStartRenderingMapListener(onWillStartRenderingMapListener);
    mapChangeEventManager.onWillStartRenderingMap();
    verify(onWillStartRenderingMapListener).onWillStartRenderingMap();

    mapChangeEventManager.addOnWillStartRenderingMapListener(onWillStartRenderingMapListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onWillStartRenderingMapListener).onWillStartRenderingMap();
    mapChangeEventManager.onWillStartRenderingMap();
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnDidFinishRenderingMapListener() {
    mapChangeEventManager.addOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(true);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);
    mapChangeEventManager.removeOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(true);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);

    mapChangeEventManager.addOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);
    mapChangeEventManager.onDidFinishRenderingMap(true);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnDidFinishRenderingMapFullyRenderedListener() {
    mapChangeEventManager.addOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(false);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);
    mapChangeEventManager.removeOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    mapChangeEventManager.onDidFinishRenderingMap(false);
    verify(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);

    mapChangeEventManager.addOnDidFinishRenderingMapListener(onDidFinishRenderingMapListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);
    mapChangeEventManager.onDidFinishRenderingMap(false);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnDidFinishLoadingStyleListener() {
    mapChangeEventManager.addOnDidFinishLoadingStyleListener(onDidFinishLoadingStyleListener);
    mapChangeEventManager.onDidFinishLoadingStyle();
    verify(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();
    mapChangeEventManager.removeOnDidFinishLoadingStyleListener(onDidFinishLoadingStyleListener);
    mapChangeEventManager.onDidFinishLoadingStyle();
    verify(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();

    mapChangeEventManager.addOnDidFinishLoadingStyleListener(onDidFinishLoadingStyleListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();
    mapChangeEventManager.onDidFinishLoadingStyle();
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }

  @Test
  public void testOnSourceChangedListener() {
    mapChangeEventManager.addOnSourceChangedListener(onSourceChangedListener);
    mapChangeEventManager.onSourceChanged(TEST_STRING);
    verify(onSourceChangedListener).onSourceChangedListener(TEST_STRING);
    mapChangeEventManager.removeOnSourceChangedListener(onSourceChangedListener);
    mapChangeEventManager.onSourceChanged(TEST_STRING);
    verify(onSourceChangedListener).onSourceChangedListener(TEST_STRING);

    mapChangeEventManager.addOnSourceChangedListener(onSourceChangedListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception err = new RuntimeException();
    doThrow(err).when(onSourceChangedListener).onSourceChangedListener(TEST_STRING);
    mapChangeEventManager.onSourceChanged(TEST_STRING);
    verify(loggerDefinition).e(anyString(), anyString(), eq(err));
  }
}
