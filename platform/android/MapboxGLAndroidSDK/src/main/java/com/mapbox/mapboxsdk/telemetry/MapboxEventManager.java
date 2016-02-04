package com.mapbox.mapboxsdk.telemetry;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.location.Location;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
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
import java.util.Locale;
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

    private MapboxEventManager(@NonNull Context context) {
        super();
        this.accessToken = ApiAccess.getToken(context);
        this.context = context;

        // Check for Staging Server Information
        try {
            ApplicationInfo appInfo = context.getPackageManager().getApplicationInfo(context.getPackageName(), PackageManager.GET_META_DATA);
            String stagingURL = appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_STAGING_SERVER);
            String stagingAccessToken = appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_STAGING_ACCESS_TOKEN);

            if (!TextUtils.isEmpty(stagingURL)) {
                eventsURL = stagingURL;
            }

            if (!TextUtils.isEmpty(stagingAccessToken)) {
                this.accessToken = stagingAccessToken;
            }

        } catch (Exception e) {
            Log.e(TAG, "Error Trying to load Staging Credentials: " + e.toString());
        }
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
        event.put(MapboxEvent.MGLEventKeyLatitude, location.getLatitude());
        event.put(MapboxEvent.MGLEventKeyLongitude, location.getLongitude());
        event.put(MapboxEvent.MGLEventKeySpeed, location.getSpeed());
        event.put(MapboxEvent.MGLEventKeyCourse, location.getBearing());
        event.put(MapboxEvent.MGLEventKeyAltitude, location.getAltitude());
        event.put(MapboxEvent.MGLEventKeyHorizontalAccuracy, location.getAccuracy());
        event.put("created", dateFormat.format(new Date()));
        event.put("event", MapboxEvent.MAPBOXEVENT_LOCATION);

        events.add(event);

        // If size > 1 then flush
        if (events.size() >= 1) {
            new FlushTheEventsTask().execute();
        }

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
                    jsonObject.put(MapboxEvent.MGLEventKeyLatitude, evt.get(MapboxEvent.MGLEventKeyLatitude));
                    jsonObject.put(MapboxEvent.MGLEventKeyLongitude, evt.get(MapboxEvent.MGLEventKeyLongitude));
                    jsonObject.put(MapboxEvent.MGLEventKeySpeed, evt.get(MapboxEvent.MGLEventKeySpeed));
                    jsonObject.put(MapboxEvent.MGLEventKeyCourse, evt.get(MapboxEvent.MGLEventKeyCourse));
                    jsonObject.put(MapboxEvent.MGLEventKeyAltitude, evt.get(MapboxEvent.MGLEventKeyAltitude));
                    jsonObject.put(MapboxEvent.MGLEventKeyHorizontalAccuracy, evt.get(MapboxEvent.MGLEventKeyHorizontalAccuracy));

                    // Basic Event Meta Data
                    jsonObject.put("event", evt.get("event"));
                    jsonObject.put("version", MapboxEvent.VERSION);
                    jsonObject.put("instance", "FFFFFFFF-GGGG-HHHH-IIII-JJJJJJJJJJJJ");
                    jsonObject.put("created", evt.get("created"));
                    jsonObject.put("anonid", "AAAAAAAA-BBBB-CCCC-DDDD-EEEEEEEEEEEE");
                    jsonObject.put("appBundleId", context.getPackageName());

                    jsonArray.put(jsonObject);
                }

                OkHttpClient client = new OkHttpClient();
                RequestBody body = RequestBody.create(JSON, jsonArray.toString());

                String url = eventsURL + "/events/v1?access_token=" + accessToken;
                Log.d(TAG, "url = " + url);

                Request request = new Request.Builder()
                        .url(url)
                        .header("User-Agent", MapboxEvent.MGLMapboxEventsUserAgent)
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
