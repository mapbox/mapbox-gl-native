package com.mapbox.mapboxsdk.snapshotter;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.content.res.ResourcesCompat;
import android.text.Html;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.attribution.AttributionLayout;
import com.mapbox.mapboxsdk.attribution.AttributionMeasure;
import com.mapbox.mapboxsdk.attribution.AttributionParser;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.storage.FileSource;

import timber.log.Timber;

/**
 * Use MapSnapshotter to create static map images, rendered in the background.
 * <br>
 * MapSnapshotter itself must be used on the UI thread (for access to the main looper).
 * <br>
 * For an example of using MapSnapshotter, see <a href="https://github.com/mapbox/mapbox-gl-native/blob/e423ef5609cd738c07180d11744d4a45ffb3f82f/platform/android/MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity/snapshot/MapSnapshotterActivity.java" target="_blank">MapSnapshotterActivity.java</a>
 */
@UiThread
public class MapSnapshotter {

  /**
   * Callback interface for receiving notification on snapshot completion.
   *
   * @see MapSnapshotter#start(SnapshotReadyCallback, ErrorHandler)
   */
  public interface SnapshotReadyCallback {

    /**
     * Called on the UI thread when the snapshot is complete.
     *
     * @param snapshot the snapshot
     */
    void onSnapshotReady(MapSnapshot snapshot);

  }

  /**
   * Optional interface for handling errors encountered during snapshot generation.
   *
   * @see MapSnapshotter#start(SnapshotReadyCallback, ErrorHandler)
   */
  public interface ErrorHandler {

    /**
     * Called on error. No further snapshotting activity will happen until next call to {@link MapSnapshotter#start}.
     *
     * @param error the error message
     */
    void onError(String error);
  }

  private static final int LOGO_MARGIN_DP = 4;

  // Holds the pointer to JNI NativeMapView
  private long nativePtr = 0;

  private final Context context;
  private SnapshotReadyCallback callback;
  private ErrorHandler errorHandler;

  /**
   * Specifies the parameters for drawing a static map image using MapSnapshotter.
   * <br><br>
   * The width and height of the image must be specified.<br>
   * Set the Mapbox Style to use with the {@link Options#withStyle(String)} method.<br>
   * Set the camera position for the snapshot with {@link Options#withCameraPosition(CameraPosition)}.
   */
  public static class Options {
    private float pixelRatio = 1;
    private int width;
    private int height;
    private String styleUrl = Style.MAPBOX_STREETS;
    private LatLngBounds region;
    private CameraPosition cameraPosition;
    private boolean showLogo = true;

    /**
     * @param width  The width of the image, in pixels
     * @param height The height of the image, in pixels
     */
    public Options(int width, int height) {
      if (width == 0 || height == 0) {
        throw new IllegalArgumentException("Unable to create a snapshot with width or height set to 0");
      }
      this.width = width;
      this.height = height;
    }

    /**
     * @param url The style URL to use
     * @return The mutated {@link Options}
     */
    public Options withStyle(String url) {
      this.styleUrl = url;
      return this;
    }

    /**
     * @param region The region to show in the snapshot.
     *               If the {@link CameraPosition} is also set,
     *               the camera position will be overridden to match the specified region.
     * @return The mutated {@link Options}
     */
    public Options withRegion(LatLngBounds region) {
      this.region = region;
      return this;
    }

    /**
     * @param pixelRatio The pixel ratio to use (default: 1)
     * @return The mutated {@link Options}
     */
    public Options withPixelRatio(float pixelRatio) {
      this.pixelRatio = pixelRatio;
      return this;
    }

    /**
     * @param cameraPosition The {@link CameraPosition} to use for the snapshot.
     *                       Setting a {@link LatLngBounds} region will override the camera position.
     * @return The mutated {@link Options}
     */
    public Options withCameraPosition(CameraPosition cameraPosition) {
      this.cameraPosition = cameraPosition;
      return this;
    }

