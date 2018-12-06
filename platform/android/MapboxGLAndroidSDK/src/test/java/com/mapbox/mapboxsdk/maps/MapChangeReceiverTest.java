package com.mapbox.mapboxsdk.maps;

import com.google.common.util.concurrent.ExecutionError;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.log.LoggerDefinition;

import junit.framework.Assert;

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
  private MapView.OnDidEnterIdleListener onDidEnterIdleListener;

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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onCameraWillChangeListener).onCameraWillChange(false);
    try {
      mapChangeEventManager.onCameraWillChange(false);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onCameraWillChangeListener).onCameraWillChange(false);
    try {
      mapChangeEventManager.onCameraWillChange(false);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onCameraWillChangeListener).onCameraWillChange(true);
    try {
      mapChangeEventManager.onCameraWillChange(true);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onCameraWillChangeListener).onCameraWillChange(true);
    try {
      mapChangeEventManager.onCameraWillChange(true);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onCameraIsChangingListener).onCameraIsChanging();
    try {
      mapChangeEventManager.onCameraIsChanging();
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onCameraIsChangingListener).onCameraIsChanging();
    try {
      mapChangeEventManager.onCameraIsChanging();
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onCameraDidChangeListener).onCameraDidChange(false);
    try {
      mapChangeEventManager.onCameraDidChange(false);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onCameraDidChangeListener).onCameraDidChange(false);
    try {
      mapChangeEventManager.onCameraDidChange(false);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onCameraDidChangeListener).onCameraDidChange(true);
    try {
      mapChangeEventManager.onCameraDidChange(true);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onCameraDidChangeListener).onCameraDidChange(true);
    try {
      mapChangeEventManager.onCameraDidChange(true);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onWillStartLoadingMapListener).onWillStartLoadingMap();
    try {
      mapChangeEventManager.onWillStartLoadingMap();
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onWillStartLoadingMapListener).onWillStartLoadingMap();
    try {
      mapChangeEventManager.onWillStartLoadingMap();
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onDidFinishLoadingMapListener).onDidFinishLoadingMap();
    try {
      mapChangeEventManager.onDidFinishLoadingMap();
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onDidFinishLoadingMapListener).onDidFinishLoadingMap();
    try {
      mapChangeEventManager.onDidFinishLoadingMap();
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);
    try {
      mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onDidFailLoadingMapListener).onDidFailLoadingMap(TEST_STRING);
    try {
      mapChangeEventManager.onDidFailLoadingMap(TEST_STRING);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onWillStartRenderingFrameListener).onWillStartRenderingFrame();
    try {
      mapChangeEventManager.onWillStartRenderingFrame();
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onWillStartRenderingFrameListener).onWillStartRenderingFrame();
    try {
      mapChangeEventManager.onWillStartRenderingFrame();
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);
    try {
      mapChangeEventManager.onDidFinishRenderingFrame(true);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(true);
    try {
      mapChangeEventManager.onDidFinishRenderingFrame(true);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);
    try {
      mapChangeEventManager.onDidFinishRenderingFrame(false);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onDidFinishRenderingFrameListener).onDidFinishRenderingFrame(false);
    try {
      mapChangeEventManager.onDidFinishRenderingFrame(false);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onWillStartRenderingMapListener).onWillStartRenderingMap();
    try {
      mapChangeEventManager.onWillStartRenderingMap();
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onWillStartRenderingMapListener).onWillStartRenderingMap();
    try {
      mapChangeEventManager.onWillStartRenderingMap();
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);
    try {
      mapChangeEventManager.onDidFinishRenderingMap(true);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onDidFinishRenderingMapListener).onDidFinishRenderingMap(true);
    try {
      mapChangeEventManager.onDidFinishRenderingMap(true);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);
    try {
      mapChangeEventManager.onDidFinishRenderingMap(false);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onDidFinishRenderingMapListener).onDidFinishRenderingMap(false);
    try {
      mapChangeEventManager.onDidFinishRenderingMap(false);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
  }

  @Test
  public void testOnDidEnterIdleListener() {
    mapChangeEventManager.addOnDidEnterIdleListener(onDidEnterIdleListener);
    mapChangeEventManager.onDidEnterIdle();
    verify(onDidEnterIdleListener).onDidEnterIdle();
    mapChangeEventManager.removeOnDidEnterIdleListener(onDidEnterIdleListener);
    mapChangeEventManager.onDidEnterIdle();
    verify(onDidEnterIdleListener).onDidEnterIdle();

    mapChangeEventManager.addOnDidEnterIdleListener(onDidEnterIdleListener);
    Logger.setLoggerDefinition(loggerDefinition);
    Exception exc = new RuntimeException();
    doThrow(exc).when(onDidEnterIdleListener).onDidEnterIdle();
    try {
      mapChangeEventManager.onDidEnterIdle();
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onDidEnterIdleListener).onDidEnterIdle();
    try {
      mapChangeEventManager.onDidEnterIdle();
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();
    try {
      mapChangeEventManager.onDidFinishLoadingStyle();
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onDidFinishLoadingStyleListener).onDidFinishLoadingStyle();
    try {
      mapChangeEventManager.onDidFinishLoadingStyle();
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
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
    Exception exc = new RuntimeException();
    doThrow(exc).when(onSourceChangedListener).onSourceChangedListener(TEST_STRING);
    try {
      mapChangeEventManager.onSourceChanged(TEST_STRING);
      Assert.fail("The exception should've been re-thrown.");
    } catch (RuntimeException throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(exc));
    }

    Error err = new ExecutionError("", new Error());
    doThrow(err).when(onSourceChangedListener).onSourceChangedListener(TEST_STRING);
    try {
      mapChangeEventManager.onSourceChanged(TEST_STRING);
      Assert.fail("The exception should've been re-thrown.");
    } catch (ExecutionError throwable) {
      verify(loggerDefinition).e(anyString(), anyString(), eq(err));
    }
  }
}
