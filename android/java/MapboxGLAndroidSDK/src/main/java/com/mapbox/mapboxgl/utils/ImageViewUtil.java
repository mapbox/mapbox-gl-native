package com.mapbox.mapboxgl.utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.support.annotation.NonNull;
import android.widget.ImageView;

public class ImageViewUtil {

    /**
     * Rotates an ImageView - does not work if the ImageView has padding, use margins
     *
     * @param imageView to rotate
     * @param angle to rotate with
     */
    public static void rotate(@NonNull final ImageView imageView, final float angle){
        Matrix matrix = new Matrix();
        matrix.setScale((float) imageView.getWidth() / (float) imageView.getDrawable().getIntrinsicWidth(), (float) imageView.getHeight() / (float) imageView.getDrawable().getIntrinsicHeight());
        matrix.postRotate(angle, (float) imageView.getWidth() / 2.0f, (float) imageView.getHeight() / 2.0f);
        imageView.setImageMatrix(matrix);
        imageView.setScaleType(ImageView.ScaleType.MATRIX);
    }


    /**
     * Detects if an resource has been overridden by the end developer
     *
     * @param context of the ImageView
     * @param resourceId of the drawable
     * @param byteCount of the original resource drawable
     * @return true for original resource
     */
    public static boolean validateByteSize(@NonNull final Context context, final int resourceId, final int byteCount) {
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inScaled = false;
        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resourceId, opts);
        return byteCount == bitmap.getByteCount();
    }

}
