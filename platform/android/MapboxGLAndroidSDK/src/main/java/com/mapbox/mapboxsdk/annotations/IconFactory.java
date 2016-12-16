package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.exceptions.TooManyIconsException;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

/**
 * Factory for creating {@link Icon} objects.
 *
 * @see Icon
 */
public final class IconFactory {

  private static final String ICON_ID_PREFIX = "com.mapbox.icons.icon_";
  public static final Bitmap ICON_MARKERVIEW_BITMAP = Bitmap.createBitmap(1, 1, Bitmap.Config.ALPHA_8);
  public static final String ICON_MARKERVIEW_ID = ICON_ID_PREFIX + "marker_view";

  private Context mContext;
  private static IconFactory sInstance;
  private Icon mDefaultMarker;
  private Icon mDefaultMarkerView;
  private BitmapFactory.Options mOptions;

  private int mNextId = 0;

  public static synchronized IconFactory getInstance(@NonNull Context context) {
    if (sInstance == null) {
      sInstance = new IconFactory(context.getApplicationContext());
    }
    return sInstance;
  }

  private IconFactory(@NonNull Context context) {
    mContext = context;
    DisplayMetrics realMetrics = null;
    DisplayMetrics metrics = new DisplayMetrics();
    WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
      realMetrics = new DisplayMetrics();
      wm.getDefaultDisplay().getRealMetrics(realMetrics);
    }
    wm.getDefaultDisplay().getMetrics(metrics);

    mOptions = new BitmapFactory.Options();
    mOptions.inScaled = true;
    mOptions.inDensity = DisplayMetrics.DENSITY_DEFAULT;
    mOptions.inTargetDensity = metrics.densityDpi;
    if (realMetrics != null) {
      mOptions.inScreenDensity = realMetrics.densityDpi;
    }
  }

  /**
   * Creates an {@link Icon} from a given Bitmap image.
   *
   * @param bitmap image used for creating the Icon.
   * @return The {@link Icon} using the given Bitmap image.
   */
  public Icon fromBitmap(@NonNull Bitmap bitmap) {
    if (mNextId < 0) {
      throw new TooManyIconsException();
    }
    String id = ICON_ID_PREFIX + ++mNextId;
    return new Icon(id, bitmap);
  }

  /**
   * Create an {@link Icon} from a given {@link Drawable}.
   *
   * @param drawable A {@link Drawable} object used for creating the {@link Icon}.
   * @return {@link Icon} with the provided {@link Drawable}.
   */
  public Icon fromDrawable(@NonNull Drawable drawable) {
    int width = drawable.getIntrinsicWidth();
    int height = drawable.getIntrinsicHeight();
    return fromDrawable(drawable, width, height);
  }

  /**
   * Create an {@link Icon} from a given {@link Drawable}.
   *
   * @param drawable A {@link Drawable} object used for creating the {@link Icon}.
   * @param width    An integer greater then zero defining the {@link Icon} width.
   * @param height   An integer greater then zero defining the {@link Icon} height.
   * @return {@link Icon} with the provided {@link Drawable}.
   */
  public Icon fromDrawable(@NonNull Drawable drawable, int width, int height) {
    if ((width < 0) || (height < 0)) {
      return null;
    }

    Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(bitmap);
    Rect temp = drawable.getBounds();
    Rect bounds = new Rect(0, 0, width, height);
    drawable.setBounds(bounds);
    drawable.draw(canvas);
    drawable.setBounds(temp);
    return fromBitmap(bitmap);
  }

  /**
   * Create an {@link Icon} using the resource ID of a Bitmap image.
   *
   * @param resourceId The resource ID of a Bitmap image.
   * @return The {@link Icon} that was loaded from the asset or {@code null} if failed to load.
   */
  public Icon fromResource(@DrawableRes int resourceId) {
    Drawable drawable = ContextCompat.getDrawable(mContext, resourceId);
    Bitmap bitmap;
    if (drawable instanceof BitmapDrawable) {
      BitmapDrawable bitmapDrawable = (BitmapDrawable) drawable;
      bitmap = bitmapDrawable.getBitmap();
    } else {
      if (drawable.getIntrinsicWidth() <= 0 || drawable.getIntrinsicHeight() <= 0) {
        bitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888);
      } else {
        bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(),
          Bitmap.Config.ARGB_8888);
      }

      Canvas canvas = new Canvas(bitmap);
      drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
      drawable.draw(canvas);
    }
    return fromBitmap(bitmap);
  }

  /**
   * Provides an {@link Icon} using the default marker icon used for {@link Marker}.
   *
   * @return An {@link Icon} with the default {@link Marker} icon.
   */
  public Icon defaultMarker() {
    if (mDefaultMarker == null) {
      mDefaultMarker = fromResource(R.drawable.mapbox_marker_icon_default);
    }
    return mDefaultMarker;
  }

  /**
   * Provides an {@link Icon} using the default marker icon used for {@link MarkerView}.
   *
   * @return An {@link Icon} with the default {@link MarkerView} icon.
   */
  public Icon defaultMarkerView() {
    if (mDefaultMarkerView == null) {
      mDefaultMarkerView = fromResource(R.drawable.mapbox_markerview_icon_default);
    }
    return mDefaultMarkerView;
  }

  private Icon fromInputStream(@NonNull InputStream is) {
    Bitmap bitmap = BitmapFactory.decodeStream(is, null, mOptions);
    return fromBitmap(bitmap);
  }

  /**
   * Creates an {@link Icon} using the name of a Bitmap image in the assets directory.
   *
   * @param assetName The name of a Bitmap image in the assets directory.
   * @return The {@link Icon} that was loaded from the asset or {@code null} if failed to load.
   */
  public Icon fromAsset(@NonNull String assetName) {
    InputStream is;
    try {
      is = mContext.getAssets().open(assetName);
    } catch (IOException ioException) {
      return null;
    }
    return fromInputStream(is);
  }

  /**
   * Creates an {@link Icon} using the absolute file path of a Bitmap image.
   *
   * @param absolutePath The absolute path of the Bitmap image.
   * @return The {@link Icon} that was loaded from the absolute path or {@code null} if failed to
   * load.
   */
  public Icon fromPath(@NonNull String absolutePath) {
    Bitmap bitmap = BitmapFactory.decodeFile(absolutePath, mOptions);
    return fromBitmap(bitmap);
  }

  /**
   * Create an {@link Icon} using the name of a Bitmap image file located in the internal storage.
   * In particular, this calls {@link Context#openFileInput(String)}.
   *
   * @param fileName The name of the Bitmap image file.
   * @return The {@link Icon} that was loaded from the asset or {@code null} if failed to load.
   * @see <a href="https://developer.android.com/guide/topics/data/data-storage.html#filesInternal">
   *   Using the Internal Storage</a>
   */
  public Icon fromFile(@NonNull String fileName) {
    FileInputStream is;
    try {
      is = mContext.openFileInput(fileName);
    } catch (FileNotFoundException fileNotFoundException) {
      return null;
    }
    return fromInputStream(is);
  }

  /**
   * Create an {@link Icon} using a previously created icon identifier along with a provided
   * Bitmap.
   *
   * @param iconId The {@link Icon} identifier you'd like to recreate.
   * @param bitmap a Bitmap used to replace the current one.
   * @return The {@link Icon} using the new Bitmap.
   */
  public static Icon recreate(@NonNull String iconId, @NonNull Bitmap bitmap) {
    return new Icon(iconId, bitmap);
  }
}
