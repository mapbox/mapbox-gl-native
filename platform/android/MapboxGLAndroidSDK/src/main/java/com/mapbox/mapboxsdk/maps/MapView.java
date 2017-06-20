package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.CallSuper;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ZoomButtonsController;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.services.android.telemetry.MapboxTelemetry;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

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
public class MapView extends FrameLayout {

  private NativeMapView nativeMapView;
  private boolean textureMode;
  private boolean destroyed;
  private boolean hasSurface;

  private MapboxMap mapboxMap;
  private MapCallback mapCallback;

  private MapGestureDetector mapGestureDetector;
  private MapKeyListener mapKeyListener;
  private MapZoomButtonController mapZoomButtonController;

  @UiThread
  public MapView(@NonNull Context context) {
    super(context);
    initialise(context, MapboxMapOptions.createFromAttributes(context, null));
  }

  @UiThread
  public MapView(@NonNull Context context, @Nullable AttributeSet attrs) {
    super(context, attrs);
    initialise(context, MapboxMapOptions.createFromAttributes(context, attrs));
  }

  @UiThread
  public MapView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
    super(context, attrs, defStyleAttr);
    initialise(context, MapboxMapOptions.createFromAttributes(context, attrs));
  }

  @UiThread
  public MapView(@NonNull Context context, @Nullable MapboxMapOptions options) {
    super(context);
    initialise(context, options == null ? MapboxMapOptions.createFromAttributes(context, null) : options);
  }

  private void initialise(@NonNull final Context context, @NonNull final MapboxMapOptions options) {
    if (isInEditMode()) {
      // in IDE, show preview map
      LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_preview, this);
      return;
    }

    // determine render surface
    textureMode = options.getTextureMode();

    // inflate view
    View view = LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_internal, this);
    CompassView compassView = (CompassView) view.findViewById(R.id.compassView);
    MyLocationView myLocationView = (MyLocationView) view.findViewById(R.id.userLocationView);
    ImageView attrView = (ImageView) view.findViewById(R.id.attributionView);

    // add accessibility support
    setContentDescription(context.getString(R.string.mapbox_mapActionDescription));

    // create native Map object
    nativeMapView = new NativeMapView(this);

    // callback for focal point invalidation
    FocalPointInvalidator focalPoint = new FocalPointInvalidator(compassView);

    // callback for registering touch listeners
    RegisterTouchListener registerTouchListener = new RegisterTouchListener();

    // callback for zooming in the camera
    CameraZoomInvalidator zoomInvalidator = new CameraZoomInvalidator();

    // callback for camera change events
    CameraChangeDispatcher cameraChangeDispatcher = new CameraChangeDispatcher();

    // setup components for MapboxMap creation
    Projection proj = new Projection(nativeMapView);
    UiSettings uiSettings = new UiSettings(proj, focalPoint, compassView, attrView, view.findViewById(R.id.logoView));
    TrackingSettings trackingSettings = new TrackingSettings(myLocationView, uiSettings, focalPoint, zoomInvalidator);
    MyLocationViewSettings myLocationViewSettings = new MyLocationViewSettings(myLocationView, proj, focalPoint);
    MarkerViewManager markerViewManager = new MarkerViewManager((ViewGroup) findViewById(R.id.markerViewContainer));
    AnnotationManager annotations = new AnnotationManager(nativeMapView, this, markerViewManager);
    Transform transform = new Transform(nativeMapView, annotations.getMarkerViewManager(), trackingSettings,
      cameraChangeDispatcher);
    mapboxMap = new MapboxMap(nativeMapView, transform, uiSettings, trackingSettings, myLocationViewSettings, proj,
      registerTouchListener, annotations, cameraChangeDispatcher);

    // user input
    mapGestureDetector = new MapGestureDetector(context, transform, proj, uiSettings, trackingSettings, annotations,
      cameraChangeDispatcher);
    mapKeyListener = new MapKeyListener(transform, trackingSettings, uiSettings);

    MapZoomControllerListener zoomListener = new MapZoomControllerListener(mapGestureDetector, uiSettings, transform);
    mapZoomButtonController = new MapZoomButtonController(this, uiSettings, zoomListener);

    // inject widgets with MapboxMap
    compassView.setMapboxMap(mapboxMap);
    myLocationView.setMapboxMap(mapboxMap);
    attrView.setOnClickListener(new AttributionDialogManager(context, mapboxMap));

    // Ensure this view is interactable
    setClickable(true);
    setLongClickable(true);
    setFocusable(true);
    setFocusableInTouchMode(true);
    requestDisallowInterceptTouchEvent(true);

    // allow onDraw invocation
    setWillNotDraw(false);

    // notify Map object about current connectivity state
    nativeMapView.setReachability(ConnectivityReceiver.instance(context).isConnected(context));

    // initialise MapboxMap
    mapboxMap.initialise(context, options);
  }

  //
  // Lifecycle events
  //

  /**
   * <p>
   * You must call this method from the parent's Activity#onCreate(Bundle)} or
   * Fragment#onCreate(Bundle).
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
      MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapLoadEvent());
    } else if (savedInstanceState.getBoolean(MapboxConstants.STATE_HAS_SAVED_STATE)) {
      mapboxMap.onRestoreInstanceState(savedInstanceState);
    }

    initialiseDrawingSurface(textureMode);
    addOnMapChangedListener(mapCallback = new MapCallback(mapboxMap));
  }

  private void initialiseDrawingSurface(boolean textureMode) {
    nativeMapView.initializeDisplay();
    nativeMapView.initializeContext();
    if (textureMode) {
      TextureView textureView = new TextureView(getContext());
      textureView.setSurfaceTextureListener(new SurfaceTextureListener());
      addView(textureView, 0);
    } else {
      SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surfaceView);
      surfaceView.getHolder().addCallback(new SurfaceCallback());
      surfaceView.setVisibility(View.VISIBLE);
    }
  }

  /**
   * You must call this method from the parent's Activity#onSaveInstanceState(Bundle)
   * or Fragment#onSaveInstanceState(Bundle).
   *
   * @param outState Pass in the parent's outState.
   */
  @UiThread
  public void onSaveInstanceState(@NonNull Bundle outState) {
    outState.putBoolean(MapboxConstants.STATE_HAS_SAVED_STATE, true);
    mapboxMap.onSaveInstanceState(outState);
  }

  /**
   * You must call this method from the parent's Activity#onStart() or Fragment#onStart()
   */
  @UiThread
  public void onStart() {
    mapboxMap.onStart();
    ConnectivityReceiver.instance(getContext()).activate();
  }

  /**
   * You must call this method from the parent's Activity#onResume() or Fragment#onResume().
   */
  @UiThread
  public void onResume() {
    // replaced by onStart in v5.0.0
  }

  /**
   * You must call this method from the parent's Activity#onPause() or Fragment#onPause().
   */
  @UiThread
  public void onPause() {
    // replaced by onStop in v5.0.0
  }

  /**
   * You must call this method from the parent's Activity#onStop() or Fragment#onStop().
   */
  @UiThread
  public void onStop() {
    mapboxMap.onStop();
    ConnectivityReceiver.instance(getContext()).deactivate();
  }

  /**
   * You must call this method from the parent's Activity#onDestroy() or Fragment#onDestroy().
   */
  @UiThread
  public void onDestroy() {
    destroyed = true;
    nativeMapView.terminateContext();
    nativeMapView.terminateDisplay();
    nativeMapView.destroySurface();
    nativeMapView.destroy();
    nativeMapView = null;
  }

  @Override
  public boolean onTouchEvent(MotionEvent event) {
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
    return mapGestureDetector.onGenericMotionEvent(event) || super.onGenericMotionEvent(event);
  }

  @Override
  public boolean onHoverEvent(MotionEvent event) {
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
    nativeMapView.onLowMemory();
  }

  // Called when debug mode is enabled to update a FPS counter
  // Called via JNI from NativeMapView
  // Forward to any listener
  protected void onFpsChanged(final double fps) {
    final MapboxMap.OnFpsChangedListener listener = mapboxMap.getOnFpsChangedListener();
    if (listener != null) {
      post(new Runnable() {
        @Override
        public void run() {
          listener.onFpsChanged(fps);
        }
      });
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
    if (destroyed) {
      return;
    }

    nativeMapView.setStyleUrl(url);
  }

  //
  // Rendering
  //

  // Called when the map needs to be rerendered
  // Called via JNI from NativeMapView
  protected void onInvalidate() {
    postInvalidate();
  }

  @Override
  public void onDraw(Canvas canvas) {
    super.onDraw(canvas);
    if (isInEditMode()) {
      return;
    }

    if (destroyed) {
      return;
    }

    if (!hasSurface) {
      return;
    }

    nativeMapView.render();
  }

  @Override
  protected void onSizeChanged(int width, int height, int oldw, int oldh) {
    if (destroyed) {
      return;
    }

    if (!isInEditMode()) {
      nativeMapView.resizeView(width, height);
    }
  }

  private class SurfaceCallback implements SurfaceHolder.Callback {

    private Surface surface;

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
      nativeMapView.createSurface(surface = holder.getSurface());
      hasSurface = true;
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
      if (destroyed) {
        return;
      }
      nativeMapView.resizeFramebuffer(width, height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
      hasSurface = false;

      if (nativeMapView != null) {
        nativeMapView.destroySurface();
      }
      surface.release();
    }
  }

  // This class handles TextureView callbacks
  private class SurfaceTextureListener implements TextureView.SurfaceTextureListener {

    private Surface surface;

    // Called when the native surface texture has been created
    // Must do all EGL/GL ES initialization here
    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
      nativeMapView.createSurface(this.surface = new Surface(surface));
      nativeMapView.resizeFramebuffer(width, height);
      hasSurface = true;
    }

    // Called when the native surface texture has been destroyed
    // Must do all EGL/GL ES destruction here
    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
      hasSurface = false;

      if (nativeMapView != null) {
        nativeMapView.destroySurface();
      }
      this.surface.release();
      return true;
    }

    // Called when the format or size of the native surface texture has been changed
    // Must handle window resizing here.
    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
      if (destroyed) {
        return;
      }

      nativeMapView.resizeFramebuffer(width, height);
    }

    // Called when the SurfaceTexure frame is drawn to screen
    // Must sync with UI here
    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
      if (destroyed) {
        return;
      }
      mapboxMap.onUpdateRegionChange();
    }
  }

  //
  // View events
  //

  // Called when view is no longer connected
  @Override
  @CallSuper
  protected void onDetachedFromWindow() {
    super.onDetachedFromWindow();
    if (mapZoomButtonController != null) {
      mapZoomButtonController.setVisible(false);
    }
  }

  // Called when view is hidden and shown
  @Override
  protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
    if (isInEditMode() || mapZoomButtonController == null) {
      return;
    }
    mapZoomButtonController.setVisible(visibility == View.VISIBLE);
  }

  //
  // Map events
  //

  /**
   * <p>
   * Add a callback that's invoked when the displayed map view changes.
   * </p>
   * To remove the callback, use {@link MapView#removeOnMapChangedListener(OnMapChangedListener)}.
   *
   * @param listener The callback that's invoked on every frame rendered to the map view.
   * @see MapView#removeOnMapChangedListener(OnMapChangedListener)
   */
  public void addOnMapChangedListener(@Nullable OnMapChangedListener listener) {
    if (listener != null) {
      nativeMapView.addOnMapChangedListener(listener);
    }
  }

  /**
   * Remove a callback added with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}
   *
   * @param listener The previously added callback to remove.
   * @see MapView#addOnMapChangedListener(OnMapChangedListener)
   */
  public void removeOnMapChangedListener(@Nullable OnMapChangedListener listener) {
    if (listener != null) {
      nativeMapView.removeOnMapChangedListener(listener);
    }
  }

  /**
   * Sets a callback object which will be triggered when the {@link MapboxMap} instance is ready to be used.
   *
   * @param callback The callback object that will be triggered when the map is ready to be used.
   */
  @UiThread
  public void getMapAsync(final OnMapReadyCallback callback) {
    if (!mapCallback.isInitialLoad() && callback != null) {
      callback.onMapReady(mapboxMap);
    } else {
      if (callback != null) {
        mapCallback.addOnMapReadyCallback(callback);
      }
    }
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
   */
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
     */
    void onMapChanged(@MapChange int change);
  }

  private class FocalPointInvalidator implements FocalPointChangeListener {

    private final FocalPointChangeListener[] focalPointChangeListeners;

    FocalPointInvalidator(FocalPointChangeListener... listeners) {
      focalPointChangeListeners = listeners;
    }

    @Override
    public void onFocalPointChanged(PointF pointF) {
      mapGestureDetector.setFocalPoint(pointF);
      for (FocalPointChangeListener focalPointChangeListener : focalPointChangeListeners) {
        focalPointChangeListener.onFocalPointChanged(pointF);
      }
    }
  }

  private class RegisterTouchListener implements MapboxMap.OnRegisterTouchListener {

    @Override
    public void onRegisterMapClickListener(MapboxMap.OnMapClickListener listener) {
      mapGestureDetector.setOnMapClickListener(listener);
    }

    @Override
    public void onRegisterMapLongClickListener(MapboxMap.OnMapLongClickListener listener) {
      mapGestureDetector.setOnMapLongClickListener(listener);
    }

    @Override
    public void onRegisterScrollListener(MapboxMap.OnScrollListener listener) {
      mapGestureDetector.setOnScrollListener(listener);
    }

    @Override
    public void onRegisterFlingListener(MapboxMap.OnFlingListener listener) {
      mapGestureDetector.setOnFlingListener(listener);
    }
  }

  private class MapZoomControllerListener implements ZoomButtonsController.OnZoomListener {

    private final MapGestureDetector mapGestureDetector;
    private final UiSettings uiSettings;
    private final Transform transform;

    MapZoomControllerListener(MapGestureDetector detector, UiSettings uiSettings, Transform transform) {
      this.mapGestureDetector = detector;
      this.uiSettings = uiSettings;
      this.transform = transform;
    }

    // Not used
    @Override
    public void onVisibilityChanged(boolean visible) {
      // Ignore
    }

    // Called when user pushes a zoom button on the ZoomButtonController
    @Override
    public void onZoom(boolean zoomIn) {
      if (uiSettings.isZoomGesturesEnabled()) {
        onZoom(zoomIn, mapGestureDetector.getFocalPoint());
      }
    }

    private void onZoom(boolean zoomIn, @Nullable PointF focalPoint) {
      if (focalPoint != null) {
        transform.zoom(zoomIn, focalPoint);
      } else {
        PointF centerPoint = new PointF(getMeasuredWidth() / 2, getMeasuredHeight() / 2);
        transform.zoom(zoomIn, centerPoint);
      }
    }
  }

  private class CameraZoomInvalidator implements TrackingSettings.CameraZoomInvalidator {

    @Override
    public void zoomTo(double zoomLevel) {
      Transform transform = mapboxMap.getTransform();
      double currentZoomLevel = transform.getCameraPosition().zoom;
      if (currentZoomLevel < zoomLevel) {
        setZoom(zoomLevel, mapGestureDetector.getFocalPoint(), transform);
      }
    }

    private void setZoom(double zoomLevel, @Nullable PointF focalPoint, @NonNull Transform transform) {
      if (focalPoint != null) {
        transform.setZoom(zoomLevel, focalPoint);
      } else {
        PointF centerPoint = new PointF(getMeasuredWidth() / 2, getMeasuredHeight() / 2);
        transform.setZoom(zoomLevel, centerPoint);
      }
    }
  }

  private static class MapCallback implements OnMapChangedListener {

    private final MapboxMap mapboxMap;
    private final List<OnMapReadyCallback> onMapReadyCallbackList = new ArrayList<>();
    private boolean initialLoad = true;

    MapCallback(MapboxMap mapboxMap) {
      this.mapboxMap = mapboxMap;
    }

    @Override
    public void onMapChanged(@MapChange int change) {
      if (change == DID_FINISH_LOADING_STYLE && initialLoad) {
        initialLoad = false;
        new Handler().post(new Runnable() {
          @Override
          public void run() {
            mapboxMap.onPreMapReady();
            onMapReady();
            mapboxMap.onPostMapReady();
          }
        });
      } else if (change == DID_FINISH_RENDERING_FRAME || change == DID_FINISH_RENDERING_FRAME_FULLY_RENDERED) {
        mapboxMap.onUpdateFullyRendered();
      } else if (change == REGION_IS_CHANGING || change == REGION_DID_CHANGE || change == DID_FINISH_LOADING_MAP) {
        mapboxMap.onUpdateRegionChange();
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
  }
}
