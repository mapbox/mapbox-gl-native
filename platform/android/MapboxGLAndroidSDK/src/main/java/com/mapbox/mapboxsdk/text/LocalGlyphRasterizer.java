package com.mapbox.mapboxsdk.text;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Bitmap;
import android.graphics.Typeface;

public class LocalGlyphRasterizer {

    public static Bitmap drawGlyphBitmap(String fontFamily, boolean bold, char glyphID) {
        Bitmap bitmap = Bitmap.createBitmap(35, 35, Bitmap.Config.ARGB_8888);

        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setTextSize(24);
        paint.setTypeface(Typeface.create(fontFamily, bold ? Typeface.BOLD : Typeface.NORMAL));

        Canvas canvas = new Canvas();
        canvas.setBitmap(bitmap);
        canvas.drawText(String.valueOf(glyphID), 0, 20, paint);

        return bitmap;
    }
}
