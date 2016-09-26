package com.mapbox.mapboxsdk.testapp.activity.offline;

import android.support.annotation.NonNull;
import android.util.Log;

import com.mapbox.mapboxsdk.offline.OfflineRegion;

import org.json.JSONObject;

import static com.mapbox.mapboxsdk.testapp.activity.offline.OfflineActivity.JSON_CHARSET;
import static com.mapbox.mapboxsdk.testapp.activity.offline.OfflineActivity.JSON_FIELD_REGION_NAME;

class OfflineUtils {

    static String getRegionName(@NonNull  OfflineRegion offlineRegion) {
        String regionName;
        try {
            byte[] metadata = offlineRegion.getMetadata();
            String json = new String(metadata, JSON_CHARSET);
            JSONObject jsonObject = new JSONObject(json);
            regionName = jsonObject.getString(JSON_FIELD_REGION_NAME);
        } catch (Exception exception) {
            Log.e("OfflineUtils", "Failed to decode metadata: " + exception.getMessage());
            regionName = "Region " + offlineRegion.getID();
        }
        return regionName;
    }

    static byte[] convertRegionName(String regionName) {
        byte[] metadata = null;
        try {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put(JSON_FIELD_REGION_NAME, regionName);
            String json = jsonObject.toString();
            metadata = json.getBytes(JSON_CHARSET);
        } catch (Exception exception) {
            Log.e("OfflineUtils", "Failed to encode metadata: " + exception.getMessage());
        }
        return metadata;
    }
    
}
