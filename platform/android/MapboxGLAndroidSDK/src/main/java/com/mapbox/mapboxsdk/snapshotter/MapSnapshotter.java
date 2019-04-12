package com.mapbox.mapboxsdk.snapshotter;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.os.Handler;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.content.res.ResourcesCompat;
import android.text.Html;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.attribution.AttributionLayout;
import com.mapbox.mapboxsdk.attribution.AttributionMeasure;
import com.mapbox.mapboxsdk.attribution.AttributionParser;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.utils.ThreadUtils;

/**
 * The map snapshotter creates a large of the map, rendered
 * off the UI thread. The snapshotter itself must be used on
 * the UI thread (for access to the main looper)
 */
@UiThread
public class MapSnapshotter {

  private static final String TAG = "Mbgl-MapSnapshotter";

  /**
   * Get notified on snapshot completion.
   *
   * @see MapSnapshotter#start(SnapshotReadyCallback, ErrorHandler)
   */
  public interface SnapshotReadyCallback {

    /**
     * Called when the snapshot is complete.
     *
     * @param snapshot the snapshot
     */
    void onSnapshotReady(MapSnapshot snapshot);

  }

  /**
   * Can be used to get notified of errors
   * in snapshot generation
   *
   * @see MapSnapshotter#start(SnapshotReadyCallback, ErrorHandler)
   */
  public interface ErrorHandler {

    /**
     * Called on error. Snapshotting will not
     * continue
     *
     * @param error the error message
     */
    void onError(String error);
  }

  private static final int LOGO_MARGIN_DP = 4;

  // Holds the pointer to JNI NativeMapView
  @Keep
  private long nativePtr = 0;

  private final Context context;
  @Nullable
  private SnapshotReadyCallback callback;
  @Nullable
  private ErrorHandler errorHandler;

  /**
   * MapSnapshotter options
   */
  public static class Options {
    private float pixelRatio = 1;
    private int width;
    private int height;
    private String styleUrl = Style.MAPBOX_STREETS;
    private String styleJson;
    private LatLngBounds region;
    private CameraPosition cameraPosition;
    private boolean showLogo = true;
    private String localIdeographFontFamily = "sans-serif";
    private String apiBaseUrl;

    /**
     * @param width  the width of the image
     * @param height the height of the image
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
     * @return the mutated {@link Options}
     */
    @NonNull
    public Options withStyle(String url) {
      this.styleUrl = url;
      return this;
    }

    /**
     * @param styleJson The style json to use
     * @return the mutated {@link Options}
     */
    @NonNull
    public Options withStyleJson(String styleJson) {
      this.styleJson = styleJson;
      return this;
    }

    /**
     * @param region the region to show in the snapshot.
     *               This is applied after the camera position
     * @return the mutated {@link Options}
     */
    @NonNull
    public Options withRegion(LatLngBounds region) {
      this.region = region;
      return this;
    }

    /**
     * @param pixelRatio the pixel ratio to use (default: 1)
     * @return the mutated {@link Options}
     */
    @NonNull
    public Options withPixelRatio(float pixelRatio) {
      this.pixelRatio = pixelRatio;
      return this;
    }

    /**
     * @param cameraPosition The camera position to use,
     *                       the {@link CameraPosition#target} is overridden
     *                       by region if set in conjunction.
     * @return the mutated {@link Options}
     */
    @NonNull
    public Options withCameraPosition(CameraPosition cameraPosition) {
      this.cameraPosition = cameraPosition;
      return this;
    }

    /**
     * @param showLogo The flag indicating to show the Mapbox logo.
     * @return the mutated {@link Options}
     */
    @NonNull
    public Options withLogo(boolean showLogo) {
      this.showLogo = showLogo;
      return this;
    }

    /**
     * Set the font family for generating glyphs locally for ideographs in the &#x27;CJK Unified Ideographs&#x27;
     * and &#x27;Hangul Syllables&#x27; ranges.
     * <p>
     * The font family argument is passed to {@link android.graphics.Typeface#create(String, int)}.
     * Default system fonts are defined in &#x27;/system/etc/fonts.xml&#x27;
     * Default font for local ideograph font family is "sans-serif".
     *
     * @param fontFamily font family for local ideograph generation.
     * @return the mutated {@link Options}
     */
    @NonNull
    public Options withLocalIdeographFontFamily(String fontFamily) {
      this.localIdeographFontFamily = fontFamily;
      return this;
    }

