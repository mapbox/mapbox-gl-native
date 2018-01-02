package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.text.TextUtils;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.Point;
import com.mapbox.services.commons.models.Position;

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
    for (Feature feature : featureCollection.getFeatures()) {
      if (feature.getGeometry() instanceof Point) {
        Position point = ((Point) feature.getGeometry()).getCoordinates();
        latLngs.add(new LatLng(point.getLatitude(), point.getLongitude()));
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
