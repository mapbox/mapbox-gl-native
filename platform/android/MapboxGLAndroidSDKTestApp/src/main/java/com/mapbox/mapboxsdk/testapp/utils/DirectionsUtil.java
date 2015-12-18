package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.support.annotation.NonNull;
import android.util.Log;

import com.google.gson.GsonBuilder;
import com.google.gson.stream.JsonReader;
import com.mapbox.mapboxsdk.geometry.BoundingBox;
import com.mapbox.mapboxsdk.geometry.CoordinateBounds;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.model.directions.DirectionsFeature;
import com.mapbox.mapboxsdk.testapp.model.directions.DirectionsResponse;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class DirectionsUtil {

    private static final String TAG = "DirectionsUtil";

    public static DirectionsResponse getDirections(@NonNull Context context) {
        DirectionsResponse response = null;
        InputStream inputStream = null;
        try {
            inputStream = context.getAssets().open("directions.json");
            final JsonReader jsonReader = new JsonReader(new InputStreamReader(inputStream));
            response = new GsonBuilder().create().fromJson(jsonReader, DirectionsResponse.class);
        } catch (final Exception e) {
            Log.e(TAG, "Can't load directions.json", e);
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (final IOException ignore) {
                }
            }
        }
        return response;
    }

    public static LatLng convertLatLng(DirectionsFeature directionsFeature) {
        List<Double> coordinates = directionsFeature.getGeometry().getCoordinates();
        return new LatLng(coordinates.get(1), coordinates.get(0));
    }

    public static CoordinateBounds calculateCoordinateBounds(LatLng origin, LatLng destination) {
        List<LatLng> coordinates = new ArrayList<>();
        coordinates.add(origin);
        coordinates.add(destination);

        BoundingBox box = BoundingBox.fromLatLngs(coordinates);
        LatLng southWest = new LatLng(box.getLatSouth(), box.getLonWest());
        LatLng northEast = new LatLng(box.getLatNorth(), box.getLonEast());
        return new CoordinateBounds(southWest, northEast);
    }

}
