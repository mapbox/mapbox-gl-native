package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.exceptions.TooManySpritesException;
import com.mapbox.mapboxsdk.views.MapView;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

public final class SpriteFactory {

    private static final String SPRITE_ID_PREFIX = "com.mapbox.sprites.sprite_";

    private MapView mMapView;
    private Sprite mDefaultMarker;
    private BitmapFactory.Options mOptions;

    private int mNextId = 0;

    public SpriteFactory(MapView mapView) {
        mMapView = mapView;
        DisplayMetrics realMetrics = null;
        DisplayMetrics metrics = new DisplayMetrics();
        WindowManager wm = (WindowManager) mMapView.getContext().getSystemService(Context.WINDOW_SERVICE);

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

    public Sprite fromBitmap(Bitmap bitmap) {
        if (bitmap == null) {
            return null;
        }

        if (mNextId < 0) {
            throw new TooManySpritesException();
        }
        String id = SPRITE_ID_PREFIX + ++mNextId;

        return new Sprite(id, bitmap);
    }

    public Sprite fromDrawable(Drawable drawable) {
        int width = drawable.getIntrinsicWidth();
        int height = drawable.getIntrinsicHeight();

        return fromDrawable(drawable, width, height);
    }


    public Sprite fromDrawable(Drawable drawable, int width, int height) {
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

    public Sprite fromResource(int resourceId) {
        Bitmap bitmap = BitmapFactory.decodeResource(mMapView.getResources(), resourceId);
        return fromBitmap(bitmap);
    }

    public Sprite defaultMarker() {
        if (mDefaultMarker == null) {
            mDefaultMarker = fromResource(R.drawable.default_marker);
        }
        return mDefaultMarker;
    }

    private Sprite fromInputStream(InputStream is) {
        Bitmap bitmap = BitmapFactory.decodeStream(is, null, mOptions);
        return fromBitmap(bitmap);
    }

    public Sprite fromAsset(String assetName) {
        InputStream is;
        try {
            is = mMapView.getContext().getAssets().open(assetName);
        } catch (IOException e) {
            return null;
        }
        return fromInputStream(is);
    }

    public Sprite fromPath(String absolutePath) {
        Bitmap bitmap = BitmapFactory.decodeFile(absolutePath, mOptions);
        return fromBitmap(bitmap);
    }

    public Sprite fromFile(String fileName) {
        FileInputStream is;
        try {
            is = mMapView.getContext().openFileInput(fileName);
        } catch (FileNotFoundException e) {
            return null;
        }
        return fromInputStream(is);
    }
}
