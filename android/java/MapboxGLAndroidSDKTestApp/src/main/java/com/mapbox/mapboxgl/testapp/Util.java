package com.mapbox.mapboxgl.testapp;

import android.content.Context;
import android.text.TextUtils;

import com.mapbox.mapboxgl.geometry.LatLng;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.charset.Charset;

public class Util {

    public static String loadStringFromAssets(final Context context, final String fileName) throws IOException {
        if (TextUtils.isEmpty(fileName)) {
            throw new NullPointerException("No GeoJSON File Name passed in.");
        }
        InputStream is = context.getAssets().open(fileName);
        BufferedReader rd = new BufferedReader(new InputStreamReader(is, Charset.forName("UTF-8")));
        return readAll(rd);
    }

    public static LatLng[] parseGeoJSONCoordinates(String geojsonStr) throws JSONException {
        JSONObject jsonObject = new JSONObject(geojsonStr);
        JSONArray features = jsonObject.getJSONArray("features");
        JSONObject feature = features.getJSONObject(0);
        JSONObject geometry = feature.getJSONObject("geometry");
        String type = geometry.getString("type");
        JSONArray coordinates;
        if (type.equals("Polygon")) {
            coordinates = geometry.getJSONArray("coordinates").getJSONArray(0);
        } else {
            coordinates = geometry.getJSONArray("coordinates");
        }
        int len = coordinates.length();
        LatLng[] latLngs = new LatLng[coordinates.length()];
        for (int i = 0; i < len; ++i) {
            JSONArray coord = coordinates.getJSONArray(i);
            double lng = coord.getDouble(0);
            double lat = coord.getDouble(1);
            latLngs[i] = new LatLng(lat, lng);
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
