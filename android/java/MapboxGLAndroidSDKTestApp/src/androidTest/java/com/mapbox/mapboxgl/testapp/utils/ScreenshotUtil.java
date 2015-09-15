package com.mapbox.mapboxgl.testapp.utils;

import android.app.Activity;
import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;

/**
 * The built-in Fuzz Suite on AWS Device Farm takes screenshots after every test. However,
 * this doesn't happen with Espresso unless we manually do it. This class fixes it.
 */
public class ScreenshotUtil {

    private static final String LOG_TAG = ScreenshotUtil.class.getName();

    // Where to store the files. This path is required by AWS Device Farm:
    // http://docs.aws.amazon.com/devicefarm/latest/developerguide/test-types-android-instrumentation.html#test-types-android-instrumentation-screenshots
    private static final String SCREENSHOT_FOLDER = "test-screenshots";

    // Image type and quality
    private static final String DEFAULT_IMAGE_EXTENSION = ".png";
    private static final Bitmap.CompressFormat DEFAULT_IMAGE_FORMAT = Bitmap.CompressFormat.PNG;
    private static final int DEFAULT_IMAGE_QUALITY = 100;

    public static void take(Activity activity) {

        // Check if storage is available
        if (!isExternalStorageWritable()) {
            Log.d(LOG_TAG, "External storage is not available.");
            return;
        }

        // Get a bitmap from the activity root view. When the drawing cache is enabled,
        // the next call to getDrawingCache() will draw the view in a bitmap.
        View rootView = activity.getWindow().getDecorView().getRootView();
        rootView.setDrawingCacheEnabled(true);
        Bitmap bitmap = Bitmap.createBitmap(rootView.getDrawingCache());
        rootView.setDrawingCacheEnabled(false);

        // Save the bitmap in external storage
        String uniqueAbsolutePath = getUniqueAbsolutePath();
        File outputFile = new File(uniqueAbsolutePath);
        OutputStream outputStream = null;
        try {
            outputStream = new FileOutputStream(outputFile);
            bitmap.compress(DEFAULT_IMAGE_FORMAT, DEFAULT_IMAGE_QUALITY, outputStream);
            outputStream.flush();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /* Checks if external storage is available for read and write */
    public static boolean isExternalStorageWritable() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            return true;
        }
        return false;
    }

    private static String getUniqueAbsolutePath() {
        String externalPath = Environment.getExternalStorageDirectory().toString();
        String filename = UUID.randomUUID().toString() + DEFAULT_IMAGE_EXTENSION;
        String path = externalPath + File.separator + SCREENSHOT_FOLDER + File.separator + filename;
        Log.d(LOG_TAG, "Screenshot path: " + path);
        return path;
    }

}
