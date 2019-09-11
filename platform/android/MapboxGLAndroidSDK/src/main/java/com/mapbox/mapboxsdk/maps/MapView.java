package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.annotation.CallSuper;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.util.LongSparseArray;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.TextureView;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.MapboxConfigurationException;
import com.mapbox.mapboxsdk.location.LocationComponent;
import com.mapbox.mapboxsdk.maps.renderer.MapRenderer;
import com.mapbox.mapboxsdk.maps.renderer.glsurfaceview.GLSurfaceViewMapRenderer;
import com.mapbox.mapboxsdk.maps.renderer.glsurfaceview.MapboxGLSurfaceView;
import com.mapbox.mapboxsdk.maps.renderer.textureview.TextureViewMapRenderer;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static com.mapbox.mapboxsdk.maps.widgets.CompassView.TIME_MAP_NORTH_ANIMATION;
import static com.mapbox.mapboxsdk.maps.widgets.CompassView.TIME_WAIT_IDLE;

/**
 * <p>
 * A {@code MapView} provides an embeddable map interface.
 * You use this class to display map information and to manipulate the map contents from your application.
 * You can center the map on a given coordinate, specify the size of the area you want to display,
 * and style the features of the map to fit your application's use case.
 * </p>
 * <p>
 * Use of {@code MapView} requires a Mapbox API access token.
 * Obtain an access token on the <a href="https://www.mapbox.com/studio/account/tokens/">Mapbox account page</a>.
 * </p>
 * <strong>Warning:</strong> Please note that you are responsible for getting permission to use the map data,
 * and for ensuring your use adheres to the relevant terms of use.
 */
public class MapView extends FrameLayout implements NativeMapView.ViewCallback {

  private final MapChangeReceiver mapChangeReceiver = new MapChangeReceiver();
  private final MapCallback mapCallback = new MapCallback();
  private final InitialRenderCallback initialRenderCallback = new InitialRenderCallback();

  @Nullable
  private NativeMap nativeMapView;
  @Nullable
  private MapboxMap mapboxMap;
  private AttributionClickListener attributionClickListener;
  private MapboxMapOptions mapboxMapOptions;
  private MapRenderer mapRenderer;
  private boolean destroyed;

  private CompassView compassView;
  private PointF focalPoint;
  private ImageView attrView;
  private ImageView logoView;

  @Nullable
  private MapGestureDetector mapGestureDetector;
  @Nullable
  private MapKeyListener mapKeyListener;
  @Nullable
  private Bundle savedInstanceState;
  private boolean isStarted;

  @UiThread
  public MapView(@NonNull Context context) {
    super(context);
    initialize(context, MapboxMapOptions.createFromAttributes(context));
  }

  @UiThread
  public MapView(@NonNull Context context, @Nullable AttributeSet attrs) {
    super(context, attrs);
    initialize(context, MapboxMapOptions.createFromAttributes(context, attrs));
  }

