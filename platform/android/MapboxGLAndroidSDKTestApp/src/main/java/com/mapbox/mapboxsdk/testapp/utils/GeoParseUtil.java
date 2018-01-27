package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.text.TextUtils;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

public class GeoParseUtil {

  public static String loadStringFromAssets(final Context context, final String fileName) throws IOException {
    if (TextUtils.isEmpty(fileName)) {
      throw new NullPointerException("No GeoJSON File Name passed in.");
    }
    InputStream is = context.getAssets().open(fileName);
    BufferedReader rd = new BufferedReader(new InputStreamReader(is, Charset.forName("UTF-8")));
    return readAll(rd);
  }

  public static List<LatLng> parseGeoJsonCoordinates(String geojsonStr) {
    List<LatLng> latLngs = new ArrayList<>();
    FeatureCollection featureCollection = FeatureCollection.fromJson(geojsonStr);
    for (Feature feature : featureCollection.features()) {
      if (feature.geometry() instanceof Point) {
        Point point = (Point) feature.geometry();
        latLngs.add(new LatLng(point.latitude(), point.longitude()));
      }
    }
    return latLngs;
  }

  private static String readAll(Reader rd) throws IOException {
    StringBuilder sb = new StringBuilder();
    int cp;
    while ((cp = rd.read()) != -1) {
      sb.append((char) cp);
    }
    return sb.toString();
  }
}
