package com.mapbox.mapboxsdk.maps;

import android.app.Activity;
import android.app.Dialog;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.PointF;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.annotation.CallSuper;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v7.app.AlertDialog;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.services.android.telemetry.MapboxEvent;
import com.mapbox.services.android.telemetry.MapboxTelemetry;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import timber.log.Timber;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

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
  private MapState mapState;
  private boolean destroyed;

  private GLSurfaceView glSurfaceView;
  private CompassView compassView;
  private ImageView attrView;
  private View logoView;
  private MyLocationView myLocationView;
  private MapboxMap mapboxMap;
  private MapCallback mapCallback = new MapCallback();
  private boolean onStartCalled;
  private boolean onStopCalled;

  private MapGestureDetector mapGestureDetector;
  private MapKeyListener mapKeyListener;
  private MapZoomButtonController mapZoomButtonController;

  private ConnectivityReceiver connectivityReceiver;

  private MapboxMapOptions mapboxMapOptions;

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
    initialise(context, options);
  }

  private void initialise(@NonNull final Context context, @NonNull final MapboxMapOptions options) {
    Timber.i("Initialize");
    this.mapboxMapOptions = options;

    // in IDE, show preview map
    if (isInEditMode()) {
      LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_preview, this);
      return;
    }

    //Setup basic view properties
    setupViewProperties();

    // inflate view
    View view = LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_internal, this);
    compassView = (CompassView) view.findViewById(R.id.compassView);
    myLocationView = (MyLocationView) view.findViewById(R.id.userLocationView);
    attrView = (ImageView) view.findViewById(R.id.attributionView);
    logoView = view.findViewById(R.id.logoView);

    glSurfaceView = (GLSurfaceView) findViewById(R.id.surfaceView);
    glSurfaceView.setEGLConfigChooser(8, 8, 8, 0 /** TODO: What alpha value do we need here?? */, 16, 8);
    glSurfaceView.setEGLContextClientVersion(2);

    glSurfaceView.setRenderer(new GLSurfaceView.Renderer() {
      @Override
      public void onSurfaceCreated(final GL10 gl, EGLConfig eglConfig) {
        Timber.i("[%s] onSurfaceCreated", Thread.currentThread().getName());
        glSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);

        // Create native Map object
        nativeMapView = new NativeMapView(MapView.this);
        nativeMapView.setAccessToken(Mapbox.getAccessToken());
        nativeMapView.setReachability(isConnected());

        //Continue configuring the map view on the main thread
        MapView.this.post(new Runnable() {
          @Override
          public void run() {
            onNativeMapViewReady();
          }
        });
      }

      @Override
      public void onSurfaceChanged(GL10 gl, int width, int height) {
        Timber.i("[%s] onSurfaceChanged %sx%s", Thread.currentThread().getName(), width, height);
        // Sets the current view port to the new size.
        gl.glViewport(0, 0, width, height);
        nativeMapView.onViewportChanged(width, height);
      }

      @Override
      public void onDrawFrame(GL10 gl) {
        Timber.i("[%s] onDrawFrame", Thread.currentThread().getName());
        nativeMapView.render();
      }
    });
  }

  private void setupViewProperties() {
    // allow onDraw invocation
    setWillNotDraw(false);

    // Ensure this view is interactable
    setClickable(true);
    setLongClickable(true);
    setFocusable(true);
    setFocusableInTouchMode(true);
    requestDisallowInterceptTouchEvent(true);
  }

  protected void onNativeMapViewReady() {
    ThreadExecutor threadExecutor = new ThreadExecutor() {
      @Override
      public void queueRenderEvent(final MapRunnable runnable) {
        glSurfaceView.queueEvent(new Runnable() {
          @Override
          public void run() {
            runnable.execute(nativeMapView);
          }
        });
      }

      @Override
      public void queueUiEvent(Runnable runnable) {
        glSurfaceView.post(runnable);
      }
    };


    mapState = new MapState(nativeMapView, threadExecutor);

    // callback for focal point invalidation
    FocalPointInvalidator focalPoint = new FocalPointInvalidator(compassView);

    // callback for registering touch listeners
    RegisterTouchListener registerTouchListener = new RegisterTouchListener();

    // callback for zooming in the camera
    CameraZoomInvalidator zoomInvalidator = new CameraZoomInvalidator();

    // setup components for MapboxMap creation
    Projection proj = new Projection(mapState, getWidth(), getHeight());
    UiSettings uiSettings = new UiSettings(proj, focalPoint, compassView, attrView, logoView);
    TrackingSettings tracking = new TrackingSettings(myLocationView, uiSettings, focalPoint, zoomInvalidator);
    MyLocationViewSettings myLocationViewSettings = new MyLocationViewSettings(myLocationView, proj, focalPoint);
    MarkerViewManager markerViewManager = new MarkerViewManager((ViewGroup) findViewById(R.id.markerViewContainer));
    AnnotationManager annotations = new AnnotationManager(nativeMapView, threadExecutor, this, markerViewManager);
    Transform transform = new Transform(nativeMapView, threadExecutor, annotations.getMarkerViewManager(), tracking);
    mapboxMap = new MapboxMap(mapState, transform, uiSettings, tracking, myLocationViewSettings, proj,
      registerTouchListener, annotations);

    // user input
    mapGestureDetector = new MapGestureDetector(getContext(), transform, proj, uiSettings, tracking, annotations);
    mapKeyListener = new MapKeyListener(transform, tracking, uiSettings);
    mapZoomButtonController = new MapZoomButtonController(this, uiSettings, transform);

    // inject widgets with MapboxMap
    compassView.setMapboxMap(mapboxMap);
    myLocationView.setMapboxMap(mapboxMap);
    attrView.setOnClickListener(new AttributionOnClickListener(getContext(), transform));

    // initialise MapboxMap
    mapboxMap.initialise(getContext(), mapboxMapOptions);

    addOnMapChangedListener(mapCallback);
    mapboxMap.onStart();
  }

  //
  // Lifecycle events
  //

  /**
   * <p>
   * You must call this method from the parent's {@link android.app.Activity#onCreate(Bundle)} or
   * {@link android.app.Fragment#onCreate(Bundle)}.
   * </p>
   * You must set a valid access token with {@link Mapbox#getInstance(Context, String)}) before you call this method
   * or an exception will be thrown.
   *
   * @param savedInstanceState Pass in the parent's savedInstanceState.
   * @see Mapbox#getInstance(Context, String)
   */
  @UiThread
  public void onCreate(@Nullable Bundle savedInstanceState) {
    if (savedInstanceState == null) {
      MapboxTelemetry.getInstance().pushEvent(MapboxEvent.buildMapLoadEvent());
    } else if (savedInstanceState.getBoolean(MapboxConstants.STATE_HAS_SAVED_STATE)) {
      mapState.onRestoreInstanceState(savedInstanceState);
      mapboxMap.onRestoreInstanceState(savedInstanceState);
    }
  }

  /**
   * You must call this method from the parent's {@link android.app.Activity#onSaveInstanceState(Bundle)}
   * or {@link android.app.Fragment#onSaveInstanceState(Bundle)}.
   *
   * @param outState Pass in the parent's outState.
   */

  @UiThread
  public void onSaveInstanceState(@NonNull Bundle outState) {
    outState.putBoolean(MapboxConstants.STATE_HAS_SAVED_STATE, true);
    mapState.onSaveInstanceState(outState);
    mapboxMap.onSaveInstanceState(outState);
  }

  /**
   * You must call this method from the parent's {@link Activity#onStart()} or {@link Fragment#onStart()}
   */
  @UiThread
  public void onStart() {
    onStartCalled = true;
    registerConnectivityReceiver();
    glSurfaceView.onResume();
  }

  /**
   * You must call this method from the parent's {@link Activity#onResume()} or {@link Fragment#onResume()}.
   */
  @UiThread
  public void onResume() {
    if (!onStartCalled) {
      // TODO: 26/10/16, can be removed after 5.0.0 release
      throw new IllegalStateException("MapView#onStart() was not called. "
        + "You must call this method from the parent's {@link Activity#onStart()} or {@link Fragment#onStart()}.");
    }
  }

  /**
   * You must call this method from the parent's {@link Activity#onPause()} or {@link Fragment#onPause()}.
   */
  @UiThread
  public void onPause() {
    // replaced by onStop in v5.0.0, keep around for future development
  }

  /**
   * You must call this method from the parent's {@link Activity#onStop()} or {@link Fragment#onStop()}.
   */
  @UiThread
  public void onStop() {
    onStopCalled = true;
    unregisterConnectivityReceiver();
    mapboxMap.onStop();
    glSurfaceView.onPause();
  }

  /**
   * You must call this method from the parent's {@link Activity#onDestroy()} or {@link Fragment#onDestroy()}.
   */
  @UiThread
  public void onDestroy() {
    if (!onStopCalled) {
      // TODO: 26/10/16, can be removed after 5.0.0 release
      throw new IllegalStateException("MapView#onStop() was not called. "
        + "You must call this method from the parent's {@link Activity#onStop()} or {@link Fragment#onStop()}.");
    }

    destroyed = true;

    glSurfaceView.queueEvent(new Runnable() {
      @Override
      public void run() {
        nativeMapView.destroy();
        nativeMapView = null;
      }
    });
  }

  private void registerConnectivityReceiver() {
    getContext().registerReceiver(connectivityReceiver = new ConnectivityReceiver(),
      new IntentFilter(ConnectivityManager.CONNECTIVITY_ACTION));
  }

  private void unregisterConnectivityReceiver() {
    if (connectivityReceiver != null) {
      getContext().unregisterReceiver(connectivityReceiver);
      connectivityReceiver = null;
    }
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
   * You must call this method from the parent's {@link Activity#onLowMemory()} or {@link Fragment#onLowMemory()}.
   */
  @UiThread
  public void onLowMemory() {
    glSurfaceView.queueEvent(new Runnable() {
      @Override
      public void run() {
        nativeMapView.onLowMemory();
      }
    });
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
  public void setStyleUrl(@NonNull final String url) {
    if (destroyed) {
      return;
    }

    mapState.setStyleUrl(url);
  }

  //
  // Rendering
  //

  /**
   * Called when the map needs to be re-rendered
   * Called via JNI from NativeMapView
   * <p>
   * May be called from any thread
   */
  protected void onInvalidate() {
    Timber.i("onInvalidate");
    //glSurfaceView.onInvalidate();
    //XXX Don't need this right? postInvalidate();
    postInvalidate();
  }

  protected void requestRender() {
    if (glSurfaceView != null) {
      glSurfaceView.requestRender();
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
    mapZoomButtonController.setVisible(false);
  }

  // Called when view is hidden and shown
  @Override
  protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
    if (isInEditMode()) {
      return;
    }
    if (mapZoomButtonController != null) {
      mapZoomButtonController.setVisible(visibility == View.VISIBLE);
    }
  }

  //
  // Connectivity events
  //

  // This class handles connectivity changes
  private class ConnectivityReceiver extends BroadcastReceiver {

    // Called when an action we are listening to in the manifest has been sent
    @Override
    public void onReceive(Context context, Intent intent) {
      if (nativeMapView != null && !destroyed && intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
        nativeMapView.setReachability(!intent.getBooleanExtra(ConnectivityManager.EXTRA_NO_CONNECTIVITY, false));
      }
    }
  }

  // Called when MapView is being created
  private boolean isConnected() {
    ConnectivityManager connectivityManager = (ConnectivityManager)
      getContext().getSystemService(Context.CONNECTIVITY_SERVICE);
    NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
    return (activeNetwork != null) && activeNetwork.isConnectedOrConnecting();
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

  private static class AttributionOnClickListener implements View.OnClickListener, DialogInterface.OnClickListener {

    private static final int ATTRIBUTION_INDEX_IMPROVE_THIS_MAP = 2;
    private static final int ATTRIBUTION_INDEX_TELEMETRY_SETTINGS = 3;
    private Context context;
    private Transform transform;

    public AttributionOnClickListener(Context context, Transform transform) {
      this.context = context;
      this.transform = transform;
    }

    // Called when someone presses the attribution icon
    @Override
    public void onClick(View view) {
      AlertDialog.Builder builder = new AlertDialog.Builder(context, R.style.mapbox_AlertDialogStyle);
      builder.setTitle(R.string.mapbox_attributionsDialogTitle);
      String[] items = context.getResources().getStringArray(R.array.mapbox_attribution_names);
      builder.setAdapter(new ArrayAdapter<>(context, R.layout.mapbox_attribution_list_item, items), this);
      builder.show();
    }

    // Called when someone selects an attribution, 'Improve this map' adds location data to the url
    @Override
    public void onClick(DialogInterface dialog, int which) {
      final Context context = ((Dialog) dialog).getContext();
      if (which == ATTRIBUTION_INDEX_TELEMETRY_SETTINGS) {
        AlertDialog.Builder builder = new AlertDialog.Builder(context, R.style.mapbox_AlertDialogStyle);
        builder.setTitle(R.string.mapbox_attributionTelemetryTitle);
        builder.setMessage(R.string.mapbox_attributionTelemetryMessage);
        builder.setPositiveButton(R.string.mapbox_attributionTelemetryPositive, new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            MapboxTelemetry.getInstance().setTelemetryEnabled(true);
            dialog.cancel();
          }
        });
        builder.setNeutralButton(R.string.mapbox_attributionTelemetryNeutral, new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            String url = context.getResources().getStringArray(R.array.mapbox_attribution_links)[3];
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse(url));
            context.startActivity(intent);
            dialog.cancel();
          }
        });
        builder.setNegativeButton(R.string.mapbox_attributionTelemetryNegative, new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            MapboxTelemetry.getInstance().setTelemetryEnabled(false);
            dialog.cancel();
          }
        });

        builder.show();
        return;
      }
      String url = context.getResources().getStringArray(R.array.mapbox_attribution_links)[which];
      if (which == ATTRIBUTION_INDEX_IMPROVE_THIS_MAP) {
        CameraPosition cameraPosition = transform.getCameraPosition();
        if (cameraPosition != null) {
          url = String.format(url, cameraPosition.target.getLongitude(),
            cameraPosition.target.getLatitude(), (int) cameraPosition.zoom);
        }
      }
      Intent intent = new Intent(Intent.ACTION_VIEW);
      intent.setData(Uri.parse(url));
      context.startActivity(intent);
    }
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
   * This event is triggered when the map finished rendeirng the frame fully.
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
   * This {@link MapChange} is triggered when a source attribution changes.
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

  private class CameraZoomInvalidator implements TrackingSettings.CameraZoomInvalidator {
    @Override
    public void zoomTo(double zoomLevel) {
      double currentZoomLevel = mapboxMap.getCameraPosition().zoom;
      if (currentZoomLevel < zoomLevel) {
        mapboxMap.getTransform().setZoom(zoomLevel);
      }
    }
  }

  private class MapCallback implements OnMapChangedListener {

    private final List<OnMapReadyCallback> onMapReadyCallbackList = new ArrayList<>();
    private boolean initialLoad = true;

    @Override
    public void onMapChanged(@MapChange int change) {
      if (change == DID_FINISH_LOADING_STYLE && initialLoad) {
        initialLoad = false;
        mapboxMap.onPreMapReady();
        onMapReady();
        mapboxMap.onPostMapReady();
      } else if (change == REGION_IS_CHANGING || change == REGION_DID_CHANGE || change == DID_FINISH_LOADING_MAP) {
        mapboxMap.onUpdate();
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
