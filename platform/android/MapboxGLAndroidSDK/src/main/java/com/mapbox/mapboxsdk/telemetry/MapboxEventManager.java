package com.mapbox.mapboxsdk.telemetry;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.location.Location;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
import android.os.BatteryManager;
import android.os.Build;
import android.support.annotation.NonNull;
import android.text.TextUtils;
import android.util.Log;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import org.json.JSONArray;
import org.json.JSONObject;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Hashtable;
import java.util.List;
import java.util.Locale;
import java.util.UUID;
import java.util.Vector;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class MapboxEventManager {

    private static final String TAG = "MapboxEventManager";

    private static MapboxEventManager mapboxEventManager = null;

    private final Vector<Hashtable<String, Object>> events = new Vector<>();
    private static final MediaType JSON  = MediaType.parse("application/json; charset=utf-8");
    private static SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSZ", Locale.US);

    private Context context = null;
    private String accessToken = null;
    private static final String MAPBOX_EVENTS_BASE_URL = "https://api.tiles.mapbox.com";
    private String eventsURL = MAPBOX_EVENTS_BASE_URL;

    private static final String SESSION_UUID = UUID.randomUUID().toString();

    private String userAgent = MapboxEvent.MGLMapboxEventsUserAgent;

    private Intent batteryStatus = null;

    private MapboxEventManager(@NonNull Context context) {
        super();
        this.accessToken = ApiAccess.getToken(context);
        this.context = context;

        // Check for Staging Server Information
        try {
            ApplicationInfo appInfo = context.getPackageManager().getApplicationInfo(context.getPackageName(), PackageManager.GET_META_DATA);
            String stagingURL = appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_STAGING_SERVER);
            String stagingAccessToken = appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_STAGING_ACCESS_TOKEN);
            String appName = context.getPackageManager().getApplicationLabel(appInfo).toString();
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
            String versionName = packageInfo.versionName;
            int versionCode = packageInfo.versionCode;

            if (!TextUtils.isEmpty(stagingURL)) {
                eventsURL = stagingURL;
            }

            if (!TextUtils.isEmpty(stagingAccessToken)) {
                this.accessToken = stagingAccessToken;
            }

            // Build User Agent
            if (!TextUtils.isEmpty(appName) && !TextUtils.isEmpty(versionName)) {
                userAgent = appName + "/" + versionName + "/" + versionCode + " " + userAgent;
            }

        } catch (Exception e) {
            Log.e(TAG, "Error Trying to load Staging Credentials: " + e.toString());
        }

        // Register for battery updates
        IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        batteryStatus = context.registerReceiver(null, ifilter);
    }

    public static MapboxEventManager getMapboxEventManager(@NonNull Context context) {
        if (mapboxEventManager == null) {
            mapboxEventManager = new MapboxEventManager(context);
        }
        return mapboxEventManager;
    }

    public void addLocationEvent(Location location) {
        // Add Location even to queue
        Hashtable<String, Object> event = new Hashtable<>();
        event.put(MapboxEvent.KEY_LATITUDE, location.getLatitude());
        event.put(MapboxEvent.KEY_LONGITUDE, location.getLongitude());
        event.put(MapboxEvent.KEY_SPEED, location.getSpeed());
        event.put(MapboxEvent.KEY_COURSE, location.getBearing());
        event.put(MapboxEvent.KEY_ALTITUDE, location.getAltitude());
        event.put(MapboxEvent.KEY_HORIZONTAL_ACCURACY, location.getAccuracy());
        event.put(MapboxEvent.ATTRIBUTE_CREATED, dateFormat.format(new Date()));
        event.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_LOCATION);

        events.add(event);

        // If size > 1 then flush
        if (events.size() >= 1) {
            new FlushTheEventsTask().execute();
        }

    }

    private String getOrientation() {
        switch (context.getResources().getConfiguration().orientation) {
            case Configuration.ORIENTATION_LANDSCAPE:
                return "Landscape";
            case Configuration.ORIENTATION_PORTRAIT:
                return "Portrait";
            default:
                return "Undefined";
        }
    }

    private int getBatteryLevel() {
        int level = batteryStatus.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = batteryStatus.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

        return Math.round((level / (float)scale) * 100);
    }

    private String getApplicationState() {

        ActivityManager activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningAppProcessInfo> appProcesses = activityManager.getRunningAppProcesses();
        if (appProcesses == null) {
            return "Unknown";
        }
        final String packageName = context.getPackageName();
        for (ActivityManager.RunningAppProcessInfo appProcess : appProcesses) {
            if (appProcess.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND && appProcess.processName.equals(packageName)) {
                return "Foreground";
            }
        }
        return "Background";
    }

    private class FlushTheEventsTask extends AsyncTask<Void, Void, Void> {

        @Override
        protected Void doInBackground(Void... voids) {

            // Check for NetworkConnectivity
            ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo networkInfo = cm.getActiveNetworkInfo();
            if (networkInfo == null || !networkInfo.isConnected()) {
                Log.w(TAG, "Not connected to network, so returning without attempting to send events");
                return null;
            }

            try {
                // Send data
                // =========
                JSONArray jsonArray = new JSONArray();

                for (Hashtable<String, Object> evt : events) {
                    JSONObject jsonObject = new JSONObject();
                    jsonObject.put(MapboxEvent.KEY_LATITUDE, evt.get(MapboxEvent.KEY_LATITUDE));
                    jsonObject.put(MapboxEvent.KEY_LONGITUDE, evt.get(MapboxEvent.KEY_LONGITUDE));
                    jsonObject.put(MapboxEvent.KEY_SPEED, evt.get(MapboxEvent.KEY_SPEED));
                    jsonObject.put(MapboxEvent.KEY_COURSE, evt.get(MapboxEvent.KEY_COURSE));
                    jsonObject.put(MapboxEvent.KEY_ALTITUDE, evt.get(MapboxEvent.KEY_ALTITUDE));
                    jsonObject.put(MapboxEvent.KEY_HORIZONTAL_ACCURACY, evt.get(MapboxEvent.KEY_HORIZONTAL_ACCURACY));

                    // Basic Event Meta Data
                    jsonObject.put(MapboxEvent.ATTRIBUTE_EVENT, evt.get("event"));
                    jsonObject.put(MapboxEvent.ATTRIBUTE_VERSION, MapboxEvent.VERSION_NUMBER);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_CREATED, evt.get("created"));
                    jsonObject.put(MapboxEvent.ATTRIBUTE_INSTANCE, SESSION_UUID);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_VENDOR_ID, "");
                    jsonObject.put(MapboxEvent.ATTRIBUTE_APP_BUNDLE_ID, context.getPackageName());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_MODEL, Build.MODEL);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM, Build.VERSION.RELEASE);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_ORIENTATION, getOrientation());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL, getBatteryLevel());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_APPLICATION_STATE, getApplicationState());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_RESOLUTION, "");
                    jsonObject.put(MapboxEvent.ATTRIBUTE_ACCESSIBILITY_FONT_SCALE, "");
                    jsonObject.put(MapboxEvent.ATTRIBUTE_CARRIER, "");
                    jsonObject.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE, "");
                    jsonObject.put(MapboxEvent.ATTRIBUTE_WIFI, "");

                    jsonArray.put(jsonObject);
                }

                OkHttpClient client = new OkHttpClient();
                RequestBody body = RequestBody.create(JSON, jsonArray.toString());

                String url = eventsURL + "/events/v1?access_token=" + accessToken;
                Log.d(TAG, "url = " + url);

                Request request = new Request.Builder()
                        .url(url)
                        .header("User-Agent", userAgent)
                        .post(body)
                        .build();
                Response response = client.newCall(request).execute();
                Log.i(TAG, "Response Code from Mapbox Events Server: " + response.code());

                // Reset Events
                // ============
                events.removeAllElements();
            } catch (Exception e) {
                Log.e(TAG, "FlushTheEventsTask borked: " + e);
            }

            return null;
        }

    }

}
