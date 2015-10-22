package com.mapbox.mapboxsdk.testapp;

import android.content.Context;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.geometry.LatLng;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

public class Util {

    public static String loadStringFromAssets(final Context context, final String fileName) throws IOException {
        if (TextUtils.isEmpty(fileName)) {
            throw new NullPointerException("No GeoJSON File Name passed in.");
        }
        InputStream is = context.getAssets().open(fileName);
        BufferedReader rd = new BufferedReader(new InputStreamReader(is, Charset.forName("UTF-8")));
        return readAll(rd);
    }

    public static List<LatLng> parseGeoJSONCoordinates(String geojsonStr) throws JSONException {
        List<LatLng> latLngs = new ArrayList<>();
        JSONObject jsonObject = new JSONObject(geojsonStr);
        JSONArray features = jsonObject.getJSONArray("features");
        int featureLength = features.length();
        for (int j = 0; j < featureLength; ++j) {
            JSONObject feature = features.getJSONObject(j);
            JSONObject geometry = feature.getJSONObject("geometry");
            String type = geometry.getString("type");
            JSONArray coordinates;
            if (type.equals("Polygon")) {
                coordinates = geometry.getJSONArray("coordinates").getJSONArray(0);
            } else {
                coordinates = geometry.getJSONArray("coordinates");
            }
            int len = coordinates.length();
            for (int i = 0; i < len; ++i) {
                if (coordinates.get(i) instanceof JSONArray) {
                    JSONArray coord = coordinates.getJSONArray(i);
                    double lng = coord.getDouble(0);
                    double lat = coord.getDouble(1);
                    latLngs.add(new LatLng(lat, lng));
                } else {
                    double lng = coordinates.getDouble(0);
                    double lat = coordinates.getDouble(1);
                    latLngs.add(new LatLng(lat, lng));
                }
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
