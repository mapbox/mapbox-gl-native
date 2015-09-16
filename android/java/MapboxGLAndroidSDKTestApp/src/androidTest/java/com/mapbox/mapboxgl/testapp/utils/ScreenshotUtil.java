package com.mapbox.mapboxgl.testapp.utils;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.os.Environment;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
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

    public static void take(Activity activity, String testName) {

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

        // Add the SurfaceView bit (see getAllTextureViews() below)
        List<TextureView> tilingViews = getAllTextureViews(rootView);
        if (tilingViews.size() > 0) {
            Canvas canvas = new Canvas(bitmap);
            for (TextureView TextureView : tilingViews) {
                Bitmap b = TextureView.getBitmap(TextureView.getWidth(), TextureView.getHeight());
                int[] location = new int[2];
                TextureView.getLocationInWindow(location);
                int[] location2 = new int[2];
                TextureView.getLocationOnScreen(location2);
                canvas.drawBitmap(b, location[0], location[1], null);
            }
        }

        // Save the bitmap in external storage
        String uniqueAbsolutePath = getUniqueAbsolutePath(testName);
        File outputFile = new File(uniqueAbsolutePath);
        OutputStream outputStream = null;
        try {
            outputStream = new FileOutputStream(outputFile);
            bitmap.compress(DEFAULT_IMAGE_FORMAT, DEFAULT_IMAGE_QUALITY, outputStream);
            outputStream.flush();
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

    /*
     * The classic way of taking a screenshot (above) doesn't work with TextureView, this fixes it:
     * http://stackoverflow.com/questions/19704060/screen-capture-textureview-is-black-using-drawingcache
     */

    public static List<TextureView> getAllTextureViews(View view)
    {
        List<TextureView> tilingViews = new ArrayList<TextureView>();
        if (view instanceof TextureView) {
            tilingViews.add((TextureView)view);
        }  else if(view instanceof ViewGroup) {
            ViewGroup viewGroup = (ViewGroup)view;
            for (int i = 0; i < viewGroup.getChildCount(); i++) {
                tilingViews.addAll(getAllTextureViews(viewGroup.getChildAt(i)));
            }
        }

        return tilingViews;
    }

    /*
     * Utils
     */

    public static boolean isExternalStorageWritable() {
        // Checks if external storage is available for read and write
        String state = Environment.getExternalStorageState();
        return Environment.MEDIA_MOUNTED.equals(state);
    }

    private static String getUniqueAbsolutePath(String testName) {
        // A screenshot after every test vs. manual tests
        String filename = UUID.randomUUID().toString() + DEFAULT_IMAGE_EXTENSION;
        if (testName != null && !testName.isEmpty()) {
            filename = testName + DEFAULT_IMAGE_EXTENSION;
        }

        String externalPath = Environment.getExternalStorageDirectory().toString();
        String path = externalPath + File.separator + SCREENSHOT_FOLDER + File.separator + filename;
        Log.d(LOG_TAG, "Screenshot path: " + path);
        return path;
    }

}