    /**
     * @param showLogo Flag indicating whether to show the Mapbox logo.
     * @return The mutated {@link Options}
     */
    public Options withLogo(boolean showLogo) {
      this.showLogo = showLogo;
      return this;
    }

    /**
     * @return The width of the image
     */
    public int getWidth() {
      return width;
    }

    /**
     * @return The height of the image
     */
    public int getHeight() {
      return height;
    }

    /**
     * @return The pixel ratio of the resulting image
     */
    public float getPixelRatio() {
      return pixelRatio;
    }

    /**
     * @return The region to include in the map, specified as {@link LatLngBounds}
     */
    @Nullable
    public LatLngBounds getRegion() {
      return region;
    }

    /**
     * @return The style URL
     */
    public String getStyleUrl() {
      return styleUrl;
    }

    /**
     * @return The {@link CameraPosition}
     */
    @Nullable
    public CameraPosition getCameraPosition() {
      return cameraPosition;
    }
  }

  /**
   * Creates the MapSnapshotter, but doesn't start rendering or
   * loading tiles. Use {@link Options} to configure the initial state of the map.
   *
   * @param context The Context that is or contains the Application context
   * @param options The {@link Options} to use for the snapshot
   */
  public MapSnapshotter(@NonNull Context context, @NonNull Options options) {
    this.context = context.getApplicationContext();
    FileSource fileSource = FileSource.getInstance(context);
    String programCacheDir = context.getCacheDir().getAbsolutePath();

    nativeInitialize(this, fileSource, options.pixelRatio, options.width,
      options.height, options.styleUrl, options.region, options.cameraPosition,
      options.showLogo, programCacheDir);
  }

  /**
   * Starts loading and rendering the snapshot. The callback will be fired
   * on the calling thread. Starting another snapshot before the current one has
   * finished will generate an error. Do not modify snapshot options while a snapshot
   * is in progress.
   *
   * @param callback The callback to use when the snapshot is ready
   */
  public void start(@NonNull SnapshotReadyCallback callback) {
    this.start(callback, null);
  }

  /**
   * Starts loading and rendering the snapshot. The callbacks will be fired
   * on the calling thread. Starting another snapshot before the current one has
   * finished will generate an error. Do not modify snapshot options while a snapshot
   * is in progress.
   *
   * @param callback     The callback to use when the snapshot is ready
   * @param errorHandler The error handler to use on snapshot errors
   */
  public void start(@NonNull SnapshotReadyCallback callback, ErrorHandler errorHandler) {
    if (this.callback != null) {
      throw new IllegalStateException("Snapshotter was already started");
    }

    this.callback = callback;
    this.errorHandler = errorHandler;
    nativeStart();
  }

  /**
   * Updates the MapSnapshotter with a new size
   *
   * @param width  The width
   * @param height The height
   */
  public native void setSize(int width, int height);

  /**
   * Updates the MapSnapshotter with a new {@link CameraPosition}
   *
   * @param cameraPosition The camera position
   */
  public native void setCameraPosition(CameraPosition cameraPosition);

  /**
   * Updates the MapSnapshotter with a new {@link LatLngBounds}.
   * The map's camera position will be updated to fit the region,
   * overriding any previously set camera position.
   *
   * @param region The region
   */
  public native void setRegion(LatLngBounds region);

  /**
   * Updates the MapSnapshotter with a new style url
   *
   * @param styleUrl The style url
   */
  public native void setStyleUrl(String styleUrl);


  /**
   * Cancel an in-progress snapshot request, allowing the MapSnapshotter to be re-used.
   * This will not cancel background network requests for map tiles: they will continue
   * to completion and the resulting tiles will be stored in the application's shared
   * tile cache.
   * Must be called in on the thread the MapSnapshotter was created on.
   */
  public void cancel() {
    reset();
    nativeCancel();
  }

