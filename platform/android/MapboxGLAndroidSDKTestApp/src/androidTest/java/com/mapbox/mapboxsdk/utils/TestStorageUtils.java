package com.mapbox.mapboxsdk.utils;

import android.os.Environment;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.text.SimpleDateFormat;
import java.util.Date;

import timber.log.Timber;

public class TestStorageUtils {

  private static final String PERF_FOLDER = "performance";
  private static final SimpleDateFormat DATE_FORMAT = new SimpleDateFormat("yyyy_MM_dd_HH_mm_ss");
  private static final String TXT_EXTENSION = ".txt";

  public static String buildFileNameFrom(String testName) {
    return testName + obtainCurrentTimeStamp() + TXT_EXTENSION;
  }

  public static void storeResponse(String response, String dumpsysService, String fileName) {
    if (isExternalStorageWritable()) {
      File pathToExternalStorage = Environment.getExternalStorageDirectory();
      String storageAbsolutePath = pathToExternalStorage.getAbsolutePath();
      String pathname = String.format("%s/%s/%s", storageAbsolutePath, PERF_FOLDER, dumpsysService);
      File appDirectory = new File(pathname);
      appDirectory.mkdirs();
      File saveFilePath = new File(appDirectory, fileName);
      write(response, saveFilePath);
    }
  }

  private static String obtainCurrentTimeStamp() {
    Date now = new Date();
    return DATE_FORMAT.format(now);
  }

  private static boolean isExternalStorageWritable() {
    String state = Environment.getExternalStorageState();
    return Environment.MEDIA_MOUNTED.equals(state);
  }

  private static void write(String response, File saveFilePath) {
    try {
      FileOutputStream fos = new FileOutputStream(saveFilePath);
      OutputStreamWriter outDataWriter = new OutputStreamWriter(fos);
      outDataWriter.write(response);
      outDataWriter.close();
      fos.flush();
      fos.close();
    } catch (Exception exception) {
      Timber.e(exception);
    }
  }
}