    /**
     * Specifies the URL used for API endpoint.
     *
     * @param apiBaseUrl The base of our API endpoint
     * @return the mutated {@link Options}
     */
    @NonNull
    public Options withApiBaseUrl(String apiBaseUrl) {
      this.apiBaseUrl = apiBaseUrl;
      return this;
    }

    /**
     * @return the width of the image
     */
    public int getWidth() {
      return width;
    }

    /**
     * @return the height of the image
     */
    public int getHeight() {
      return height;
    }

    /**
     * @return the pixel ratio
     */
    public float getPixelRatio() {
      return pixelRatio;
    }

    /**
     * @return the region
     */
    @Nullable
    public LatLngBounds getRegion() {
      return region;
    }

    /**
     * @return the style url
     */
    public String getStyleUrl() {
      return styleUrl;
    }

    /**
     * @return the camera position
     */
    @Nullable
    public CameraPosition getCameraPosition() {
      return cameraPosition;
    }

    /**
     * @return the font family used for locally generating ideographs,
     * Default font for local ideograph font family is "sans-serif".
     */
    public String getLocalIdeographFontFamily() {
      return localIdeographFontFamily;
    }

    /**
     * @return The base of our API endpoint
     */
    @Nullable
    public String getApiBaseUrl() {
      return apiBaseUrl;
    }
  }

  /**
   * Creates the Map snapshotter, but doesn't start rendering or
   * loading yet.
   *
   * @param context the Context that is or contains the Application context
   * @param options the options to use for the snapshot
   */
  public MapSnapshotter(@NonNull Context context, @NonNull Options options) {
    checkThread();
    this.context = context.getApplicationContext();
    TelemetryDefinition telemetry = Mapbox.getTelemetry();
    if (telemetry != null) {
      telemetry.onAppUserTurnstileEvent();
    }
    FileSource fileSource = FileSource.getInstance(context);
    String apiBaseUrl = options.getApiBaseUrl();
    if (!TextUtils.isEmpty(apiBaseUrl)) {
      fileSource.setApiBaseUrl(apiBaseUrl);
    }

    String programCacheDir = FileSource.getInternalCachePath(context);

    nativeInitialize(this, fileSource, options.pixelRatio, options.width,
      options.height, options.styleUrl, options.styleJson, options.region, options.cameraPosition,
      options.showLogo, programCacheDir, options.localIdeographFontFamily);
  }

  /**
   * Starts loading and rendering the snapshot. The callback will be fired
   * on the calling thread.
   *
   * @param callback the callback to use when the snapshot is ready
   */
  public void start(@NonNull SnapshotReadyCallback callback) {
    this.start(callback, null);
  }

  /**
   * Starts loading and rendering the snapshot. The callbacks will be fired
   * on the calling thread.
   *
   * @param callback     the callback to use when the snapshot is ready
   * @param errorHandler the error handler to use on snapshot errors
   */
  public void start(@NonNull SnapshotReadyCallback callback, ErrorHandler errorHandler) {
    if (this.callback != null) {
      throw new IllegalStateException("Snapshotter was already started");
    }
    checkThread();
    this.callback = callback;
    this.errorHandler = errorHandler;
    nativeStart();
  }

  /**
   * Updates the snapshotter with a new size
   *
   * @param width  the width
   * @param height the height
   */
  @Keep
  public native void setSize(int width, int height);

  /**
   * Updates the snapshotter with a new {@link CameraPosition}
   *
   * @param cameraPosition the camera position
   */
  @Keep
  public native void setCameraPosition(CameraPosition cameraPosition);

  /**
   * Updates the snapshotter with a new {@link LatLngBounds}
   *
   * @param region the region
   */
  @Keep
  public native void setRegion(LatLngBounds region);

  /**
   * Updates the snapshotter with a new style url
   *
   * @param styleUrl the style url
   */
  @Keep
  public native void setStyleUrl(String styleUrl);

  /**
   * Updates the snapshotter with a new style json
   *
   * @param styleJson the style json
   */
  @Keep
  public native void setStyleJson(String styleJson);