  /**
   * Draw an overlay on the map snapshot.
   *
   * @param mapSnapshot the map snapshot to draw the overlay on
   */
  private void addOverlay(MapSnapshot mapSnapshot) {
    Bitmap snapshot = mapSnapshot.getBitmap();
    Canvas canvas = new Canvas(snapshot);
    int margin = (int) context.getResources().getDisplayMetrics().density * LOGO_MARGIN_DP;
    drawOverlay(mapSnapshot, snapshot, canvas, margin);
  }

  private void drawOverlay(MapSnapshot mapSnapshot, Bitmap snapshot, Canvas canvas, int margin) {
    AttributionMeasure measure = getAttributionMeasure(mapSnapshot, snapshot, margin);
    AttributionLayout layout = measure.measure();
    drawLogo(mapSnapshot, canvas, margin, layout);
    drawAttribution(mapSnapshot, canvas, measure, layout);
  }

  private AttributionMeasure getAttributionMeasure(MapSnapshot mapSnapshot, Bitmap snapshot, int margin) {
    Logo logo = createScaledLogo(snapshot);
    TextView longText = createTextView(mapSnapshot, false, logo.getScale());
    TextView shortText = createTextView(mapSnapshot, true, logo.getScale());

    return new AttributionMeasure.Builder()
      .setSnapshot(snapshot)
      .setLogo(logo.getLarge())
      .setLogoSmall(logo.getSmall())
      .setTextView(longText)
      .setTextViewShort(shortText)
      .setMarginPadding(margin)
      .build();
  }

  private void drawLogo(MapSnapshot mapSnapshot, Canvas canvas, int margin, AttributionLayout layout) {
    if (mapSnapshot.isShowLogo()) {
      drawLogo(mapSnapshot.getBitmap(), canvas, margin, layout);
    }
  }

  private void drawLogo(Bitmap snapshot, Canvas canvas, int margin, AttributionLayout placement) {
    Bitmap selectedLogo = placement.getLogo();
    if (selectedLogo != null) {
      canvas.drawBitmap(selectedLogo, margin, snapshot.getHeight() - selectedLogo.getHeight() - margin, null);
    }
  }

  private void drawAttribution(MapSnapshot mapSnapshot, Canvas canvas,
                               AttributionMeasure measure, AttributionLayout layout) {
    // draw attribution
    PointF anchorPoint = layout.getAnchorPoint();
    if (anchorPoint != null) {
      drawAttribution(canvas, measure, anchorPoint);
    } else {
      Bitmap snapshot = mapSnapshot.getBitmap();
      Timber.e("Could not generate attribution for snapshot size: %s x %s."
          + " You are required to provide your own attribution for the used sources: %s",
        snapshot.getWidth(), snapshot.getHeight(), mapSnapshot.getAttributions());
    }
  }

  private void drawAttribution(Canvas canvas, AttributionMeasure measure, PointF anchorPoint) {
    canvas.save();
    canvas.translate(anchorPoint.x, anchorPoint.y);
    measure.getTextView().draw(canvas);
    canvas.restore();
  }

  private TextView createTextView(MapSnapshot mapSnapshot, boolean shortText, float scale) {
    int textColor = ResourcesCompat.getColor(context.getResources(), R.color.mapbox_gray_dark, context.getTheme());
    int widthMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
    int heightMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
    TextView textView = new TextView(context);
    textView.setLayoutParams(new ViewGroup.LayoutParams(
      ViewGroup.LayoutParams.WRAP_CONTENT,
      ViewGroup.LayoutParams.WRAP_CONTENT)
    );
    textView.setSingleLine(true);
    textView.setTextSize(10 * scale);
    textView.setTextColor(textColor);
    textView.setBackgroundResource(R.drawable.mapbox_rounded_corner);
    textView.setText(Html.fromHtml(createAttributionString(mapSnapshot, shortText)));
    textView.measure(widthMeasureSpec, heightMeasureSpec);
    textView.layout(0, 0, textView.getMeasuredWidth(), textView.getMeasuredHeight());
    return textView;
  }