  @UiThread
  public MapView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
    super(context, attrs, defStyleAttr);
    initialize(context, MapboxMapOptions.createFromAttributes(context, attrs));
  }

  @UiThread
  public MapView(@NonNull Context context, @Nullable MapboxMapOptions options) {
    super(context);
    initialize(context, options == null ? MapboxMapOptions.createFromAttributes(context) : options);
  }

  @CallSuper
  @UiThread
  protected void initialize(@NonNull final Context context, @NonNull final MapboxMapOptions options) {
    if (isInEditMode()) {
      // in IDE layout editor, just return
      return;
    }

    if (!Mapbox.hasInstance()) {
      throw new MapboxConfigurationException();
    }

    // hide surface until map is fully loaded #10990
    setForeground(new ColorDrawable(options.getForegroundLoadColor()));

    mapboxMapOptions = options;

    // inflate view
    View view = LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_internal, this);
    compassView = view.findViewById(R.id.compassView);
    attrView = view.findViewById(R.id.attributionView);
    attrView.setImageDrawable(BitmapUtils.getDrawableFromRes(getContext(), R.drawable.mapbox_info_bg_selector));
    logoView = view.findViewById(R.id.logoView);
    logoView.setImageDrawable(BitmapUtils.getDrawableFromRes(getContext(), R.drawable.mapbox_logo_icon));

    // add accessibility support
    setContentDescription(context.getString(R.string.mapbox_mapActionDescription));
    setWillNotDraw(false);
    initialiseDrawingSurface(options);
  }

  private void initialiseMap() {
    Context context = getContext();

    // callback for focal point invalidation
    final FocalPointInvalidator focalInvalidator = new FocalPointInvalidator();
    focalInvalidator.addListener(createFocalPointChangeListener());

    // callback for registering touch listeners
    GesturesManagerInteractionListener registerTouchListener = new GesturesManagerInteractionListener();

    // callback for camera change events
    final CameraChangeDispatcher cameraDispatcher = new CameraChangeDispatcher();

    // setup components for MapboxMap creation
    Projection proj = new Projection(nativeMapView, this);
    UiSettings uiSettings = new UiSettings(proj, focalInvalidator, compassView, attrView, logoView, getPixelRatio());
    LongSparseArray<Annotation> annotationsArray = new LongSparseArray<>();
    IconManager iconManager = new IconManager(nativeMapView);
    Annotations annotations = new AnnotationContainer(nativeMapView, annotationsArray);
    Markers markers = new MarkerContainer(nativeMapView, annotationsArray, iconManager);
    Polygons polygons = new PolygonContainer(nativeMapView, annotationsArray);
    Polylines polylines = new PolylineContainer(nativeMapView, annotationsArray);
    ShapeAnnotations shapeAnnotations = new ShapeAnnotationContainer(nativeMapView, annotationsArray);
    AnnotationManager annotationManager = new AnnotationManager(this, annotationsArray, iconManager,
      annotations, markers, polygons, polylines, shapeAnnotations);
    Transform transform = new Transform(this, nativeMapView, cameraDispatcher);

    // MapboxMap
    List<MapboxMap.OnDeveloperAnimationListener> developerAnimationListeners = new ArrayList<>();
    mapboxMap = new MapboxMap(nativeMapView, transform, uiSettings, proj, registerTouchListener, cameraDispatcher,
      developerAnimationListeners);
    mapboxMap.injectAnnotationManager(annotationManager);

    // user input
    mapGestureDetector = new MapGestureDetector(context, transform, proj, uiSettings,
      annotationManager, cameraDispatcher);
    mapKeyListener = new MapKeyListener(transform, uiSettings, mapGestureDetector);

    // compass
    compassView.injectCompassAnimationListener(createCompassAnimationListener(cameraDispatcher));
    compassView.setOnClickListener(createCompassClickListener(cameraDispatcher));

    // LocationComponent
    mapboxMap.injectLocationComponent(new LocationComponent(mapboxMap, transform, developerAnimationListeners));

    // inject widgets with MapboxMap
    attrView.setOnClickListener(attributionClickListener = new AttributionClickListener(context, mapboxMap));

    // Ensure this view is interactable
    setClickable(true);
    setLongClickable(true);
    setFocusable(true);
    setFocusableInTouchMode(true);
    requestDisallowInterceptTouchEvent(true);

    // notify Map object about current connectivity state
    nativeMapView.setReachability(Mapbox.isConnected());

    // initialise MapboxMap
    if (savedInstanceState == null) {
      mapboxMap.initialise(context, mapboxMapOptions);
    } else {
      mapboxMap.onRestoreInstanceState(savedInstanceState);
    }

    mapCallback.initialised();
  }

  private FocalPointChangeListener createFocalPointChangeListener() {
    return new FocalPointChangeListener() {
      @Override
      public void onFocalPointChanged(PointF pointF) {
        focalPoint = pointF;
      }
    };
  }

  private MapboxMap.OnCompassAnimationListener createCompassAnimationListener(@NonNull final CameraChangeDispatcher
                                                                                cameraChangeDispatcher) {
    return new MapboxMap.OnCompassAnimationListener() {
      @Override
      public void onCompassAnimation() {
        cameraChangeDispatcher.onCameraMove();
      }

      @Override
      public void onCompassAnimationFinished() {
        compassView.isAnimating(false);
        cameraChangeDispatcher.onCameraIdle();
      }
    };
  }

  private OnClickListener createCompassClickListener(@NonNull final CameraChangeDispatcher cameraChangeDispatcher) {
    return new OnClickListener() {
      @Override
      public void onClick(View v) {
        if (mapboxMap != null && compassView != null) {
          if (focalPoint != null) {
            mapboxMap.setFocalBearing(0, focalPoint.x, focalPoint.y, TIME_MAP_NORTH_ANIMATION);
          } else {
            mapboxMap.setFocalBearing(0, mapboxMap.getWidth() / 2, mapboxMap.getHeight() / 2, TIME_MAP_NORTH_ANIMATION);
          }
          cameraChangeDispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_ANIMATION);
          compassView.isAnimating(true);
          compassView.postDelayed(compassView, TIME_WAIT_IDLE + TIME_MAP_NORTH_ANIMATION);
        }
      }
    };
  }

  //
  // Lifecycle events
  //

  /**
   * <p>
   * You must call this method from the parent's Activity#onCreate(Bundle)} or
   * Fragment#onViewCreated(View, Bundle).
   * </p>
   * You must set a valid access token with {@link com.mapbox.mapboxsdk.Mapbox#getInstance(Context, String)}
   * before you call this method or an exception will be thrown.
   *
   * @param savedInstanceState Pass in the parent's savedInstanceState.
   * @see com.mapbox.mapboxsdk.Mapbox#getInstance(Context, String)
   */
  @UiThread
  public void onCreate(@Nullable Bundle savedInstanceState) {
    if (savedInstanceState == null) {
      TelemetryDefinition telemetry = Mapbox.getTelemetry();
      if (telemetry != null) {
        telemetry.onAppUserTurnstileEvent();
      }
    } else if (savedInstanceState.getBoolean(MapboxConstants.STATE_HAS_SAVED_STATE)) {
      this.savedInstanceState = savedInstanceState;
    }
  }

  private void initialiseDrawingSurface(MapboxMapOptions options) {
    String localFontFamily = options.getLocalIdeographFontFamily();
    if (options.getTextureMode()) {
      TextureView textureView = new TextureView(getContext());
      boolean translucentSurface = options.getTranslucentTextureSurface();
      mapRenderer = new TextureViewMapRenderer(getContext(),
        textureView, localFontFamily, translucentSurface) {
        @Override
        protected void onSurfaceCreated(GL10 gl, EGLConfig config) {
          MapView.this.onSurfaceCreated();
          super.onSurfaceCreated(gl, config);
        }
      };

      addView(textureView, 0);
    } else {
      MapboxGLSurfaceView glSurfaceView = new MapboxGLSurfaceView(getContext());
      glSurfaceView.setZOrderMediaOverlay(mapboxMapOptions.getRenderSurfaceOnTop());
      mapRenderer = new GLSurfaceViewMapRenderer(getContext(), glSurfaceView, localFontFamily) {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
          MapView.this.onSurfaceCreated();
          super.onSurfaceCreated(gl, config);
        }
      };

      addView(glSurfaceView, 0);
    }

    boolean crossSourceCollisions = mapboxMapOptions.getCrossSourceCollisions();
    nativeMapView = new NativeMapView(
      getContext(), getPixelRatio(), crossSourceCollisions, this, mapChangeReceiver, mapRenderer
    );
  }

  private void onSurfaceCreated() {
    post(new Runnable() {
      @Override
      public void run() {
        // Initialise only when not destroyed and only once
        if (!destroyed && mapboxMap == null) {
          MapView.this.initialiseMap();
          mapboxMap.onStart();
        }
      }
    });
  }

  /**
   * You must call this method from the parent's Activity#onSaveInstanceState(Bundle)
   * or Fragment#onSaveInstanceState(Bundle).
   *
   * @param outState Pass in the parent's outState.
   */
  @UiThread
  public void onSaveInstanceState(@NonNull Bundle outState) {
    if (mapboxMap != null) {
      outState.putBoolean(MapboxConstants.STATE_HAS_SAVED_STATE, true);
      mapboxMap.onSaveInstanceState(outState);
    }
  }

  /**
   * You must call this method from the parent's Activity#onStart() or Fragment#onStart()
   */
  @UiThread
  public void onStart() {
    if (!isStarted) {
      ConnectivityReceiver.instance(getContext()).activate();
      FileSource.getInstance(getContext()).activate();
      isStarted = true;
    }
    if (mapboxMap != null) {
      mapboxMap.onStart();
    }

    if (mapRenderer != null) {
      mapRenderer.onStart();
    }
  }

  /**
   * You must call this method from the parent's Activity#onResume() or Fragment#onResume().
   */
  @UiThread
  public void onResume() {
    if (mapRenderer != null) {
      mapRenderer.onResume();
    }
  }

  /**
   * You must call this method from the parent's Activity#onPause() or Fragment#onPause().
   */
  @UiThread
  public void onPause() {
    if (mapRenderer != null) {
      mapRenderer.onPause();
    }
  }

  /**
   * You must call this method from the parent's Activity#onStop() or Fragment#onStop().
   */
  @UiThread
  public void onStop() {
    if (attributionClickListener != null) {
      attributionClickListener.onStop();
    }

    if (mapboxMap != null) {
      // map was destroyed before it was started
      mapGestureDetector.cancelAnimators();
      mapboxMap.onStop();
    }

    if (mapRenderer != null) {
      mapRenderer.onStop();
    }

    if (isStarted) {
      ConnectivityReceiver.instance(getContext()).deactivate();
      FileSource.getInstance(getContext()).deactivate();
      isStarted = false;
    }
  }

  /**
   * You must call this method from the parent's Activity#onDestroy() or Fragment#onDestroyView().
   */
  @UiThread
  public void onDestroy() {
    destroyed = true;
    mapChangeReceiver.clear();
    mapCallback.onDestroy();
    initialRenderCallback.onDestroy();

    if (compassView != null) {
      // avoid leaking context through animator #13742
      compassView.resetAnimation();
    }

    if (mapboxMap != null) {
      mapboxMap.onDestroy();
    }

    if (nativeMapView != null) {
      // null when destroying an activity programmatically mapbox-navigation-android/issues/503
      nativeMapView.destroy();
      nativeMapView = null;
    }

    if (mapRenderer != null) {
      mapRenderer.onDestroy();
    }
  }

  /**
   * The maximum frame rate at which the map view is rendered,
   * but it can't excess the ability of device hardware.
   *
   * @param maximumFps Can be set to arbitrary integer values.
   */
  public void setMaximumFps(int maximumFps) {
    if (mapRenderer != null) {
      mapRenderer.setMaximumFps(maximumFps);
    } else {
      throw new IllegalStateException("Calling MapView#setMaximumFps before mapRenderer is created.");
    }
  }

  /**
   * Returns if the map has been destroyed.
   * <p>
   * This method can be used to determine if the result of an asynchronous operation should be set.
   * </p>
   *
   * @return true, if the map has been destroyed
   */
  @UiThread
  public boolean isDestroyed() {
    return destroyed;
  }

  @Override
  public boolean onTouchEvent(MotionEvent event) {
    if (!isGestureDetectorInitialized()) {
      return super.onTouchEvent(event);
    }

    return mapGestureDetector.onTouchEvent(event) || super.onTouchEvent(event);
  }

  @Override
  public boolean onKeyDown(int keyCode, @NonNull KeyEvent event) {
    return mapKeyListener.onKeyDown(keyCode, event) || super.onKeyDown(keyCode, event);
  }

  @Override
  public boolean onKeyLongPress(int keyCode, KeyEvent event) {
    return mapKeyListener.onKeyLongPress(keyCode, event) || super.onKeyLongPress(keyCode, event);
  }

  @Override
  public boolean onKeyUp(int keyCode, @NonNull KeyEvent event) {
    return mapKeyListener.onKeyUp(keyCode, event) || super.onKeyUp(keyCode, event);
  }

  @Override
  public boolean onTrackballEvent(@NonNull MotionEvent event) {
    return mapKeyListener.onTrackballEvent(event) || super.onTrackballEvent(event);
  }

  @Override
  public boolean onGenericMotionEvent(@NonNull MotionEvent event) {
    if (!isGestureDetectorInitialized()) {
      return super.onGenericMotionEvent(event);
    }
    return mapGestureDetector.onGenericMotionEvent(event) || super.onGenericMotionEvent(event);
  }

  /**
   * You must call this method from the parent's Activity#onLowMemory() or Fragment#onLowMemory().
   */
  @UiThread
  public void onLowMemory() {
    if (nativeMapView != null && mapboxMap != null && !destroyed) {
      nativeMapView.onLowMemory();
    }
  }

  //
  // Rendering
  //

  @Override
  protected void onSizeChanged(int width, int height, int oldw, int oldh) {
    if (!isInEditMode() && nativeMapView != null) {
      // null-checking the nativeMapView, see #13277
      nativeMapView.resizeView(width, height);
    }
  }

  private float getPixelRatio() {
    // check is user defined his own pixel ratio value
    float pixelRatio = mapboxMapOptions.getPixelRatio();
    if (pixelRatio == 0) {
      // if not, get the one defined by the system
      pixelRatio = getResources().getDisplayMetrics().density;
    }
    return pixelRatio;
  }

  //
  // ViewCallback
  //

  @Nullable
  @Override
  public Bitmap getViewContent() {
    return BitmapUtils.createBitmapFromView(this);
  }

  //
  // Map events
  //

  /**
   * Set a callback that's invoked when the camera region will change.
   *
   * @param listener The callback that's invoked when the camera region will change
   */
  public void addOnCameraWillChangeListener(@NonNull OnCameraWillChangeListener listener) {
    mapChangeReceiver.addOnCameraWillChangeListener(listener);
  }

  /**
   * Remove a callback that's invoked when the camera region will change.
   *
   * @param listener The callback that's invoked when the camera region will change
   */
  public void removeOnCameraWillChangeListener(@NonNull OnCameraWillChangeListener listener) {
    mapChangeReceiver.removeOnCameraWillChangeListener(listener);
  }

  /**
   * Set a callback that's invoked when the camera is changing.
   *
   * @param listener The callback that's invoked when the camera is changing
   */
  public void addOnCameraIsChangingListener(@NonNull OnCameraIsChangingListener listener) {
    mapChangeReceiver.addOnCameraIsChangingListener(listener);
  }

  /**
   * Remove a callback that's invoked when the camera is changing.
   *
   * @param listener The callback that's invoked when the camera is changing
   */
  public void removeOnCameraIsChangingListener(@NonNull OnCameraIsChangingListener listener) {
    mapChangeReceiver.removeOnCameraIsChangingListener(listener);
  }

  /**
   * Set a callback that's invoked when the camera region did change.
   *
   * @param listener The callback that's invoked when the camera region did change
   */
  public void addOnCameraDidChangeListener(@NonNull OnCameraDidChangeListener listener) {
    mapChangeReceiver.addOnCameraDidChangeListener(listener);
  }

  /**
   * Set a callback that's invoked when the camera region did change.
   *
   * @param listener The callback that's invoked when the camera region did change
   */
  public void removeOnCameraDidChangeListener(@NonNull OnCameraDidChangeListener listener) {
    mapChangeReceiver.removeOnCameraDidChangeListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start loading.
   *
   * @param listener The callback that's invoked when the map will start loading
   */
  public void addOnWillStartLoadingMapListener(@NonNull OnWillStartLoadingMapListener listener) {
    mapChangeReceiver.addOnWillStartLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start loading.
   *
   * @param listener The callback that's invoked when the map will start loading
   */
  public void removeOnWillStartLoadingMapListener(@NonNull OnWillStartLoadingMapListener listener) {
    mapChangeReceiver.removeOnWillStartLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished loading.
   *
   * @param listener The callback that's invoked when the map has finished loading
   */
  public void addOnDidFinishLoadingMapListener(@NonNull OnDidFinishLoadingMapListener listener) {
    mapChangeReceiver.addOnDidFinishLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished loading.
   *
   * @param listener The callback that's invoked when the map has finished loading
   */
  public void removeOnDidFinishLoadingMapListener(@NonNull OnDidFinishLoadingMapListener listener) {
    mapChangeReceiver.removeOnDidFinishLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map failed to load.
   *
   * @param listener The callback that's invoked when the map failed to load
   */
  public void addOnDidFailLoadingMapListener(@NonNull OnDidFailLoadingMapListener listener) {
    mapChangeReceiver.addOnDidFailLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map failed to load.
   *
   * @param listener The callback that's invoked when the map failed to load
   */
  public void removeOnDidFailLoadingMapListener(@NonNull OnDidFailLoadingMapListener listener) {
    mapChangeReceiver.removeOnDidFailLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start rendering a frame.
   *
   * @param listener The callback that's invoked when the camera will start rendering a frame
   */
  public void addOnWillStartRenderingFrameListener(@NonNull OnWillStartRenderingFrameListener listener) {
    mapChangeReceiver.addOnWillStartRenderingFrameListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start rendering a frame.
   *
   * @param listener The callback that's invoked when the camera will start rendering a frame
   */
  public void removeOnWillStartRenderingFrameListener(@NonNull OnWillStartRenderingFrameListener listener) {
    mapChangeReceiver.removeOnWillStartRenderingFrameListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished rendering a frame.
   *
   * @param listener The callback that's invoked when the map has finished rendering a frame
   */
  public void addOnDidFinishRenderingFrameListener(@NonNull OnDidFinishRenderingFrameListener listener) {
    mapChangeReceiver.addOnDidFinishRenderingFrameListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished rendering a frame.
   *
   * @param listener The callback that's invoked when the map has finished rendering a frame
   */
  public void removeOnDidFinishRenderingFrameListener(@NonNull OnDidFinishRenderingFrameListener listener) {
    mapChangeReceiver.removeOnDidFinishRenderingFrameListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start rendering.
   *
   * @param listener The callback that's invoked when the map will start rendering
   */
  public void addOnWillStartRenderingMapListener(@NonNull OnWillStartRenderingMapListener listener) {
    mapChangeReceiver.addOnWillStartRenderingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start rendering.
   *
   * @param listener The callback that's invoked when the map will start rendering
   */
  public void removeOnWillStartRenderingMapListener(@NonNull OnWillStartRenderingMapListener listener) {
    mapChangeReceiver.removeOnWillStartRenderingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished rendering.
   *
   * @param listener The callback that's invoked when the map has finished rendering
   */
  public void addOnDidFinishRenderingMapListener(@NonNull OnDidFinishRenderingMapListener listener) {
    mapChangeReceiver.addOnDidFinishRenderingMapListener(listener);
  }

  /**
   * Remove a callback that's invoked when the map has finished rendering.
   *
   * @param listener The callback that's invoked when the map has has finished rendering.
   */
  public void removeOnDidFinishRenderingMapListener(OnDidFinishRenderingMapListener listener) {
    mapChangeReceiver.removeOnDidFinishRenderingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has entered the idle state.
   *
   * @param listener The callback that's invoked when the map has entered the idle state.
   */
  public void addOnDidBecomeIdleListener(@NonNull OnDidBecomeIdleListener listener) {
    mapChangeReceiver.addOnDidBecomeIdleListener(listener);
  }

  /**
   * Remove a callback that's invoked when the map has entered the idle state.
   *
   * @param listener The callback that's invoked when the map has entered the idle state.
   */
  public void removeOnDidBecomeIdleListener(@NonNull OnDidBecomeIdleListener listener) {
    mapChangeReceiver.removeOnDidBecomeIdleListener(listener);
  }

  /**
   * Set a callback that's invoked when the style has finished loading.
   *
   * @param listener The callback that's invoked when the style has finished loading
   */
  public void addOnDidFinishLoadingStyleListener(@NonNull OnDidFinishLoadingStyleListener listener) {
    mapChangeReceiver.addOnDidFinishLoadingStyleListener(listener);
  }

  /**
   * Set a callback that's invoked when the style has finished loading.
   *
   * @param listener The callback that's invoked when the style has finished loading
   */
  public void removeOnDidFinishLoadingStyleListener(@NonNull OnDidFinishLoadingStyleListener listener) {
    mapChangeReceiver.removeOnDidFinishLoadingStyleListener(listener);
  }

  /**
   * Set a callback that's invoked when a map source has changed.
   *
   * @param listener The callback that's invoked when the source has changed
   */
  public void addOnSourceChangedListener(@NonNull OnSourceChangedListener listener) {
    mapChangeReceiver.addOnSourceChangedListener(listener);
  }

  /**
   * Set a callback that's invoked when a map source has changed.
   *
   * @param listener The callback that's invoked when the source has changed
   */
  public void removeOnSourceChangedListener(@NonNull OnSourceChangedListener listener) {
    mapChangeReceiver.removeOnSourceChangedListener(listener);
  }

  /**
   * Set a callback that's invoked when the id of an icon is missing.
   *
   * @param listener The callback that's invoked when the id of an icon is missing
   */
  public void addOnStyleImageMissingListener(@NonNull OnStyleImageMissingListener listener) {
    mapChangeReceiver.addOnStyleImageMissingListener(listener);
  }

  /**
   * Set a callback that's invoked when a map source has changed.
   *
   * @param listener The callback that's invoked when the source has changed
   */
  public void removeOnStyleImageMissingListener(@NonNull OnStyleImageMissingListener listener) {
    mapChangeReceiver.removeOnStyleImageMissingListener(listener);
  }

  /**
   * Set a callback that's invoked when map needs to release unused image resources.
   * <p>
   * A callback will be called only for unused images that were provided by the client via
   * {@link OnStyleImageMissingListener#onStyleImageMissing(String)} listener interface.
   * </p>
   * <p>
   * By default, platform will remove unused images from the style. By adding listener, default
   * behavior can be overridden and client can control whether to release unused resources.
   * </p>
   *
   * @param listener The callback that's invoked when map needs to release unused image resources
   */
  public void addOnCanRemoveUnusedStyleImageListener(@NonNull OnCanRemoveUnusedStyleImageListener listener) {
    mapChangeReceiver.addOnCanRemoveUnusedStyleImageListener(listener);
  }

  /**
   * Removes a callback that's invoked when map needs to release unused image resources.
   * <p>
   * When all listeners are removed, platform will fallback to default behavior, which is to remove
   * unused images from the style.
   * </p>
   *
   * @param listener The callback that's invoked when map needs to release unused image resources
   */
  public void removeOnCanRemoveUnusedStyleImageListener(@NonNull OnCanRemoveUnusedStyleImageListener listener) {
    mapChangeReceiver.removeOnCanRemoveUnusedStyleImageListener(listener);
  }

  /**
   * Interface definition for a callback to be invoked when the camera will change.
   * <p>
   * {@link MapView#addOnCameraWillChangeListener(OnCameraWillChangeListener)}
   * </p>
   */
  public interface OnCameraWillChangeListener {

    /**
     * Called when the camera region will change.
     */
    void onCameraWillChange(boolean animated);
  }

  /**
   * Interface definition for a callback to be invoked when the camera is changing.
   * <p>
   * {@link MapView#addOnCameraIsChangingListener(OnCameraIsChangingListener)}
   * </p>
   */
  public interface OnCameraIsChangingListener {
    /**
     * Called when the camera is changing.
     */
    void onCameraIsChanging();
  }

  /**
   * Interface definition for a callback to be invoked when the map region did change.
   * <p>
   * {@link MapView#addOnCameraDidChangeListener(OnCameraDidChangeListener)}
   * </p>
   */
  public interface OnCameraDidChangeListener {
    /**
     * Called when the camera did change.
     */
    void onCameraDidChange(boolean animated);
  }

  /**
   * Interface definition for a callback to be invoked when the map will start loading.
   * <p>
   * {@link MapView#addOnWillStartLoadingMapListener(OnWillStartLoadingMapListener)}
   * </p>
   */
  public interface OnWillStartLoadingMapListener {
    /**
     * Called when the map will start loading.
     */
    void onWillStartLoadingMap();
  }

  /**
   * Interface definition for a callback to be invoked when the map finished loading.
   * <p>
   * {@link MapView#addOnDidFinishLoadingMapListener(OnDidFinishLoadingMapListener)}
   * </p>
   */
  public interface OnDidFinishLoadingMapListener {
    /**
     * Called when the map has finished loading.
     */
    void onDidFinishLoadingMap();
  }

  /**
   * Interface definition for a callback to be invoked when the map is changing.
   * <p>
   * {@link MapView#addOnDidFailLoadingMapListener(OnDidFailLoadingMapListener)}
   * </p>
   */
  public interface OnDidFailLoadingMapListener {
    /**
     * Called when the map failed to load.
     *
     * @param errorMessage The reason why the map failed to load
     */
    void onDidFailLoadingMap(String errorMessage);
  }

  /**
   * Interface definition for a callback to be invoked when the map will start rendering a frame.
   * <p>
   * {@link MapView#addOnWillStartRenderingFrameListener(OnWillStartRenderingFrameListener)}
   * </p>
   */
  public interface OnWillStartRenderingFrameListener {
    /**
     * Called when the map will start rendering a frame.
     */
    void onWillStartRenderingFrame();
  }

  /**
   * Interface definition for a callback to be invoked when the map finished rendering a frame.
   * <p>
   * {@link MapView#addOnDidFinishRenderingFrameListener(OnDidFinishRenderingFrameListener)}
   * </p>
   */
  public interface OnDidFinishRenderingFrameListener {
    /**
     * Called when the map has finished rendering a frame
     *
     * @param fully true if all frames have been rendered, false if partially rendered
     */
    void onDidFinishRenderingFrame(boolean fully);
  }

  /**
   * Interface definition for a callback to be invoked when the map will start rendering the map.
   * <p>
   * {@link MapView#addOnDidFailLoadingMapListener(OnDidFailLoadingMapListener)}
   * </p>
   */
  public interface OnWillStartRenderingMapListener {
    /**
     * Called when the map will start rendering.
     */
    void onWillStartRenderingMap();
  }

  /**
   * Interface definition for a callback to be invoked when the map is changing.
   * <p>
   * {@link MapView#addOnDidFinishRenderingMapListener(OnDidFinishRenderingMapListener)}
   * </p>
   */
  public interface OnDidFinishRenderingMapListener {
    /**
     * Called when the map has finished rendering.
     *
     * @param fully true if map is fully rendered, false if not fully rendered
     */
    void onDidFinishRenderingMap(boolean fully);
  }

  /**
   * Interface definition for a callback to be invoked when the map has entered the idle state.
   * <p>
   * Calling {@link MapboxMap#snapshot(MapboxMap.SnapshotReadyCallback)} from this callback
   * will result in recursive execution. Use {@link OnDidFinishRenderingFrameListener} instead.
   * </p>
   * <p>
   * {@link MapView#addOnDidBecomeIdleListener(OnDidBecomeIdleListener)}
   * </p>
   */
  public interface OnDidBecomeIdleListener {
    /**
     * Called when the map has entered the idle state.
     */
    void onDidBecomeIdle();
  }

  /**
   * Interface definition for a callback to be invoked when the map has loaded the style.
   * <p>
   * {@link MapView#addOnDidFailLoadingMapListener(OnDidFailLoadingMapListener)}
   * </p>
   */
  public interface OnDidFinishLoadingStyleListener {
    /**
     * Called when a style has finished loading.
     */
    void onDidFinishLoadingStyle();
  }

  /**
   * Interface definition for a callback to be invoked when a map source has changed.
   * <p>
   * {@link MapView#addOnDidFailLoadingMapListener(OnDidFailLoadingMapListener)}
   * </p>
   */
  public interface OnSourceChangedListener {
    /**
     * Called when a map source has changed.
     *
     * @param id the id of the source that has changed
     */
    void onSourceChangedListener(String id);
  }

  /**
   * Interface definition for a callback to be invoked with the id of a missing icon. The icon should be added
   * synchronously with {@link Style#addImage(String, Bitmap)} to be rendered on the current zoom level. When loading
   * icons asynchronously, you can load a placeholder image and replace it when you icon has loaded.
   * <p>
   * {@link MapView#addOnStyleImageMissingListener(OnStyleImageMissingListener)}
   * </p>
   */
  public interface OnStyleImageMissingListener {
    /**
     * Called when the map is missing an icon.The icon should be added synchronously with
     * {@link Style#addImage(String, Bitmap)} to be rendered on the current zoom level. When loading icons
     * asynchronously, you can load a placeholder image and replace it when you icon has loaded.
     *
     * @param id the id of the icon that is missing
     */
    void onStyleImageMissing(@NonNull String id);
  }

  /**
   * Interface definition for a callback to be invoked with an unused image identifier.
   * <p>
   * {@link MapView#addOnCanRemoveUnusedStyleImageListener(OnCanRemoveUnusedStyleImageListener)}
   * </p>
   */
  public interface OnCanRemoveUnusedStyleImageListener {
    /**
     * Called when the map needs to release unused image resources.
     *
     * @param id of an image that is not used by the map and can be removed from the style.
     * @return true if image can be removed, false otherwise.
     */
    boolean onCanRemoveUnusedStyleImage(@NonNull String id);
  }

  /**
   * Sets a callback object which will be triggered when the {@link MapboxMap} instance is ready to be used.
   *
   * @param callback The callback object that will be triggered when the map is ready to be used.
   */
  @UiThread
  public void getMapAsync(final @NonNull OnMapReadyCallback callback) {
    if (mapboxMap == null) {
      // Add callback to the list only if the style hasn't loaded, or the drawing surface isn't ready
      mapCallback.addOnMapReadyCallback(callback);
    } else {
      callback.onMapReady(mapboxMap);
    }
  }

  private boolean isGestureDetectorInitialized() {
    return mapGestureDetector != null;
  }

  @Nullable
  MapboxMap getMapboxMap() {
    return mapboxMap;
  }

  void setMapboxMap(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
  }

  private class FocalPointInvalidator implements FocalPointChangeListener {

    private final List<FocalPointChangeListener> focalPointChangeListeners = new ArrayList<>();

    void addListener(FocalPointChangeListener focalPointChangeListener) {
      focalPointChangeListeners.add(focalPointChangeListener);
    }

    @Override
    public void onFocalPointChanged(PointF pointF) {
      mapGestureDetector.setFocalPoint(pointF);
      for (FocalPointChangeListener focalPointChangeListener : focalPointChangeListeners) {
        focalPointChangeListener.onFocalPointChanged(pointF);
      }
    }
  }

  /**
   * The initial render callback waits for rendering to happen before making the map visible for end-users.
   * We wait for the second DID_FINISH_RENDERING_FRAME map change event as the first will still show a black surface.
   */
  private class InitialRenderCallback implements OnDidFinishRenderingFrameListener {

    private int renderCount;

    InitialRenderCallback() {
      addOnDidFinishRenderingFrameListener(this);
    }

    @Override
    public void onDidFinishRenderingFrame(boolean fully) {
      if (mapboxMap != null && mapboxMap.getStyle() != null && mapboxMap.getStyle().isFullyLoaded()) {
        renderCount++;
        if (renderCount == 3) {
          MapView.this.setForeground(null);
          removeOnDidFinishRenderingFrameListener(this);
        }
      }
    }

    private void onDestroy() {
      removeOnDidFinishRenderingFrameListener(this);
    }
  }

  private class GesturesManagerInteractionListener implements MapboxMap.OnGesturesManagerInteractionListener {

    @Override
    public void onAddMapClickListener(MapboxMap.OnMapClickListener listener) {
      mapGestureDetector.addOnMapClickListener(listener);
    }

    @Override
    public void onRemoveMapClickListener(MapboxMap.OnMapClickListener listener) {
      mapGestureDetector.removeOnMapClickListener(listener);
    }

    @Override
    public void onAddMapLongClickListener(MapboxMap.OnMapLongClickListener listener) {
      mapGestureDetector.addOnMapLongClickListener(listener);
    }

    @Override
    public void onRemoveMapLongClickListener(MapboxMap.OnMapLongClickListener listener) {
      mapGestureDetector.removeOnMapLongClickListener(listener);
    }

    @Override
    public void onAddFlingListener(MapboxMap.OnFlingListener listener) {
      mapGestureDetector.addOnFlingListener(listener);
    }

    @Override
    public void onRemoveFlingListener(MapboxMap.OnFlingListener listener) {
      mapGestureDetector.removeOnFlingListener(listener);
    }

    @Override
    public void onAddMoveListener(MapboxMap.OnMoveListener listener) {
      mapGestureDetector.addOnMoveListener(listener);
    }

    @Override
    public void onRemoveMoveListener(MapboxMap.OnMoveListener listener) {
      mapGestureDetector.removeOnMoveListener(listener);
    }

    @Override
    public void onAddRotateListener(MapboxMap.OnRotateListener listener) {
      mapGestureDetector.addOnRotateListener(listener);
    }

    @Override
    public void onRemoveRotateListener(MapboxMap.OnRotateListener listener) {
      mapGestureDetector.removeOnRotateListener(listener);
    }

    @Override
    public void onAddScaleListener(MapboxMap.OnScaleListener listener) {
      mapGestureDetector.addOnScaleListener(listener);
    }

    @Override
    public void onRemoveScaleListener(MapboxMap.OnScaleListener listener) {
      mapGestureDetector.removeOnScaleListener(listener);
    }

    @Override
    public void onAddShoveListener(MapboxMap.OnShoveListener listener) {
      mapGestureDetector.addShoveListener(listener);
    }

    @Override
    public void onRemoveShoveListener(MapboxMap.OnShoveListener listener) {
      mapGestureDetector.removeShoveListener(listener);
    }

    @Override
    public AndroidGesturesManager getGesturesManager() {
      return mapGestureDetector.getGesturesManager();
    }

    @Override
    public void setGesturesManager(AndroidGesturesManager gesturesManager, boolean attachDefaultListeners,
                                   boolean setDefaultMutuallyExclusives) {
      mapGestureDetector.setGesturesManager(
        getContext(), gesturesManager, attachDefaultListeners, setDefaultMutuallyExclusives);
    }

    @Override
    public void cancelAllVelocityAnimations() {
      mapGestureDetector.cancelAnimators();
    }
  }

  private class MapCallback implements OnDidFinishLoadingStyleListener,
    OnDidFinishRenderingFrameListener, OnDidFinishLoadingMapListener,
    OnCameraIsChangingListener, OnCameraDidChangeListener, OnDidFailLoadingMapListener {

    private final List<OnMapReadyCallback> onMapReadyCallbackList = new ArrayList<>();

    MapCallback() {
      addOnDidFinishLoadingStyleListener(this);
      addOnDidFinishRenderingFrameListener(this);
      addOnDidFinishLoadingMapListener(this);
      addOnCameraIsChangingListener(this);
      addOnCameraDidChangeListener(this);
      addOnDidFailLoadingMapListener(this);
    }

    void initialised() {
      mapboxMap.onPreMapReady();
      onMapReady();
      mapboxMap.onPostMapReady();
    }

    /**
     * Notify listeners, clear when done
     */
    private void onMapReady() {
      if (onMapReadyCallbackList.size() > 0) {
        Iterator<OnMapReadyCallback> iterator = onMapReadyCallbackList.iterator();
        while (iterator.hasNext()) {
          OnMapReadyCallback callback = iterator.next();
          if (callback != null) {
            // null checking required for #13279
            callback.onMapReady(mapboxMap);
          }
          iterator.remove();
        }
      }
    }

    void addOnMapReadyCallback(OnMapReadyCallback callback) {
      onMapReadyCallbackList.add(callback);
    }

    void onDestroy() {
      onMapReadyCallbackList.clear();
      removeOnDidFinishLoadingStyleListener(this);
      removeOnDidFinishRenderingFrameListener(this);
      removeOnDidFinishLoadingMapListener(this);
      removeOnCameraIsChangingListener(this);
      removeOnCameraDidChangeListener(this);
      removeOnDidFailLoadingMapListener(this);
    }

    @Override
    public void onDidFinishLoadingStyle() {
      if (mapboxMap != null) {
        mapboxMap.onFinishLoadingStyle();
      }
    }

    @Override
    public void onDidFailLoadingMap(String errorMessage) {
      if (mapboxMap != null) {
        mapboxMap.onFailLoadingStyle();
      }
    }

    @Override
    public void onDidFinishRenderingFrame(boolean fully) {
      if (mapboxMap != null) {
        mapboxMap.onUpdateFullyRendered();
      }
    }

    @Override
    public void onDidFinishLoadingMap() {
      if (mapboxMap != null) {
        mapboxMap.onUpdateRegionChange();
      }
    }

    @Override
    public void onCameraIsChanging() {
      if (mapboxMap != null) {
        mapboxMap.onUpdateRegionChange();
      }
    }

    @Override
    public void onCameraDidChange(boolean animated) {
      if (mapboxMap != null) {
        mapboxMap.onUpdateRegionChange();
      }
    }
  }

  /**
   * Click event hook for providing a custom attribution dialog manager.
   */
  private static class AttributionClickListener implements OnClickListener {

    @NonNull
    private final AttributionDialogManager defaultDialogManager;
    private UiSettings uiSettings;

    private AttributionClickListener(@NonNull Context context, @NonNull MapboxMap mapboxMap) {
      this.defaultDialogManager = new AttributionDialogManager(context, mapboxMap);
      this.uiSettings = mapboxMap.getUiSettings();
    }

    @Override
    public void onClick(View v) {
      getDialogManager().onClick(v);
    }

    public void onStop() {
      getDialogManager().onStop();
    }

    private AttributionDialogManager getDialogManager() {
      AttributionDialogManager customDialogManager = uiSettings.getAttributionDialogManager();
      if (customDialogManager != null) {
        return uiSettings.getAttributionDialogManager();
      } else {
        return defaultDialogManager;
      }
    }
  }

  /**
   * Sets the strict mode that will throw the {@link com.mapbox.mapboxsdk.MapStrictModeException}
   * whenever the map would fail silently otherwise.
   *
   * @param strictModeEnabled true to enable the strict mode, false otherwise
   */
  public static void setMapStrictModeEnabled(boolean strictModeEnabled) {
    MapStrictMode.setStrictModeEnabled(strictModeEnabled);
  }
}