  /**
   * Must be called in on the thread
   * the object was created on.
   */
  public void cancel() {
    checkThread();
    reset();
    nativeCancel();
  }

  /**
   * Draw an overlay on the map snapshot.
   *
   * @param mapSnapshot the map snapshot to draw the overlay on
   */
  protected void addOverlay(@NonNull MapSnapshot mapSnapshot) {
    Bitmap snapshot = mapSnapshot.getBitmap();
    Canvas canvas = new Canvas(snapshot);
    int margin = (int) context.getResources().getDisplayMetrics().density * LOGO_MARGIN_DP;
    drawOverlay(mapSnapshot, snapshot, canvas, margin);
  }

  private void drawOverlay(@NonNull MapSnapshot mapSnapshot, @NonNull Bitmap snapshot,
                           @NonNull Canvas canvas, int margin) {
    AttributionMeasure measure = getAttributionMeasure(mapSnapshot, snapshot, margin);
    AttributionLayout layout = measure.measure();
    drawLogo(mapSnapshot, canvas, margin, layout);
    drawAttribution(mapSnapshot, canvas, measure, layout);
  }

  @NonNull
  private AttributionMeasure getAttributionMeasure(@NonNull MapSnapshot mapSnapshot,
                                                   @NonNull Bitmap snapshot, int margin) {
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

  private void drawLogo(MapSnapshot mapSnapshot, @NonNull Canvas canvas,
                        int margin, @NonNull AttributionLayout layout) {
    if (mapSnapshot.isShowLogo()) {
      drawLogo(mapSnapshot.getBitmap(), canvas, margin, layout);
    }
  }

  private void drawLogo(@NonNull Bitmap snapshot, @NonNull Canvas canvas,
                        int margin, AttributionLayout placement) {
    Bitmap selectedLogo = placement.getLogo();
    if (selectedLogo != null) {
      canvas.drawBitmap(selectedLogo, margin, snapshot.getHeight() - selectedLogo.getHeight() - margin, null);
    }
  }

  private void drawAttribution(@NonNull MapSnapshot mapSnapshot, @NonNull Canvas canvas,
                               @NonNull AttributionMeasure measure, AttributionLayout layout) {
    // draw attribution
    PointF anchorPoint = layout.getAnchorPoint();
    if (anchorPoint != null) {
      drawAttribution(canvas, measure, anchorPoint);
    } else {
      Bitmap snapshot = mapSnapshot.getBitmap();
      Logger.e(TAG, String.format("Could not generate attribution for snapshot size: %s x %s."
          + " You are required to provide your own attribution for the used sources: %s",
        snapshot.getWidth(), snapshot.getHeight(), mapSnapshot.getAttributions())
      );
    }
  }

  private void drawAttribution(Canvas canvas, AttributionMeasure measure, PointF anchorPoint) {
    canvas.save();
    canvas.translate(anchorPoint.x, anchorPoint.y);
    measure.getTextView().draw(canvas);
    canvas.restore();
  }

  @NonNull
  private TextView createTextView(@NonNull MapSnapshot mapSnapshot, boolean shortText, float scale) {
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
  @NonNull
  private String createAttributionString(MapSnapshot mapSnapshot, boolean shortText) {
    AttributionParser attributionParser = new AttributionParser.Options(context)
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
  @Keep
  protected void onSnapshotReady(@NonNull final MapSnapshot snapshot) {
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
  @Keep
  protected void onSnapshotFailed(String reason) {
    if (errorHandler != null) {
      errorHandler.onError(reason);
      reset();
    }
  }

  private void checkThread() {
    ThreadUtils.checkThread(TAG);
  }

  protected void reset() {
    callback = null;
    errorHandler = null;
  }

  @Keep
  protected native void nativeInitialize(MapSnapshotter mapSnapshotter,
                                         FileSource fileSource, float pixelRatio,
                                         int width, int height, String styleUrl, String styleJson,
                                         LatLngBounds region, CameraPosition position,
                                         boolean showLogo, String programCacheDir,
                                         String localIdeographFontFamily);

  @Keep
  protected native void nativeStart();

  @Keep
  protected native void nativeCancel();

  @Override
  @Keep
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
