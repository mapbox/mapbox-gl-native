package com.mapbox.mapboxsdk.telemetry;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.location.Location;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.BatteryManager;
import android.os.Build;
import android.support.annotation.NonNull;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.WindowManager;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import org.json.JSONArray;
import org.json.JSONObject;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Hashtable;
import java.util.List;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;
import java.util.Vector;
import okhttp3.CertificatePinner;
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

    private String userAgent = MapboxEvent.MGLMapboxEventsUserAgent;

    private Intent batteryStatus = null;

    private DisplayMetrics displayMetrics = null;

    private String mapboxVendorId = null;

    private String mapboxSessionId = null;
    private long mapboxSessionIdLastSet = 0;
    private static long hourInMillis = 1000 * 60 * 60;
    private static long flushDelayInMillis = 1000 * 60 * 2;  // 2 Minutes
    private static final int SESSION_ID_ROTATION_HOURS = 24;

    private static MessageDigest messageDigest = null;

    private Timer timer = null;

    private MapboxEventManager(@NonNull Context context) {
        super();
        this.accessToken = ApiAccess.getToken(context);
        this.context = context;

        // Setup Message Digest
        try {
            messageDigest = MessageDigest.getInstance("SHA-1");
        } catch (NoSuchAlgorithmException e) {
            Log.w(TAG, "Error getting Encryption Algorithm: " + e);
        }

        // Load / Create Vendor Id
        SharedPreferences prefs = context.getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES_FILE, Context.MODE_PRIVATE);
        if (prefs.contains(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_VENDORID)) {
            mapboxVendorId = prefs.getString(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_VENDORID, "Default Value");
            Log.i(TAG, "Found Vendor Id = " + mapboxVendorId);
        } else {
            String vendorId = UUID.randomUUID().toString();
            vendorId = encodeString(vendorId);
            SharedPreferences.Editor editor = prefs.edit();
            editor.putString(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_VENDORID, vendorId);
            editor.apply();
            editor.commit();
            Log.i(TAG, "Set New Vendor Id = " + vendorId);
        }

        // Create Initial Session Id
        rotateSessionId();

        // Get DisplayMetrics Setup
        displayMetrics = new DisplayMetrics();
        ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getMetrics(displayMetrics);

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

        // Manage Timer Flush
        timer = new Timer();
        timer.schedule(new FlushEventsTimerTask(), 1, flushDelayInMillis);
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

        rotateSessionId();
    }

    private String encodeString(String string) {
        try {
            if (messageDigest != null) {
                messageDigest.reset();
                messageDigest.update(string.getBytes("UTF-8"));
                byte[] bytes = messageDigest.digest();
                return new String(bytes);
            }
        } catch (Exception e) {
            Log.w(TAG, "Error encoding string, will return in original form." + e);
        }
        return string;
    }

    private void rotateSessionId() {
        long now = System.currentTimeMillis();
        if (now - mapboxSessionIdLastSet > (SESSION_ID_ROTATION_HOURS * hourInMillis)) {
            mapboxSessionId = UUID.randomUUID().toString();
            mapboxSessionIdLastSet = System.currentTimeMillis();
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

    private float getAccesibilityFontScaleSize() {
        // Values
        // Small = 0.85
        // Normal = 1.0
        // Large = 1.15
        // Huge = 1.3

        return context.getResources().getConfiguration().fontScale;
    }

    private String getCellularCarrier() {
        TelephonyManager manager = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
        String carrierName = manager.getNetworkOperatorName();
        if (TextUtils.isEmpty(carrierName)) {
            carrierName = "None";
        }
        return carrierName;
    }

    private String getCellularNetworkType () {
        TelephonyManager manager = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
        switch (manager.getNetworkType()) {
            case TelephonyManager.NETWORK_TYPE_1xRTT:
                return "1xRTT";
            case TelephonyManager.NETWORK_TYPE_CDMA:
                return "CDMA";
            case TelephonyManager.NETWORK_TYPE_EDGE:
                return "EDGE";
            case TelephonyManager.NETWORK_TYPE_EHRPD:
                return "EHRPD";
            case TelephonyManager.NETWORK_TYPE_EVDO_0:
                return "EVDO_0";
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
                return "EVDO_A";
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
                return "EVDO_B";
            case TelephonyManager.NETWORK_TYPE_GPRS:
                return "GPRS";
            case TelephonyManager.NETWORK_TYPE_HSDPA:
                return "HSDPA";
            case TelephonyManager.NETWORK_TYPE_HSPA:
                return "HSPA";
            case TelephonyManager.NETWORK_TYPE_HSPAP:
                return "HSPAP";
            case TelephonyManager.NETWORK_TYPE_HSUPA:
                return "HSUPA";
            case TelephonyManager.NETWORK_TYPE_IDEN:
                return "IDEN";
            case TelephonyManager.NETWORK_TYPE_LTE:
                return "LTE";
            case TelephonyManager.NETWORK_TYPE_UMTS:
                return "UMTS";
            case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                return "Unknown";
            default:
                return "Default Unknown";
        }
    }


    public String getConnectedToWifi() {

        String status = "No";
        WifiManager wifiMgr = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        if (wifiMgr.isWifiEnabled()) {
            try {
                WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
                if( wifiInfo.getNetworkId() != -1 ){
                    status = "Yes";
                }
            } catch (Exception e) {
                Log.w(TAG, "Error getting Wifi Connection Status: " + e);
                status = "Unknown";
            }
        }

        return status;
    }

    private class FlushTheEventsTask extends AsyncTask<Void, Void, Void> {

        @Override
        protected Void doInBackground(Void... voids) {

            if (events.size() < 1) {
                Log.i(TAG, "No events in the queue to send so returning.");
                return null;
            }

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
                    jsonObject.put(MapboxEvent.ATTRIBUTE_SESSION_ID, encodeString(mapboxSessionId));
                    jsonObject.put(MapboxEvent.ATTRIBUTE_VERSION, MapboxEvent.VERSION_NUMBER);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_CREATED, evt.get("created"));
                    jsonObject.put(MapboxEvent.ATTRIBUTE_VENDOR_ID, mapboxVendorId);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_APP_BUNDLE_ID, context.getPackageName());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_MODEL, Build.MODEL);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM, Build.VERSION.RELEASE);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_ORIENTATION, getOrientation());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL, getBatteryLevel());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_APPLICATION_STATE, getApplicationState());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_RESOLUTION, displayMetrics.density);
                    jsonObject.put(MapboxEvent.ATTRIBUTE_ACCESSIBILITY_FONT_SCALE, getAccesibilityFontScaleSize());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_CARRIER, getCellularCarrier());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE, getCellularNetworkType());
                    jsonObject.put(MapboxEvent.ATTRIBUTE_WIFI, getConnectedToWifi());

                    jsonArray.put(jsonObject);
                }

                // Based on http://square.github.io/okhttp/3.x/okhttp/okhttp3/CertificatePinner.html
                CertificatePinner certificatePinner = new CertificatePinner.Builder()
                        .add("cloudfront-staging.tilestream.net", "sha1/KcdiTca54HxWTV8VuAd67x8I=")
                        .add("cloudfront-staging.tilestream.net", "sha1//KDE76PP0DQBDcTnMFBv+efp4eg=")
                        .build();

                OkHttpClient client = new OkHttpClient.Builder().certificatePinner(certificatePinner).build();
                RequestBody body = RequestBody.create(JSON, jsonArray.toString());

                String url = eventsURL + "/events/v1?access_token=" + accessToken;
                Log.d(TAG, "url = " + url);

                Request request = new Request.Builder()
                        .url(url)
                        .header("User-Agent", userAgent)
                        .post(body)
                        .build();
                Response response = client.newCall(request).execute();
                Log.i(TAG, "Response Code from Mapbox Events Server: " + response.code() + " for " + events.size() + " events sent in.");

                // Reset Events
                // ============
                events.removeAllElements();
            } catch (Exception e) {
                Log.e(TAG, "FlushTheEventsTask borked: " + e);
            }

            return null;
        }

    }


    private class FlushEventsTimerTask extends TimerTask {
        /**
         * The task to run should be specified in the implementation of the {@code run()}
         * method.
         */
        @Override
        public void run() {
            new FlushTheEventsTask().execute();
        }
    }
}
