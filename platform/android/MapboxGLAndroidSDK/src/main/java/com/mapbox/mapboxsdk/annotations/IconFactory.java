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

    private static IconFactory instance;

    private Context context;
    private Icon defaultMarker;
    private Icon defaultMarkerView;
    private BitmapFactory.Options options;
    private int nextId = 0;

    public static synchronized IconFactory getInstance(@NonNull Context context) {
        if (instance == null) {
            instance = new IconFactory(context.getApplicationContext());
        }
        return instance;
    }

    private IconFactory(@NonNull Context context) {
        this.context = context;
        DisplayMetrics realMetrics = null;
        DisplayMetrics metrics = new DisplayMetrics();
        WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            realMetrics = new DisplayMetrics();
            wm.getDefaultDisplay().getRealMetrics(realMetrics);
        }
        wm.getDefaultDisplay().getMetrics(metrics);

        options = new BitmapFactory.Options();
        options.inScaled = true;
        options.inDensity = DisplayMetrics.DENSITY_DEFAULT;
        options.inTargetDensity = metrics.densityDpi;
        if (realMetrics != null) {
            options.inScreenDensity = realMetrics.densityDpi;
        }
    }

    public Icon fromBitmap(@NonNull Bitmap bitmap) {
        if (nextId < 0) {
            throw new TooManyIconsException();
        }
        String id = ICON_ID_PREFIX + ++nextId;
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
        Drawable drawable = ContextCompat.getDrawable(context, resourceId);
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
        if (defaultMarker == null) {
            defaultMarker = fromResource(R.drawable.default_marker);
        }
        return defaultMarker;
    }

    public Icon defaultMarkerView() {
        if (defaultMarkerView == null) {
            defaultMarkerView = fromResource(R.drawable.default_markerview);
        }
        return defaultMarkerView;
    }

    private Icon fromInputStream(@NonNull InputStream is) {
        Bitmap bitmap = BitmapFactory.decodeStream(is, null, options);
        return fromBitmap(bitmap);
    }

    public Icon fromAsset(@NonNull String assetName) {
        InputStream is;
        try {
            is = context.getAssets().open(assetName);
        } catch (IOException e) {
            return null;
        }
        return fromInputStream(is);
    }

    public Icon fromPath(@NonNull String absolutePath) {
        Bitmap bitmap = BitmapFactory.decodeFile(absolutePath, options);
        return fromBitmap(bitmap);
    }

    public Icon fromFile(@NonNull String fileName) {
        FileInputStream is;
        try {
            is = context.openFileInput(fileName);
        } catch (FileNotFoundException e) {
            return null;
        }
        return fromInputStream(is);
    }

    public static Icon recreate(@NonNull String iconId, @NonNull Bitmap bitmap) {
        return new Icon(iconId, bitmap);
    }
}