  /**
   * Create the attribution string.
   *
   * @param mapSnapshot the map snapshot to create the attribution for
   * @param shortText   indicates if the short variant of the string should be parsed
   * @return the parsed attribution string
   */
  private String createAttributionString(MapSnapshot mapSnapshot, boolean shortText) {
    AttributionParser attributionParser = new AttributionParser.Options()
      .withAttributionData(mapSnapshot.getAttributions())
      .withCopyrightSign(false)
      .withImproveMap(false)
      .build();

    return attributionParser.createAttributionString(shortText);
  }

  /**
   * Create a scaled logo for a map snapshot.
   *
   * @param snapshot the map snapshot where the logo should be placed on
   * @return the scaled large logo
   */
  private Logo createScaledLogo(@NonNull Bitmap snapshot) {
    Bitmap logo = BitmapFactory.decodeResource(context.getResources(), R.drawable.mapbox_logo_icon, null);
    float scale = calculateLogoScale(snapshot, logo);
    Matrix matrix = new Matrix();
    matrix.postScale(scale, scale);
    Bitmap helmet = BitmapFactory.decodeResource(context.getResources(), R.drawable.mapbox_logo_helmet, null);
    Bitmap large = Bitmap.createBitmap(logo, 0, 0, logo.getWidth(), logo.getHeight(), matrix, true);
    Bitmap small = Bitmap.createBitmap(helmet, 0, 0, helmet.getWidth(), helmet.getHeight(), matrix, true);
    return new Logo(large, small, scale);
  }

  /**
   * Calculates the scale of the logo, only allow downscaling.
   *
   * @param snapshot the large of the map snapshot
   * @param logo     the large of the mapbox logo
   * @return the scale value
   */
  private float calculateLogoScale(Bitmap snapshot, Bitmap logo) {
    DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
    float widthRatio = displayMetrics.widthPixels / snapshot.getWidth();
    float heightRatio = displayMetrics.heightPixels / snapshot.getHeight();
    float prefWidth = logo.getWidth() / widthRatio;
    float prefHeight = logo.getHeight() / heightRatio;
    float calculatedScale = Math.min(prefWidth / logo.getWidth(), prefHeight / logo.getHeight()) * 2;
    if (calculatedScale > 1) {
      // don't allow over-scaling
      calculatedScale = 1.0f;
    } else if (calculatedScale < 0.60f) {
      // don't scale to low either
      calculatedScale = 0.60f;
    }
    return calculatedScale;
  }

  /**
   * Called by JNI peer when snapshot is ready.
   * Always called on the origin (main) thread.
   *
   * @param snapshot the generated snapshot
   */
  private void onSnapshotReady(final MapSnapshot snapshot) {
    new Handler().post(new Runnable() {
      @Override
      public void run() {
        if (callback != null) {
          addOverlay(snapshot);
          callback.onSnapshotReady(snapshot);
          reset();
        }
      }
    });
  }

  /**
   * Called by JNI peer when snapshot has failed.
   * Always called on the origin (main) thread.
   *
   * @param reason the exception string
   */
  private void onSnapshotFailed(String reason) {
    if (errorHandler != null) {
      errorHandler.onError(reason);
      reset();
    }
  }

  private void reset() {
    callback = null;
    errorHandler = null;
  }

  private native void nativeInitialize(MapSnapshotter mapSnapshotter,
                                         FileSource fileSource, float pixelRatio,
                                         int width, int height, String styleUrl,
                                         LatLngBounds region, CameraPosition position,
                                         boolean showLogo, String programCacheDir);

  private native void nativeStart();

  private native void nativeCancel();

  @Override
  protected native void finalize() throws Throwable;

  private class Logo {
    private Bitmap large;
    private Bitmap small;
    private float scale;

    public Logo(Bitmap large, Bitmap small, float scale) {
      this.large = large;
      this.small = small;
      this.scale = scale;
    }

    public Bitmap getLarge() {
      return large;
    }

    public Bitmap getSmall() {
      return small;
    }

    public float getScale() {
      return scale;
    }
  }
}
