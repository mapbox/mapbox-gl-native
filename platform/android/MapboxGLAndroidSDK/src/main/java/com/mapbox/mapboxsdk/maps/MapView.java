package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.drawable.ColorDrawable;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.annotation.CallSuper;
import android.support.annotation.IntDef;
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
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ZoomButtonsController;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationComponent;
import com.mapbox.mapboxsdk.maps.renderer.MapRenderer;
import com.mapbox.mapboxsdk.maps.renderer.glsurfaceview.GLSurfaceViewMapRenderer;
import com.mapbox.mapboxsdk.maps.renderer.textureview.TextureViewMapRenderer;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.mapboxsdk.offline.OfflineGeometryRegionDefinition;
import com.mapbox.mapboxsdk.offline.OfflineRegionDefinition;
import com.mapbox.mapboxsdk.offline.OfflineTilePyramidRegionDefinition;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

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

  private final CopyOnWriteArrayList<OnMapChangedListener> onMapChangedListeners = new CopyOnWriteArrayList<>();
  private final MapChangeReceiver mapChangeReceiver = new MapChangeReceiver();
  private final MapCallback mapCallback = new MapCallback();
  private final InitialRenderCallback initialRenderCallback = new InitialRenderCallback();

  private NativeMapView nativeMapView;
  private MapboxMap mapboxMap;
  private MapboxMapOptions mapboxMapOptions;
  private MapRenderer mapRenderer;
  private boolean destroyed;
  private boolean hasSurface;

  private CompassView compassView;
  private PointF focalPoint;
  private ImageView attrView;
  private ImageView logoView;

  private MapGestureDetector mapGestureDetector;
  private MapKeyListener mapKeyListener;
  private MapZoomButtonController mapZoomButtonController;
  private Bundle savedInstanceState;

  @UiThread
  public MapView(@NonNull Context context) {
    super(context);
    initialize(context, MapboxMapOptions.createFromAttributes(context, null));
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
    initialize(context, options == null ? MapboxMapOptions.createFromAttributes(context, null) : options);
  }

  @CallSuper
  @UiThread
  protected void initialize(@NonNull final Context context, @NonNull final MapboxMapOptions options) {
    if (isInEditMode()) {
      // in IDE layout editor, just return
      return;
    }

    // hide surface until map is fully loaded #10990
    setForeground(new ColorDrawable(options.getForegroundLoadColor()));

    mapboxMapOptions = options;

    // inflate view
    View view = LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_internal, this);
    compassView = (CompassView) view.findViewById(R.id.compassView);
    attrView = (ImageView) view.findViewById(R.id.attributionView);
    logoView = (ImageView) view.findViewById(R.id.logoView);

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
    final CameraChangeDispatcher cameraChangeDispatcher = new CameraChangeDispatcher();

    // setup components for MapboxMap creation
    Projection proj = new Projection(nativeMapView);
    UiSettings uiSettings = new UiSettings(proj, focalInvalidator, compassView, attrView, logoView, getPixelRatio());
    LongSparseArray<Annotation> annotationsArray = new LongSparseArray<>();
    MarkerViewManager markerViewManager = new MarkerViewManager((ViewGroup) findViewById(R.id.markerViewContainer));
    IconManager iconManager = new IconManager(nativeMapView);
    Annotations annotations = new AnnotationContainer(nativeMapView, annotationsArray);
    Markers markers = new MarkerContainer(nativeMapView, this, annotationsArray, iconManager, markerViewManager);
    Polygons polygons = new PolygonContainer(nativeMapView, annotationsArray);
    Polylines polylines = new PolylineContainer(nativeMapView, annotationsArray);
    ShapeAnnotations shapeAnnotations = new ShapeAnnotationContainer(nativeMapView, annotationsArray);
    AnnotationManager annotationManager = new AnnotationManager(nativeMapView, this, annotationsArray,
      markerViewManager, iconManager, annotations, markers, polygons, polylines, shapeAnnotations);
    Transform transform = new Transform(nativeMapView, annotationManager.getMarkerViewManager(),
      cameraChangeDispatcher);

    // MapboxMap
    mapboxMap = new MapboxMap(nativeMapView, transform, uiSettings, proj, registerTouchListener,
      annotationManager, cameraChangeDispatcher);

    // user input
    mapGestureDetector = new MapGestureDetector(context, transform, proj, uiSettings,
      annotationManager, cameraChangeDispatcher);
    mapKeyListener = new MapKeyListener(transform, uiSettings, mapGestureDetector);

    // overlain zoom buttons
    mapZoomButtonController = new MapZoomButtonController(new ZoomButtonsController(this));
    MapZoomControllerListener zoomListener = new MapZoomControllerListener(
      mapGestureDetector, cameraChangeDispatcher, getWidth(), getHeight());
    mapZoomButtonController.bind(uiSettings, zoomListener);

    // compass
    compassView.injectCompassAnimationListener(createCompassAnimationListener(cameraChangeDispatcher));
    compassView.setOnClickListener(createCompassClickListener(cameraChangeDispatcher));

    // LocationComponent
    mapboxMap.injectLocationComponent(new LocationComponent(mapboxMap));

    // inject widgets with MapboxMap
    attrView.setOnClickListener(new AttributionClickListener(context, mapboxMap));

    // Ensure this view is interactable
    setClickable(true);
    setLongClickable(true);
    setFocusable(true);
    setFocusableInTouchMode(true);
    requestDisallowInterceptTouchEvent(true);

    // notify Map object about current connectivity state
    nativeMapView.setReachability(ConnectivityReceiver.instance(context).isConnected(context));

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

  private MapboxMap.OnCompassAnimationListener createCompassAnimationListener(final CameraChangeDispatcher
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

  private OnClickListener createCompassClickListener(final CameraChangeDispatcher cameraChangeDispatcher) {
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
      GLSurfaceView glSurfaceView = new GLSurfaceView(getContext());
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

    // deprecated API
    nativeMapView.addOnMapChangedListener(new OnMapChangedListener() {
      @Override
      public void onMapChanged(int change) {
        // dispatch events to external listeners
        if (!onMapChangedListeners.isEmpty()) {
          for (OnMapChangedListener onMapChangedListener : onMapChangedListeners) {
            onMapChangedListener.onMapChanged(change);
          }
        }
      }
    });
  }

  private void onSurfaceCreated() {
    hasSurface = true;
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
    ConnectivityReceiver.instance(getContext()).activate();
    FileSource.getInstance(getContext()).activate();
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
    if (mapboxMap != null) {
      // map was destroyed before it was started
      mapGestureDetector.cancelAnimators();
      mapboxMap.onStop();
    }

    if (mapRenderer != null) {
      mapRenderer.onStop();
    }

    ConnectivityReceiver.instance(getContext()).deactivate();
    FileSource.getInstance(getContext()).deactivate();
  }

  /**
   * You must call this method from the parent's Activity#onDestroy() or Fragment#onDestroyView().
   */
  @UiThread
  public void onDestroy() {
    destroyed = true;
    mapChangeReceiver.clear();
    onMapChangedListeners.clear();
    mapCallback.onDestroy();
    initialRenderCallback.onDestroy();

    if (mapboxMap != null) {
      mapboxMap.onDestroy();
    }

    if (nativeMapView != null && hasSurface) {
      // null when destroying an activity programmatically mapbox-navigation-android/issues/503
      nativeMapView.destroy();
      nativeMapView = null;
    }

    if (mapRenderer != null) {
      mapRenderer.onDestroy();
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
    if (!isZoomButtonControllerInitialized() || !isGestureDetectorInitialized()) {
      return super.onTouchEvent(event);
    }

    if (event.getAction() == MotionEvent.ACTION_DOWN) {
      mapZoomButtonController.setVisible(true);
    }
    return mapGestureDetector.onTouchEvent(event) || super.onTouchEvent(event);
  }

  @Override
  public boolean onKeyDown(int keyCode, KeyEvent event) {
    return mapKeyListener.onKeyDown(keyCode, event) || super.onKeyDown(keyCode, event);
  }

  @Override
  public boolean onKeyLongPress(int keyCode, KeyEvent event) {
    return mapKeyListener.onKeyLongPress(keyCode, event) || super.onKeyLongPress(keyCode, event);
  }

  @Override
  public boolean onKeyUp(int keyCode, KeyEvent event) {
    return mapKeyListener.onKeyUp(keyCode, event) || super.onKeyUp(keyCode, event);
  }

  @Override
  public boolean onTrackballEvent(MotionEvent event) {
    return mapKeyListener.onTrackballEvent(event) || super.onTrackballEvent(event);
  }

  @Override
  public boolean onGenericMotionEvent(MotionEvent event) {
    if (!isGestureDetectorInitialized()) {
      return super.onGenericMotionEvent(event);
    }
    return mapGestureDetector.onGenericMotionEvent(event) || super.onGenericMotionEvent(event);
  }

  @Override
  public boolean onHoverEvent(MotionEvent event) {
    if (!isZoomButtonControllerInitialized()) {
      return super.onHoverEvent(event);
    }

    switch (event.getActionMasked()) {
      case MotionEvent.ACTION_HOVER_ENTER:
      case MotionEvent.ACTION_HOVER_MOVE:
        mapZoomButtonController.setVisible(true);
        return true;

      case MotionEvent.ACTION_HOVER_EXIT:
        mapZoomButtonController.setVisible(false);
        return true;

      default:
        // We are not interested in this event
        return false;
    }
  }

  /**
   * You must call this method from the parent's Activity#onLowMemory() or Fragment#onLowMemory().
   */
  @UiThread
  public void onLowMemory() {
    if (nativeMapView != null) {
      nativeMapView.onLowMemory();
    }
  }

  /**
   * <p>
   * Loads a new map style from the specified URL.
   * </p>
   * {@code url} can take the following forms:
   * <ul>
   * <li>{@code Style.*}: load one of the bundled styles in {@link Style}.</li>
   * <li>{@code mapbox://styles/<user>/<style>}:
   * retrieves the style from a <a href="https://www.mapbox.com/account/">Mapbox account.</a>
   * {@code user} is your username. {@code style} is the ID of your custom
   * style created in <a href="https://www.mapbox.com/studio">Mapbox Studio</a>.</li>
   * <li>{@code http://...} or {@code https://...}:
   * retrieves the style over the Internet from any web server.</li>
   * <li>{@code asset://...}:
   * reads the style from the APK {@code assets/} directory.
   * This is used to load a style bundled with your app.</li>
   * <li>{@code null}: loads the default {@link Style#MAPBOX_STREETS} style.</li>
   * </ul>
   * <p>
   * This method is asynchronous and will return immediately before the style finishes loading.
   * If you wish to wait for the map to finish loading listen for the {@link MapView#DID_FINISH_LOADING_MAP} event.
   * </p>
   * If the style fails to load or an invalid style URL is set, the map view will become blank.
   * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be sent.
   *
   * @param url The URL of the map style
   * @see Style
   */
  public void setStyleUrl(@NonNull String url) {
    if (nativeMapView != null) {
      // null-checking the nativeMapView as it can be mistakenly called after it's been destroyed
      nativeMapView.setStyleUrl(url);
    }
  }

  /**
   * Loads a new style from the specified offline region definition and moves the map camera to that region.
   *
   * @param definition the offline region definition
   * @see OfflineRegionDefinition
   */
  public void setOfflineRegionDefinition(OfflineRegionDefinition definition) {
    if (definition instanceof OfflineTilePyramidRegionDefinition) {
      setOfflineTilePyramidRegionDefinition((OfflineTilePyramidRegionDefinition) definition);
    } else if (definition instanceof OfflineGeometryRegionDefinition) {
      setOfflineGeometryRegionDefinition((OfflineGeometryRegionDefinition) definition);
    } else {
      throw new UnsupportedOperationException("OfflineRegionDefintion instance not supported");
    }
  }

  private void setOfflineRegionDefinition(String styleUrl, LatLng cameraTarget, double minZoom, double maxZoom) {
    CameraPosition cameraPosition = new CameraPosition.Builder()
      .target(cameraTarget)
      .zoom(minZoom)
      .build();
    setStyleUrl(styleUrl);
    if (mapboxMap != null) {
      mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
      mapboxMap.setMinZoomPreference(minZoom);
      mapboxMap.setMaxZoomPreference(maxZoom);
    } else {
      mapboxMapOptions.camera(cameraPosition);
      mapboxMapOptions.minZoomPreference(minZoom);
      mapboxMapOptions.maxZoomPreference(maxZoom);
    }
  }

  private void setOfflineTilePyramidRegionDefinition(OfflineTilePyramidRegionDefinition regionDefinition) {
    setOfflineRegionDefinition(regionDefinition.getStyleURL(),
      regionDefinition.getBounds().getCenter(),
      regionDefinition.getMinZoom(),
      regionDefinition.getMaxZoom()
    );
  }

  private void setOfflineGeometryRegionDefinition(OfflineGeometryRegionDefinition regionDefinition) {
    setOfflineRegionDefinition(regionDefinition.getStyleURL(),
      regionDefinition.getBounds().getCenter(),
      regionDefinition.getMinZoom(),
      regionDefinition.getMaxZoom()
    );
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
  // View events
  //

  // Called when view is no longer connected
  @Override
  @CallSuper
  protected void onDetachedFromWindow() {
    super.onDetachedFromWindow();
    if (isZoomButtonControllerInitialized()) {
      mapZoomButtonController.setVisible(false);
    }
  }

  // Called when view is hidden and shown
  @Override
  protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
    if (isInEditMode()) {
      return;
    }

    if (isZoomButtonControllerInitialized()) {
      mapZoomButtonController.setVisible(visibility == View.VISIBLE);
    }
  }

  //
  // ViewCallback
  //

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
  public void addOnCameraWillChangeListener(OnCameraWillChangeListener listener) {
    mapChangeReceiver.addOnCameraWillChangeListener(listener);
  }

  /**
   * Remove a callback that's invoked when the camera region will change.
   *
   * @param listener The callback that's invoked when the camera region will change
   */
  public void removeOnCameraWillChangeListener(OnCameraWillChangeListener listener) {
    mapChangeReceiver.removeOnCameraWillChangeListener(listener);
  }

  /**
   * Set a callback that's invoked when the camera is changing.
   *
   * @param listener The callback that's invoked when the camera is changing
   */
  public void addOnCameraIsChangingListener(OnCameraIsChangingListener listener) {
    mapChangeReceiver.addOnCameraIsChangingListener(listener);
  }

  /**
   * Remove a callback that's invoked when the camera is changing.
   *
   * @param listener The callback that's invoked when the camera is changing
   */
  public void removeOnCameraIsChangingListener(OnCameraIsChangingListener listener) {
    mapChangeReceiver.removeOnCameraIsChangingListener(listener);
  }

  /**
   * Set a callback that's invoked when the camera region did change.
   *
   * @param listener The callback that's invoked when the camera region did change
   */
  public void addOnCameraDidChangeListener(OnCameraDidChangeListener listener) {
    mapChangeReceiver.addOnCameraDidChangeListener(listener);
  }

  /**
   * Set a callback that's invoked when the camera region did change.
   *
   * @param listener The callback that's invoked when the camera region did change
   */
  public void removeOnCameraDidChangeListener(OnCameraDidChangeListener listener) {
    mapChangeReceiver.removeOnCameraDidChangeListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start loading.
   *
   * @param listener The callback that's invoked when the map will start loading
   */
  public void addOnWillStartLoadingMapListener(OnWillStartLoadingMapListener listener) {
    mapChangeReceiver.addOnWillStartLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start loading.
   *
   * @param listener The callback that's invoked when the map will start loading
   */
  public void removeOnWillStartLoadingMapListener(OnWillStartLoadingMapListener listener) {
    mapChangeReceiver.removeOnWillStartLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished loading.
   *
   * @param listener The callback that's invoked when the map has finished loading
   */
  public void addOnDidFinishLoadingMapListener(OnDidFinishLoadingMapListener listener) {
    mapChangeReceiver.addOnDidFinishLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished loading.
   *
   * @param listener The callback that's invoked when the map has finished loading
   */
  public void removeOnDidFinishLoadingMapListener(OnDidFinishLoadingMapListener listener) {
    mapChangeReceiver.removeOnDidFinishLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map failed to load.
   *
   * @param listener The callback that's invoked when the map failed to load
   */
  public void addOnDidFailLoadingMapListener(OnDidFailLoadingMapListener listener) {
    mapChangeReceiver.addOnDidFailLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map failed to load.
   *
   * @param listener The callback that's invoked when the map failed to load
   */
  public void removeOnDidFailLoadingMapListener(OnDidFailLoadingMapListener listener) {
    mapChangeReceiver.removeOnDidFailLoadingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start rendering a frame.
   *
   * @param listener The callback that's invoked when the camera will start rendering a frame
   */
  public void addOnWillStartRenderingFrameListener(OnWillStartRenderingFrameListener listener) {
    mapChangeReceiver.addOnWillStartRenderingFrameListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start rendering a frame.
   *
   * @param listener The callback that's invoked when the camera will start rendering a frame
   */
  public void removeOnWillStartRenderingFrameListener(OnWillStartRenderingFrameListener listener) {
    mapChangeReceiver.removeOnWillStartRenderingFrameListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished rendering a frame.
   *
   * @param listener The callback that's invoked when the map has finished rendering a frame
   */
  public void addOnDidFinishRenderingFrameListener(OnDidFinishRenderingFrameListener listener) {
    mapChangeReceiver.addOnDidFinishRenderingFrameListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished rendering a frame.
   *
   * @param listener The callback that's invoked when the map has finished rendering a frame
   */
  public void removeOnDidFinishRenderingFrameListener(OnDidFinishRenderingFrameListener listener) {
    mapChangeReceiver.removeOnDidFinishRenderingFrameListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start rendering.
   *
   * @param listener The callback that's invoked when the map will start rendering
   */
  public void addOnWillStartRenderingMapListener(OnWillStartRenderingMapListener listener) {
    mapChangeReceiver.addOnWillStartRenderingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map will start rendering.
   *
   * @param listener The callback that's invoked when the map will start rendering
   */
  public void removeOnWillStartRenderingMapListener(OnWillStartRenderingMapListener listener) {
    mapChangeReceiver.removeOnWillStartRenderingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the map has finished rendering.
   *
   * @param listener The callback that's invoked when the map has finished rendering
   */
  public void addOnDidFinishRenderingMapListener(OnDidFinishRenderingMapListener listener) {
    mapChangeReceiver.addOnDidFinishRenderingMapListener(listener);
  }

  /**
   * Remove a callback that's invoked when the map has finished rendering.
   *
   * @param listener The callback that's invoked when the map has finished rendering
   */
  public void removeOnDidFinishRenderingMapListener(OnDidFinishRenderingMapListener listener) {
    mapChangeReceiver.removeOnDidFinishRenderingMapListener(listener);
  }

  /**
   * Set a callback that's invoked when the style has finished loading.
   *
   * @param listener The callback that's invoked when the style has finished loading
   */
  public void addOnDidFinishLoadingStyleListener(OnDidFinishLoadingStyleListener listener) {
    mapChangeReceiver.addOnDidFinishLoadingStyleListener(listener);
  }

  /**
   * Set a callback that's invoked when the style has finished loading.
   *
   * @param listener The callback that's invoked when the style has finished loading
   */
  public void removeOnDidFinishLoadingStyleListener(OnDidFinishLoadingStyleListener listener) {
    mapChangeReceiver.removeOnDidFinishLoadingStyleListener(listener);
  }

  /**
   * Set a callback that's invoked when a map source has changed.
   *
   * @param listener The callback that's invoked when the source has changed
   */
  public void addOnSourceChangedListener(OnSourceChangedListener listener) {
    mapChangeReceiver.addOnSourceChangedListener(listener);
  }

  /**
   * Set a callback that's invoked when a map source has changed.
   *
   * @param listener The callback that's invoked when the source has changed
   */
  public void removeOnSourceChangedListener(OnSourceChangedListener listener) {
    mapChangeReceiver.removeOnSourceChangedListener(listener);
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
     * @param fully true if map is fully rendered, false if fully rendered
     */
    void onDidFinishRenderingMap(boolean fully);
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
   * <p>
   * Add a callback that's invoked when the displayed map view changes.
   * </p>
   * To remove the callback, use {@link MapView#removeOnMapChangedListener(OnMapChangedListener)}.
   *
   * @param listener The callback that's invoked on every frame rendered to the map view.
   * @see MapView#removeOnMapChangedListener(OnMapChangedListener)
   * @deprecated use specific map change callbacks instead
   */
  @Deprecated
  public void addOnMapChangedListener(@NonNull OnMapChangedListener listener) {
    onMapChangedListeners.add(listener);
  }

  /**
   * Remove a callback added with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}
   *
   * @param listener The previously added callback to remove.
   * @see MapView#addOnMapChangedListener(OnMapChangedListener)
   * @deprecated use specific map change callbacks instead
   */
  @Deprecated
  public void removeOnMapChangedListener(@NonNull OnMapChangedListener listener) {
    onMapChangedListeners.remove(listener);
  }

  /**
   * Sets a callback object which will be triggered when the {@link MapboxMap} instance is ready to be used.
   *
   * @param callback The callback object that will be triggered when the map is ready to be used.
   */
  @UiThread
  public void getMapAsync(final @NonNull OnMapReadyCallback callback) {
    if (mapCallback.isInitialLoad() || mapboxMap == null) {
      // Add callback to the list only if the style hasn't loaded, or the drawing surface isn't ready
      mapCallback.addOnMapReadyCallback(callback);
    } else {
      callback.onMapReady(mapboxMap);
    }
  }

  private boolean isZoomButtonControllerInitialized() {
    return mapZoomButtonController != null;
  }

  private boolean isGestureDetectorInitialized() {
    return mapGestureDetector != null;
  }

  MapboxMap getMapboxMap() {
    return mapboxMap;
  }

  void setMapboxMap(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
  }

  /**
   * Definition of a map change event.
   *
   * @see MapView.OnMapChangedListener#onMapChanged(int)
   */
  @IntDef( {REGION_WILL_CHANGE,
    REGION_WILL_CHANGE_ANIMATED,
    REGION_IS_CHANGING,
    REGION_DID_CHANGE,
    REGION_DID_CHANGE_ANIMATED,
    WILL_START_LOADING_MAP,
    DID_FINISH_LOADING_MAP,
    DID_FAIL_LOADING_MAP,
    WILL_START_RENDERING_FRAME,
    DID_FINISH_RENDERING_FRAME,
    DID_FINISH_RENDERING_FRAME_FULLY_RENDERED,
    WILL_START_RENDERING_MAP,
    DID_FINISH_RENDERING_MAP,
    DID_FINISH_RENDERING_MAP_FULLY_RENDERED,
    DID_FINISH_LOADING_STYLE,
    SOURCE_DID_CHANGE
  })
  @Retention(RetentionPolicy.SOURCE)
  public @interface MapChange {
  }

  /**
   * This event is triggered whenever the currently displayed map region is about to changing
   * without an animation.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int REGION_WILL_CHANGE = 0;

  /**
   * This event is triggered whenever the currently displayed map region is about to changing
   * with an animation.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int REGION_WILL_CHANGE_ANIMATED = 1;

  /**
   * This event is triggered whenever the currently displayed map region is changing.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int REGION_IS_CHANGING = 2;

  /**
   * This event is triggered whenever the currently displayed map region finished changing
   * without an animation.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int REGION_DID_CHANGE = 3;

  /**
   * This event is triggered whenever the currently displayed map region finished changing
   * with an animation.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int REGION_DID_CHANGE_ANIMATED = 4;

  /**
   * This event is triggered when the map is about to start loading a new map style.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int WILL_START_LOADING_MAP = 5;

  /**
   * This  is triggered when the map has successfully loaded a new map style.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int DID_FINISH_LOADING_MAP = 6;

  /**
   * This event is triggered when the map has failed to load a new map style.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int DID_FAIL_LOADING_MAP = 7;

  /**
   * This event is triggered when the map will start rendering a frame.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int WILL_START_RENDERING_FRAME = 8;

  /**
   * This event is triggered when the map finished rendering a frame.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int DID_FINISH_RENDERING_FRAME = 9;

  /**
   * This event is triggered when the map finished rendering the frame fully.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int DID_FINISH_RENDERING_FRAME_FULLY_RENDERED = 10;

  /**
   * This event is triggered when the map will start rendering the map.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int WILL_START_RENDERING_MAP = 11;

  /**
   * This event is triggered when the map finished rendering the map.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int DID_FINISH_RENDERING_MAP = 12;

  /**
   * This event is triggered when the map is fully rendered.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int DID_FINISH_RENDERING_MAP_FULLY_RENDERED = 13;

  /**
   * This {@link MapChange} is triggered when a style has finished loading.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int DID_FINISH_LOADING_STYLE = 14;

  /**
   * This {@link MapChange} is triggered when a source changes.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapChange
   * @see MapView.OnMapChangedListener
   */
  public static final int SOURCE_DID_CHANGE = 15;

  /**
   * Interface definition for a callback to be invoked when the displayed map view changes.
   * <p>
   * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
   * </p>
   *
   * @see MapView#addOnMapChangedListener(OnMapChangedListener)
   * @see MapView.MapChange
   * @deprecated use specific map change callbacks instead
   */
  @Deprecated
  public interface OnMapChangedListener {
    /**
     * Called when the displayed map view changes.
     *
     * @param change Type of map change event, one of {@link #REGION_WILL_CHANGE},
     *               {@link #REGION_WILL_CHANGE_ANIMATED},
     *               {@link #REGION_IS_CHANGING},
     *               {@link #REGION_DID_CHANGE},
     *               {@link #REGION_DID_CHANGE_ANIMATED},
     *               {@link #WILL_START_LOADING_MAP},
     *               {@link #DID_FAIL_LOADING_MAP},
     *               {@link #DID_FINISH_LOADING_MAP},
     *               {@link #WILL_START_RENDERING_FRAME},
     *               {@link #DID_FINISH_RENDERING_FRAME},
     *               {@link #DID_FINISH_RENDERING_FRAME_FULLY_RENDERED},
     *               {@link #WILL_START_RENDERING_MAP},
     *               {@link #DID_FINISH_RENDERING_MAP},
     *               {@link #DID_FINISH_RENDERING_MAP_FULLY_RENDERED}.
     *               {@link #DID_FINISH_LOADING_STYLE},
     *               {@link #SOURCE_DID_CHANGE}.
     */
    void onMapChanged(@MapChange int change);
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
  private class InitialRenderCallback implements OnDidFinishLoadingStyleListener, OnDidFinishRenderingFrameListener {

    private int renderCount;
    private boolean styleLoaded;

    InitialRenderCallback() {
      addOnDidFinishLoadingStyleListener(this);
      addOnDidFinishRenderingFrameListener(this);
    }

    @Override
    public void onDidFinishLoadingStyle() {
      styleLoaded = true;
    }

    @Override
    public void onDidFinishRenderingFrame(boolean fully) {
      if (styleLoaded) {
        renderCount++;
        if (renderCount == 2) {
          MapView.this.setForeground(null);
          removeOnDidFinishRenderingFrameListener(this);
        }
      }
    }

    private void onDestroy() {
      removeOnDidFinishLoadingStyleListener(this);
      removeOnDidFinishRenderingFrameListener(this);
    }
  }

  private class GesturesManagerInteractionListener implements MapboxMap.OnGesturesManagerInteractionListener {

    @Override
    public void onSetMapClickListener(MapboxMap.OnMapClickListener listener) {
      mapGestureDetector.setOnMapClickListener(listener);
    }

    @Override
    public void onAddMapClickListener(MapboxMap.OnMapClickListener listener) {
      mapGestureDetector.addOnMapClickListener(listener);
    }

    @Override
    public void onRemoveMapClickListener(MapboxMap.OnMapClickListener listener) {
      mapGestureDetector.removeOnMapClickListener(listener);
    }

    @Override
    public void onSetMapLongClickListener(MapboxMap.OnMapLongClickListener listener) {
      mapGestureDetector.setOnMapLongClickListener(listener);
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
    public void onSetScrollListener(MapboxMap.OnScrollListener listener) {
      mapGestureDetector.setOnScrollListener(listener);
    }

    @Override
    public void onAddScrollListener(MapboxMap.OnScrollListener listener) {
      mapGestureDetector.addOnScrollListener(listener);
    }

    @Override
    public void onRemoveScrollListener(MapboxMap.OnScrollListener listener) {
      mapGestureDetector.removeOnScrollListener(listener);
    }

    @Override
    public void onSetFlingListener(MapboxMap.OnFlingListener listener) {
      mapGestureDetector.setOnFlingListener(listener);
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

  private static class MapZoomControllerListener implements ZoomButtonsController.OnZoomListener {

    private final MapGestureDetector mapGestureDetector;
    private final CameraChangeDispatcher cameraChangeDispatcher;
    private final float mapWidth;
    private final float mapHeight;

    MapZoomControllerListener(MapGestureDetector detector, CameraChangeDispatcher dispatcher,
                              float mapWidth, float mapHeight) {
      this.mapGestureDetector = detector;
      this.cameraChangeDispatcher = dispatcher;
      this.mapWidth = mapWidth;
      this.mapHeight = mapHeight;
    }

    // Not used
    @Override
    public void onVisibilityChanged(boolean visible) {
      // Ignore
    }

    // Called when user pushes a zoom button on the ZoomButtonController
    @Override
    public void onZoom(boolean zoomIn) {
      cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_ANIMATION);
      onZoom(zoomIn, mapGestureDetector.getFocalPoint());
    }

    private void onZoom(boolean zoomIn, @Nullable PointF focalPoint) {
      if (focalPoint == null) {
        focalPoint = new PointF(mapWidth / 2, mapHeight / 2);
      }
      if (zoomIn) {
        mapGestureDetector.zoomInAnimated(focalPoint, true);
      } else {
        mapGestureDetector.zoomOutAnimated(focalPoint, true);
      }
    }
  }

  private class MapCallback implements OnWillStartLoadingMapListener, OnDidFinishLoadingStyleListener,
    OnDidFinishRenderingFrameListener, OnDidFinishLoadingMapListener,
    OnCameraIsChangingListener, OnCameraDidChangeListener {

    private final List<OnMapReadyCallback> onMapReadyCallbackList = new ArrayList<>();
    private boolean initialLoad = true;

    MapCallback() {
      addOnWillStartLoadingMapListener(this);
      addOnDidFinishLoadingStyleListener(this);
      addOnDidFinishRenderingFrameListener(this);
      addOnDidFinishLoadingMapListener(this);
      addOnCameraIsChangingListener(this);
      addOnCameraDidChangeListener(this);
    }

    void initialised() {
      if (!initialLoad) {
        // Style has loaded before the drawing surface has been initialized, delivering OnMapReady
        mapboxMap.onPreMapReady();
        onMapReady();
        mapboxMap.onPostMapReady();
      }
    }

    private void onMapReady() {
      if (onMapReadyCallbackList.size() > 0) {
        // Notify listeners, clear when done
        Iterator<OnMapReadyCallback> iterator = onMapReadyCallbackList.iterator();
        while (iterator.hasNext()) {
          OnMapReadyCallback callback = iterator.next();
          callback.onMapReady(mapboxMap);
          iterator.remove();
        }
      }
    }

    boolean isInitialLoad() {
      return initialLoad;
    }

    void addOnMapReadyCallback(OnMapReadyCallback callback) {
      onMapReadyCallbackList.add(callback);
    }

    void onDestroy() {
      onMapReadyCallbackList.clear();
      removeOnWillStartLoadingMapListener(this);
      removeOnDidFinishLoadingStyleListener(this);
      removeOnDidFinishRenderingFrameListener(this);
      removeOnDidFinishLoadingMapListener(this);
      removeOnCameraIsChangingListener(this);
      removeOnCameraDidChangeListener(this);
    }

    @Override
    public void onWillStartLoadingMap() {
      if (mapboxMap != null && !initialLoad) {
        mapboxMap.onStartLoadingMap();
      }
    }

    @Override
    public void onDidFinishLoadingStyle() {
      if (mapboxMap != null) {
        if (initialLoad) {
          mapboxMap.onPreMapReady();
          onMapReady();
          mapboxMap.onPostMapReady();
        } else {
          mapboxMap.onFinishLoadingStyle();
        }
      }
      initialLoad = false;
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

    private final AttributionDialogManager defaultDialogManager;
    private UiSettings uiSettings;

    private AttributionClickListener(Context context, MapboxMap mapboxMap) {
      this.defaultDialogManager = new AttributionDialogManager(context, mapboxMap);
      this.uiSettings = mapboxMap.getUiSettings();
    }

    @Override
    public void onClick(View v) {
      AttributionDialogManager customDialogManager = uiSettings.getAttributionDialogManager();
      if (customDialogManager != null) {
        uiSettings.getAttributionDialogManager().onClick(v);
      } else {
        defaultDialogManager.onClick(v);
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