package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.support.annotation.RawRes;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;

public class ResourceUtils {

  public static String readRawResource(Context context, @RawRes int rawResource) throws IOException {
    String json = "";
    if (context != null) {
      InputStream is = context.getResources().openRawResource(rawResource);
      Writer writer = new StringWriter();
      char[] buffer = new char[1024];
      try {
        Reader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"));
        int numRead;
        while ((numRead = reader.read(buffer)) != -1) {
          writer.write(buffer, 0, numRead);
        }
      } finally {
        is.close();
      }
      json = writer.toString();
    }
    return json;
  }
}

