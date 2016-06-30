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
 * @see Icon
 */
public final class IconFactory {

    private static final String ICON_ID_PREFIX = "com.mapbox.icons.icon_";

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

    public Icon fromBitmap(@NonNull Bitmap bitmap) {
        if (mNextId < 0) {
            throw new TooManyIconsException();
        }
        String id = ICON_ID_PREFIX + ++mNextId;
        return new Icon(id, bitmap);
    }

    public Icon fromDrawable(@NonNull Drawable drawable) {
        int width = drawable.getIntrinsicWidth();
        int height = drawable.getIntrinsicHeight();
        return fromDrawable(drawable, width, height);
    }

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
                bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(), Bitmap.Config.ARGB_8888);
            }

            Canvas canvas = new Canvas(bitmap);
            drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
            drawable.draw(canvas);
        }
        return fromBitmap(bitmap);
    }

    public Icon defaultMarker() {
        if (mDefaultMarker == null) {
            mDefaultMarker = fromResource(R.drawable.default_marker);
        }
        return mDefaultMarker;
    }

    public Icon defaultMarkerView() {
        if (mDefaultMarkerView == null) {
            mDefaultMarkerView = fromResource(R.drawable.default_markerview);
        }
        return mDefaultMarkerView;
    }

    private Icon fromInputStream(@NonNull InputStream is) {
        Bitmap bitmap = BitmapFactory.decodeStream(is, null, mOptions);
        return fromBitmap(bitmap);
    }

    public Icon fromAsset(@NonNull String assetName) {
        InputStream is;
        try {
            is = mContext.getAssets().open(assetName);
        } catch (IOException e) {
            return null;
        }
        return fromInputStream(is);
    }

    public Icon fromPath(@NonNull String absolutePath) {
        Bitmap bitmap = BitmapFactory.decodeFile(absolutePath, mOptions);
        return fromBitmap(bitmap);
    }

    public Icon fromFile(@NonNull String fileName) {
        FileInputStream is;
        try {
            is = mContext.openFileInput(fileName);
        } catch (FileNotFoundException e) {
            return null;
        }
        return fromInputStream(is);
    }

    public static Icon recreate(@NonNull String iconId, @NonNull Bitmap bitmap) {
        return new Icon(iconId, bitmap);
    }
}
