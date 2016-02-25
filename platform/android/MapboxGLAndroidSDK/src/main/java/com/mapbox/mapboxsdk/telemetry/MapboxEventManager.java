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
import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.location.LocationServices;
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

    private boolean telemetryEnabled;

    private final Vector<Hashtable<String, Object>> events = new Vector<>();
    private static final MediaType JSON  = MediaType.parse("application/json; charset=utf-8");
    private static SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSZ", Locale.US);

    private Context context = null;
    private String accessToken = null;
    private String eventsURL = MapboxEvent.MAPBOX_EVENTS_BASE_URL;

    private String userAgent = BuildConfig.MAPBOX_EVENTS_USER_AGENT_BASE;

    private Intent batteryStatus = null;
    private final String operatingSystem = "Android - " + Build.VERSION.RELEASE;

    private DisplayMetrics displayMetrics = null;

    private String mapboxVendorId = null;

    private String mapboxSessionId = null;
    private long mapboxSessionIdLastSet = 0;
    private static long hourInMillis = 1000 * 60 * 60;
    private static long flushDelayInitialInMillis = 1000 * 10;  // 10 Seconds
    private static long flushDelayInMillis = 1000 * 60 * 2;  // 2 Minutes
    private static final int SESSION_ID_ROTATION_HOURS = 24;

    private static MessageDigest messageDigest = null;

    private Timer timer = null;

    private MapboxEventManager(@NonNull Context context, String accessToken) {
        super();
        this.accessToken = accessToken;
        this.context = context;

        // Setup Message Digest
        try {
            messageDigest = MessageDigest.getInstance("SHA-1");
        } catch (NoSuchAlgorithmException e) {
            Log.w(TAG, "Error getting Encryption Algorithm: " + e);
        }

        SharedPreferences prefs = context.getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES_FILE, Context.MODE_PRIVATE);

        // Determine if Telemetry Should Be Enabled
        setTelemetryEnabled(prefs.getBoolean(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_TELEMETRY_ENABLED, true));

        // Load / Create Vendor Id
        if (prefs.contains(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_VENDORID)) {
            mapboxVendorId = prefs.getString(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_VENDORID, "");
        }
        if (TextUtils.isEmpty(mapboxVendorId)) {
            String vendorId = UUID.randomUUID().toString();
            mapboxVendorId = encodeString(vendorId);
            SharedPreferences.Editor editor = prefs.edit();
            editor.putString(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_VENDORID, mapboxVendorId);
            editor.apply();
            editor.commit();
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
        IntentFilter iFilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        batteryStatus = context.registerReceiver(null, iFilter);
    }

    /**
     * Internal setup of MapboxEventsManager.  It needs to be called once before @link MapboxEventManager#getMapboxEventManager
     *
     * This allows for a cleaner getMapboxEventManager() that doesn't require context and accessToken
     *
     * @param context Context
     * @param accessToken Access Token
     */
    public static void configureAndStartMapboxEventManager(@NonNull Context context, @NonNull String accessToken) {
        if (mapboxEventManager != null) {
            Log.w(TAG, "Singleton has already been created.");
            return;
        }
        mapboxEventManager = new MapboxEventManager(context.getApplicationContext(), accessToken);
    }

    /**
     * Primary Access method using Singleton pattern
     * @return MapboxEventManager
     */
    public static MapboxEventManager getMapboxEventManager() {
        return mapboxEventManager;
    }

    public static String generateCreateDate() {
        return dateFormat.format(new Date());
    }

    public boolean isTelemetryEnabled() {
        return telemetryEnabled;
    }

    /**
     * Enables / Disables Telemetry
     * @param telemetryEnabled True to start telemetry, false to stop it
     */
    public void setTelemetryEnabled(boolean telemetryEnabled) {
        if (this.telemetryEnabled == telemetryEnabled) {
            Log.d(TAG, "No need to start / stop telemetry as it's already in that state.");
            return;
        }

        if (telemetryEnabled) {
            Log.d(TAG, "Starting Telemetry Up!");
            // Start It Up
            context.startService(new Intent(context, TelemetryService.class));

            // Make sure Ambient Mode is started at a minimum
            if (LocationServices.getLocationServices(context).isGPSEnabled()) {
                LocationServices.getLocationServices(context).toggleGPS(false);
            }

            // Manage Timer Flush
            timer = new Timer();
            timer.schedule(new FlushEventsTimerTask(), flushDelayInitialInMillis, flushDelayInMillis);
        } else {
            Log.d(TAG, "Shutting Telemetry Down");
            // Shut It Down
            events.removeAllElements();
            context.stopService(new Intent(context, TelemetryService.class));

            if (timer != null) {
                timer.cancel();
                timer = null;
            }
        }

        // Persist
        this.telemetryEnabled = telemetryEnabled;
        SharedPreferences prefs = context.getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES_FILE, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putBoolean(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_TELEMETRY_ENABLED, telemetryEnabled);
        editor.apply();
        editor.commit();
    }

    /**
     * Adds a Location Event to the system for processing
     * @param location Location event
     */
    public void addLocationEvent(Location location) {
        // Add Location even to queue
        Hashtable<String, Object> event = new Hashtable<>();
        event.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_LOCATION);
        event.put(MapboxEvent.ATTRIBUTE_CREATED, generateCreateDate());
        event.put(MapboxEvent.ATTRIBUTE_SOURCE, MapboxEvent.SOURCE_MAPBOX);
        event.put(MapboxEvent.ATTRIBUTE_SESSION_ID, encodeString(mapboxSessionId));
        event.put(MapboxEvent.KEY_LATITUDE, location.getLatitude());
        event.put(MapboxEvent.KEY_LONGITUDE, location.getLongitude());
        event.put(MapboxEvent.KEY_ALTITUDE, location.getAltitude());
        event.put(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM, operatingSystem);
        event.put(MapboxEvent.ATTRIBUTE_APPLICATION_STATE, getApplicationState());

        events.add(event);

        rotateSessionId();
    }

    /**
     * Push Interactive Events to the system for processing
     * @param eventWithAttributes Event with attributes
     */
    public void pushEvent(Hashtable<String, Object> eventWithAttributes) {

        if (eventWithAttributes == null) {
            return;
        }

        String eventType = (String)eventWithAttributes.get(MapboxEvent.ATTRIBUTE_EVENT);
        if (TextUtils.isEmpty(eventType)) {
            return;
        }

        if (eventType.equalsIgnoreCase(MapboxEvent.TYPE_MAP_LOAD)) {
            // Map Load Data Model
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_USERID, mapboxVendorId);
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_MODEL, Build.MODEL);
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM, operatingSystem);
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_RESOLUTION, displayMetrics.density);
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_ACCESSIBILITY_FONT_SCALE, getAccesibilityFontScaleSize());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_ORIENTATION, getOrientation());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL, getBatteryLevel());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_PLUGGED_IN, isPluggedIn());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_CARRIER, getCellularCarrier());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE, getCellularNetworkType());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_WIFI, getConnectedToWifi());

            // Put Map Load on events before Turnstile clears it
            events.add(eventWithAttributes);

            // Turnstile
            pushTurnstileEvent();

            // Return immediately to avoid double adding of event
            return;

        } else if (eventType.equalsIgnoreCase(MapboxEvent.TYPE_MAP_CLICK)) {
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_ORIENTATION, getOrientation());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL, getBatteryLevel());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_PLUGGED_IN, isPluggedIn());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_CARRIER, getCellularCarrier());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE, getCellularNetworkType());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_WIFI, getConnectedToWifi());
        } else if (eventType.equalsIgnoreCase(MapboxEvent.TYPE_MAP_DRAGEND)) {
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_ORIENTATION, getOrientation());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL, getBatteryLevel());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_PLUGGED_IN, isPluggedIn());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_CARRIER, getCellularCarrier());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE, getCellularNetworkType());
            eventWithAttributes.put(MapboxEvent.ATTRIBUTE_WIFI, getConnectedToWifi());
        } else {
            Log.w(TAG, "This is not an event type in the Events Data Model.");
            return;
        }

       events.add(eventWithAttributes);
    }

    /**
     * Pushes turnstile event for internal billing purposes
     */
    private void pushTurnstileEvent() {

        Hashtable<String, Object> event = new Hashtable<>();
        event.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_TURNSTILE);
        event.put(MapboxEvent.ATTRIBUTE_CREATED, generateCreateDate());
        event.put(MapboxEvent.ATTRIBUTE_USERID, mapboxVendorId);
        event.put(MapboxEvent.ATTRIBUTE_ENABLED_TELEMETRY, telemetryEnabled);

        events.add(event);

        // Send to Server Immediately
        new FlushTheEventsTask().execute();
        Log.d(TAG, "turnstile event pushed.");
    }

    /**
     * SHA-1 Encoding for strings
     * @param string String to encode
     * @return String encoded if no error, original string if error
     */
    private String encodeString(String string) {
        try {
            if (messageDigest != null) {
                messageDigest.reset();
                messageDigest.update(string.getBytes("UTF-8"));
                byte[] bytes = messageDigest.digest();

                // Get the Hex version of the digest
                StringBuilder sb = new StringBuilder();
                for (byte b : bytes) {
                    sb.append( String.format("%02X", b) );
                }
                String hex = sb.toString();

                return hex;
            }
        } catch (Exception e) {
            Log.w(TAG, "Error encoding string, will return in original form." + e);
        }
        return string;
    }

    /**
     * Changes Session Id based on time boundary
     */
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
                return "";
        }
    }

    private int getBatteryLevel() {
        int level = batteryStatus.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = batteryStatus.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

        return Math.round((level / (float)scale) * 100);
    }

    /**
     * Determine if device is plugged in to power via USB or AC or not.
     *
     * http://developer.android.com/reference/android/os/BatteryManager.html#EXTRA_PLUGGED
     * @return true if plugged in, false if not
     */
    private boolean isPluggedIn() {

        int chargePlug = batteryStatus.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1);
        if (chargePlug == BatteryManager.BATTERY_PLUGGED_USB || chargePlug == BatteryManager.BATTERY_PLUGGED_AC) {
            return true;
        }

        return false;
    }

    private String getApplicationState() {

        ActivityManager activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningAppProcessInfo> appProcesses = activityManager.getRunningAppProcesses();
        if (appProcesses == null) {
            return "";
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
            carrierName = "";
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
                return "";
        }
    }

    public Boolean getConnectedToWifi() {

        Boolean status = false;
        WifiManager wifiMgr = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        if (wifiMgr.isWifiEnabled()) {
            try {
                WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
                if (wifiInfo.getNetworkId() != -1){
                    status = true;
                }
            } catch (Exception e) {
                Log.w(TAG, "Error getting Wifi Connection Status: " + e);
                status = false;
            }
        }

        return status;
    }


    /**
     * Task responsible for converting stored events and sending them to the server
     */
    private class FlushTheEventsTask extends AsyncTask<Void, Void, Void> {

        @Override
        protected Void doInBackground(Void... voids) {

             if (events.size() < 1) {
                Log.d(TAG, "No events in the queue to send so returning.");
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

                    // Build the JSON but only if there's a value for it in the evt
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_EVENT, evt.get(MapboxEvent.ATTRIBUTE_EVENT));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_CREATED, evt.get(MapboxEvent.ATTRIBUTE_CREATED));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_USERID, evt.get(MapboxEvent.ATTRIBUTE_USERID));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_ENABLED_TELEMETRY, evt.get(MapboxEvent.ATTRIBUTE_ENABLED_TELEMETRY));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_SOURCE, evt.get(MapboxEvent.ATTRIBUTE_SOURCE));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_SESSION_ID, evt.get(MapboxEvent.ATTRIBUTE_SESSION_ID));
                    jsonObject.putOpt(MapboxEvent.KEY_LATITUDE, evt.get(MapboxEvent.KEY_LATITUDE));
                    jsonObject.putOpt(MapboxEvent.KEY_LONGITUDE, evt.get(MapboxEvent.KEY_LONGITUDE));
                    jsonObject.putOpt(MapboxEvent.KEY_ALTITUDE, evt.get(MapboxEvent.KEY_ALTITUDE));
                    jsonObject.putOpt(MapboxEvent.KEY_ZOOM, evt.get(MapboxEvent.KEY_ZOOM));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM, evt.get(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_APPLICATION_STATE, evt.get(MapboxEvent.ATTRIBUTE_APPLICATION_STATE));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_USERID, evt.get(MapboxEvent.ATTRIBUTE_USERID));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_MODEL, evt.get(MapboxEvent.ATTRIBUTE_MODEL));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_RESOLUTION, evt.get(MapboxEvent.ATTRIBUTE_RESOLUTION));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_ACCESSIBILITY_FONT_SCALE, evt.get(MapboxEvent.ATTRIBUTE_ACCESSIBILITY_FONT_SCALE));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL, evt.get(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_PLUGGED_IN, evt.get(MapboxEvent.ATTRIBUTE_PLUGGED_IN));
                    jsonObject.putOpt(MapboxEvent.ATTRIBUTE_WIFI, evt.get(MapboxEvent.ATTRIBUTE_WIFI));

                    // Special Cases where empty string is denoting null and therefore should not be sent at all
                    // This arises as thread safe Hashtable does not accept null values (nor keys)
                    if (evt.containsKey(MapboxEvent.ATTRIBUTE_ORIENTATION)) {
                        String orientation =  (String)evt.get(MapboxEvent.ATTRIBUTE_ORIENTATION);
                        if (!TextUtils.isEmpty(orientation)) {
                            jsonObject.putOpt(MapboxEvent.ATTRIBUTE_ORIENTATION, orientation);
                        }
                    }
                    if (evt.containsKey(MapboxEvent.ATTRIBUTE_CARRIER)) {
                        String carrier =  (String)evt.get(MapboxEvent.ATTRIBUTE_CARRIER);
                        if (!TextUtils.isEmpty(carrier)) {
                            jsonObject.putOpt(MapboxEvent.ATTRIBUTE_CARRIER, carrier);
                        }
                    }

                    // Special Cases where null has to be passed if no value exists
                    // Requires using put() instead of putOpt()
                    String eventType = (String)evt.get(MapboxEvent.ATTRIBUTE_EVENT);
                    if (!TextUtils.isEmpty(eventType) && eventType.equalsIgnoreCase(MapboxEvent.TYPE_MAP_CLICK)) {
                        jsonObject.put(MapboxEvent.KEY_GESTURE_ID, evt.get(MapboxEvent.KEY_GESTURE_ID));
                    }
                    if (evt.containsKey(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE)) {
                        String cellularNetworkType = (String)evt.get(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE);
                        if (TextUtils.isEmpty(cellularNetworkType)) {
                            jsonObject.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE, null);
                        } else {
                            jsonObject.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE, evt.get(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE));
                        }
                    }

                    jsonArray.put(jsonObject);
                }

                // Based on http://square.github.io/okhttp/3.x/okhttp/okhttp3/CertificatePinner.html
                CertificatePinner certificatePinner = new CertificatePinner.Builder()
                        .add("cloudfront-staging.tilestream.net", "sha1/KcdiTca54HxWTV8VuAd67x8I=")
                        .add("cloudfront-staging.tilestream.net", "sha1//KDE76PP0DQBDcTnMFBv+efp4eg=")
                        .add("api.mapbox.com", "sha1/Uv71ooi32pyba+oLD7egnXm7/GQ=")
                        .add("api.mapbox.com", "sha1/hOP0d37/ZTSGgCSseE3DIZ1uSg0=")
                        .build();

                OkHttpClient client = new OkHttpClient.Builder().certificatePinner(certificatePinner).build();
                RequestBody body = RequestBody.create(JSON, jsonArray.toString());

                String url = eventsURL + "/events/v2?access_token=" + accessToken;

                Request request = new Request.Builder()
                        .url(url)
                        .header("User-Agent", userAgent)
                        .post(body)
                        .build();
                Response response = client.newCall(request).execute();

                // Reset Events
                // ============
                events.removeAllElements();
            } catch (Exception e) {
                Log.e(TAG, "FlushTheEventsTask borked: " + e);
            }

            return null;
        }

    }


    /**
     * TimerTask responsible for sending event data to server
     */
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
