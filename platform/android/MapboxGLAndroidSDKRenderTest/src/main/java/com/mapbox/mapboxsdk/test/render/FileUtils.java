package com.mapbox.mapboxsdk.test.render;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class FileUtils {

  public static File createTestResultRootFolder() {
    File testResultDir = new File(getRoot());
    if (testResultDir.exists()) {
      // cleanup old files
      deleteRecursive(testResultDir);
    }

    if (!testResultDir.mkdirs()) {
      throw new RuntimeException("can't create root test directory");
    }
    return testResultDir;
  }

  private static String getRoot(){
    return Environment.getExternalStorageDirectory()
      + File.separator + "mapbox" + File.separator + "render";
  }

  public static void deleteRecursive(File fileOrDirectory) {
    if (fileOrDirectory.isDirectory()) {
      File[] files = fileOrDirectory.listFiles();
      if (files != null) {
        for (File file : files) {
          deleteRecursive(file);
        }
      }
    }

    if (!fileOrDirectory.delete()) {
      Log.e("TAG", "can't delete directory");
    }
  }

  public static String createTestDirectory(String testName) {
    File testDir = new File(getRoot() + File.separator + testName);
    if (!testDir.exists()) {
      if (!testDir.mkdirs()) {
        throw new RuntimeException("can't create sub directory for " + testName);
      }
    }
    return testDir.getAbsolutePath();
  }

  public static void writeTestResultToDisk(String testPath, Bitmap testResult) {
    String filePath = testPath + "/actual.png";
    try (FileOutputStream out = new FileOutputStream(filePath)) {
      testResult.compress(Bitmap.CompressFormat.PNG, 100, out);
    } catch (IOException exception) {
      Log.e("TAG", "exception", exception);
    }
  }

  public static boolean copyAsset(AssetManager assetManager,
                                   String fromAssetPath, String toPath) {
    InputStream in;
    OutputStream out;
    try {
      in = assetManager.open(fromAssetPath);
      new File(toPath).createNewFile();
      out = new FileOutputStream(toPath);
      copyFile(in, out);
      in.close();
      out.flush();
      out.close();
      return true;
    } catch(Exception e) {
      e.printStackTrace();
      return false;
    }
  }

  private static void copyFile(InputStream in, OutputStream out) throws IOException {
    byte[] buffer = new byte[1024];
    int read;
    while((read = in.read(buffer)) != -1){
      out.write(buffer, 0, read);
    }
  }
}