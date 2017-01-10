package com.mapbox.mapboxsdk.telemetry;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ServiceInfo;
import android.content.res.Configuration;
import android.location.Location;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.constants.GeoConstants;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.TelemetryServiceNotConfiguredException;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.utils.MathUtils;

import org.json.JSONArray;
import org.json.JSONObject;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Hashtable;
import java.util.List;
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
import okhttp3.internal.Util;
import timber.log.Timber;

/**
 * Singleton control center for managing Telemetry Data.
 * Primary access is via MapboxEventManager.getMapboxEventManager()
 */
public class MapboxEventManager {

  private static MapboxEventManager mapboxEventManager = null;

  private boolean initialized = false;
  private boolean stagingEnv;
  private boolean telemetryEnabled;

  private final Vector<Hashtable<String, Object>> events = new Vector<>();
  private static final MediaType JSON = MediaType.parse("application/json; charset=utf-8");
  private static SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSZ",
    MapboxConstants.MAPBOX_LOCALE);

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
  private static long flushDelayInMillis = 1000 * 60 * 3;  // 3 Minutes
  private static final int SESSION_ID_ROTATION_HOURS = 24;

  private static final int FLUSH_EVENTS_CAP = 1000;

  private static MessageDigest messageDigest = null;

  private static final double locationEventAccuracy = 10000000;

  private Timer timer = null;

  /**
   * Private Constructor for configuring the single instance per app.
   */
  private MapboxEventManager() {
    super();
  }

  /**
   * Internal setup of MapboxEventsManager.  It needs to be called once before @link
   * MapboxEventManager#getMapboxEventManager
   * <p>
   * This allows for a cleaner getMapboxEventManager() that doesn't require context and accessToken
   *
   * @param context     The context associated with MapView
   * @param accessToken The accessToken to load MapView
   */
  public void initialize(@NonNull Context context, @NonNull String accessToken) {

    //Timber.i("Telemetry initialize() called...");

    if (initialized) {
      //Timber.i("Mapbox Telemetry has already been initialized.");
      return;
    }

    this.context = context.getApplicationContext();
    this.accessToken = accessToken;

    validateTelemetryServiceConfigured();

    // Setup Message Digest
    try {
      messageDigest = MessageDigest.getInstance("SHA-1");
    } catch (NoSuchAlgorithmException exception) {
      Timber.w("Error getting Encryption Algorithm: ", exception);
    }

    // Create Initial Session Id
    rotateSessionId();

    SharedPreferences prefs = context.getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES_FILE,
      Context.MODE_PRIVATE);

    // Determine if Telemetry Should Be Enabled
    ///Timber.i("Right before Telemetry set enabled in initialized()");
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

    // Get DisplayMetrics Setup
    displayMetrics = new DisplayMetrics();
    ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getMetrics(displayMetrics);

    // Check for Staging Server Information
    try {
      ApplicationInfo appInfo = context.getPackageManager().getApplicationInfo(context.getPackageName(),
        PackageManager.GET_META_DATA);
      String stagingURL = appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_STAGING_SERVER);
      String stagingAccessToken = appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_STAGING_ACCESS_TOKEN);

      if (TextUtils.isEmpty(stagingURL) || TextUtils.isEmpty(stagingAccessToken)) {
        //Timber.d("Looking in SharedPreferences for Staging Credentials");
        stagingURL = prefs.getString(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_TELEMETRY_STAGING_URL, null);
        stagingAccessToken = prefs.getString(
          MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_TELEMETRY_STAGING_ACCESS_TOKEN, null);
      }

      if (!TextUtils.isEmpty(stagingURL) && !TextUtils.isEmpty(stagingAccessToken)) {
        eventsURL = stagingURL;
        this.accessToken = accessToken;
        stagingEnv = true;
      }

      // Build User Agent
      String appIdentifier = getApplicationIdentifier();
      if (TextUtils.equals(userAgent, BuildConfig.MAPBOX_EVENTS_USER_AGENT_BASE) && !TextUtils.isEmpty(appIdentifier)) {
        userAgent = Util
          .toHumanReadableAscii(String.format(MapboxConstants.MAPBOX_LOCALE, "%s %s", appIdentifier, userAgent));
      }

    } catch (Exception exception) {
      //Timber.e("Error Trying to load Staging Credentials: ", exception);
    }

    // Register for battery updates
    IntentFilter iFilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
    batteryStatus = context.registerReceiver(null, iFilter);

    initialized = true;
  }

  /**
   * Primary Access method using Singleton pattern
   *
   * @return MapboxEventManager
   */
  public static MapboxEventManager getMapboxEventManager() {
    if (mapboxEventManager == null) {
      mapboxEventManager = new MapboxEventManager();
    }
    return mapboxEventManager;
  }

  // Checks that TelemetryService has been configured by developer
  private void validateTelemetryServiceConfigured() {
    try {
      // Check Implementing app's AndroidManifest.xml
      PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(),
        PackageManager.GET_SERVICES);
      if (packageInfo.services != null) {
        for (ServiceInfo service : packageInfo.services) {
          if (TextUtils.equals("com.mapbox.mapboxsdk.telemetry.TelemetryService", service.name)) {
            return;
          }
        }
      }
    } catch (Exception exception) {
      //Timber.w("Error checking for Telemetry Service Config: ", exception);
    }
    throw new TelemetryServiceNotConfiguredException();
  }

  public static String generateCreateDate() {
    return dateFormat.format(new Date());
  }

  public boolean isTelemetryEnabled() {
    return telemetryEnabled;
  }

  /**
   * Enables / Disables Telemetry
   *
   * @param telemetryEnabled True to start telemetry, false to stop it
   */
  public void setTelemetryEnabled(boolean telemetryEnabled) {
    //Timber.i("this.telemetryEnabled = " + this.telemetryEnabled + "; telemetryEnabled = " + telemetryEnabled);
    if (this.telemetryEnabled == telemetryEnabled) {
      //Timber.d("No need to start / stop telemetry as it's already in that state.");
      return;
    }

    if (telemetryEnabled) {
      //Timber.d("Starting Telemetry Up!");
      // Start It Up
      context.startService(new Intent(context, TelemetryService.class));

      // Make sure Ambient Mode is started at a minimum
      if (LocationServices.getLocationServices(context).areLocationPermissionsGranted()) {
        //Timber.i("Permissions are good, see if GPS is enabled and if not then setup Ambient.");
        if (LocationServices.getLocationServices(context).isGpsEnabled()) {
          LocationServices.getLocationServices(context).toggleGPS(false);
        }
      } else {
        // Start timer that checks for Permissions
        //Timber.i("Permissions are not good.  Need to do some looping to check on stuff.");

        final Handler permsHandler = new Handler();
        Runnable runnable = new Runnable() {

          private final ExponentialBackoffCounter exponentialBackoffCounter = new ExponentialBackoffCounter();

          @Override
          public void run() {
            if (LocationServices.getLocationServices(context).areLocationPermissionsGranted()) {
              //Timber.i("Permissions finally granted, so starting Ambient if GPS isn't already enabled");
              // Start Ambient
              if (LocationServices.getLocationServices(context).isGpsEnabled()) {
                LocationServices.getLocationServices(context).toggleGPS(false);
              }
            } else {
              // Restart Handler
              long nextWaitTime = exponentialBackoffCounter.getNextCount();
              //Timber.i("Permissions not granted yet... let's try again in " + nextWaitTime/1000 + " seconds");
              permsHandler.postDelayed(this, nextWaitTime);
            }
          }
        };
        permsHandler.postDelayed(runnable, 1000 * 10);
      }

      // Manage Timer Flush
      timer = new Timer();
      timer.schedule(new FlushEventsTimerTask(), flushDelayInitialInMillis, flushDelayInMillis);
    } else {
      //Timber.d("Shutting Telemetry Down");
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
    SharedPreferences prefs = context.getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES_FILE,
      Context.MODE_PRIVATE);
    SharedPreferences.Editor editor = prefs.edit();
    editor.putBoolean(MapboxConstants.MAPBOX_SHARED_PREFERENCE_KEY_TELEMETRY_ENABLED, telemetryEnabled);
    editor.apply();
    editor.commit();
  }

  /**
   * Immediately attempt to send all events data in the queue to the server.
   * <p>
   * NOTE: Permission set to package private to enable only telemetry code to use this.
   */
  void flushEventsQueueImmediately() {
    //Timber.i("flushEventsQueueImmediately() called...");
    new FlushTheEventsTask().execute();
  }

  /**
   * Centralized method for adding populated event to the queue allowing for cap size checking
   *
   * @param event Event to add to the Events Queue
   */
  private void putEventOnQueue(@NonNull Hashtable<String, Object> event) {
    if (event == null) {
      return;
    }
    events.add(event);
    if (events.size() == FLUSH_EVENTS_CAP) {
      //Timber.d("eventsSize == flushCap so send data.");
      flushEventsQueueImmediately();
    }
  }

  /**
   * Adds a Location Event to the system for processing
   *
   * @param location Location event
   */
  public void addLocationEvent(Location location) {

    // NaN and Infinite checks to prevent JSON errors at send to server time
    if (Double.isNaN(location.getLatitude()) || Double.isNaN(location.getLongitude())
      || Double.isNaN(location.getAltitude()) || Float.isNaN(location.getAccuracy())) {
      return;
    }

    if (Double.isInfinite(location.getLatitude()) || Double.isInfinite(location.getLongitude())
      || Double.isInfinite(location.getAltitude()) || Float.isInfinite(location.getAccuracy())) {
      return;
    }

    // Add Location even to queue
    Hashtable<String, Object> event = new Hashtable<>();
    event.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_LOCATION);
    event.put(MapboxEvent.ATTRIBUTE_CREATED, generateCreateDate());
    event.put(MapboxEvent.ATTRIBUTE_SOURCE, MapboxEvent.SOURCE_MAPBOX);
    event.put(MapboxEvent.ATTRIBUTE_SESSION_ID, encodeString(mapboxSessionId));
    event.put(MapboxEvent.KEY_LATITUDE,
      Math.floor(location.getLatitude() * locationEventAccuracy) / locationEventAccuracy);
    event.put(MapboxEvent.KEY_LONGITUDE,
      Math.floor(location.getLongitude() * locationEventAccuracy) / locationEventAccuracy);
    event.put(MapboxEvent.KEY_ALTITUDE, location.getAltitude());
    event.put(MapboxEvent.KEY_HORIZONTAL_ACCURACY, Math.round(location.getAccuracy()));
    event.put(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM, operatingSystem);
    event.put(MapboxEvent.ATTRIBUTE_APPLICATION_STATE, getApplicationState());

    putEventOnQueue(event);

    rotateSessionId();
  }

  /**
   * Push Interactive Events to the system for processing
   *
   * @param eventWithAttributes Event with attributes
   */
  public void pushEvent(Hashtable<String, Object> eventWithAttributes) {
    if (context == null || accessToken == null) {
      return;
    }

    if (eventWithAttributes == null) {
      return;
    }

    String eventType = (String) eventWithAttributes.get(MapboxEvent.ATTRIBUTE_EVENT);
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
      putEventOnQueue(eventWithAttributes);

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
      //Timber.w("This is not an event type in the Events Data Model.");
      return;
    }

    putEventOnQueue(eventWithAttributes);
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
    flushEventsQueueImmediately();
    //Timber.d("turnstile event pushed.");
  }

  /**
   * SHA-1 Encoding for strings
   *
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
          sb.append(String.format("%02X", b));
        }
        String hex = sb.toString();

        return hex;
      }
    } catch (Exception exception) {
      //Timber.w("Error encoding string, will return in original form.", exception);
    }
    return string;
  }

  /**
   * Changes Session Id based on time boundary
   */
  private void rotateSessionId() {
    long now = System.currentTimeMillis();
    if ((TextUtils.isEmpty(mapboxSessionId))
      || (now - mapboxSessionIdLastSet > (SESSION_ID_ROTATION_HOURS * hourInMillis))) {
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

    return Math.round((level / (float) scale) * 100);
  }

  /**
   * Determine if device is plugged in to power via USB or AC or not.
   * <p>
   * http://developer.android.com/reference/android/os/BatteryManager.html#EXTRA_PLUGGED
   *
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
      if (appProcess.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND
        && appProcess.processName.equals(packageName)) {
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
    TelephonyManager manager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    String carrierName = manager.getNetworkOperatorName();
    if (TextUtils.isEmpty(carrierName)) {
      carrierName = "";
    }
    return carrierName;
  }

  private String getCellularNetworkType() {
    TelephonyManager manager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
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
        if (wifiInfo.getNetworkId() != -1) {
          status = true;
        }
      } catch (Exception exception) {
        //Timber.w("Error getting Wifi Connection Status: ", exception);
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

      if (events.isEmpty()) {
        //Timber.d("No events in the queue to send so returning.");
        return null;
      }

      // Check for NetworkConnectivity
      if (!Mapbox.isConnected()) {
        //Timber.w("Not connected to network, so empty events cache and return without attempting to send events");
        // Make sure that events don't pile up when Offline
        // and thus impact available memory over time.
        events.removeAllElements();
        return null;
      }

      Response response = null;

      try {
        // Send data
        // =========
        JSONArray jsonArray = new JSONArray();

        Vector<Hashtable<String, Object>> eventsClone = (Vector<Hashtable<String, Object>>) events.clone();

        for (Hashtable<String, Object> evt : eventsClone) {
          JSONObject jsonObject = new JSONObject();

          // Build the JSON but only if there's a value for it in the evt
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_EVENT, evt.get(MapboxEvent.ATTRIBUTE_EVENT));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_CREATED, evt.get(MapboxEvent.ATTRIBUTE_CREATED));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_USERID, evt.get(MapboxEvent.ATTRIBUTE_USERID));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_ENABLED_TELEMETRY, evt.get(MapboxEvent.ATTRIBUTE_ENABLED_TELEMETRY));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_SOURCE, evt.get(MapboxEvent.ATTRIBUTE_SOURCE));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_SESSION_ID, evt.get(MapboxEvent.ATTRIBUTE_SESSION_ID));
          jsonObject.putOpt(MapboxEvent.KEY_LATITUDE, evt.get(MapboxEvent.KEY_LATITUDE));

          // Make sure Longitude Is Wrapped
          if (evt.containsKey(MapboxEvent.KEY_LONGITUDE)) {
            double lon = (double) evt.get(MapboxEvent.KEY_LONGITUDE);
            if ((lon < GeoConstants.MIN_LONGITUDE) || (lon > GeoConstants.MAX_LONGITUDE)) {
              lon = MathUtils.wrap(lon, GeoConstants.MIN_LONGITUDE, GeoConstants.MAX_LONGITUDE);
            }
            jsonObject.put(MapboxEvent.KEY_LONGITUDE, lon);
          }

          jsonObject.putOpt(MapboxEvent.KEY_ALTITUDE, evt.get(MapboxEvent.KEY_ALTITUDE));
          jsonObject.putOpt(MapboxEvent.KEY_ZOOM, evt.get(MapboxEvent.KEY_ZOOM));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM, evt.get(MapboxEvent.ATTRIBUTE_OPERATING_SYSTEM));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_USERID, evt.get(MapboxEvent.ATTRIBUTE_USERID));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_MODEL, evt.get(MapboxEvent.ATTRIBUTE_MODEL));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_RESOLUTION, evt.get(MapboxEvent.ATTRIBUTE_RESOLUTION));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_ACCESSIBILITY_FONT_SCALE,
            evt.get(MapboxEvent.ATTRIBUTE_ACCESSIBILITY_FONT_SCALE));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL, evt.get(MapboxEvent.ATTRIBUTE_BATTERY_LEVEL));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_PLUGGED_IN, evt.get(MapboxEvent.ATTRIBUTE_PLUGGED_IN));
          jsonObject.putOpt(MapboxEvent.ATTRIBUTE_WIFI, evt.get(MapboxEvent.ATTRIBUTE_WIFI));

          // Special Cases where empty string is denoting null and therefore should not be sent at all
          // This arises as thread safe Hashtable does not accept null values (nor keys)
          if (evt.containsKey(MapboxEvent.ATTRIBUTE_ORIENTATION)) {
            String orientation = (String) evt.get(MapboxEvent.ATTRIBUTE_ORIENTATION);
            if (!TextUtils.isEmpty(orientation)) {
              jsonObject.putOpt(MapboxEvent.ATTRIBUTE_ORIENTATION, orientation);
            }
          }
          if (evt.containsKey(MapboxEvent.ATTRIBUTE_CARRIER)) {
            String carrier = (String) evt.get(MapboxEvent.ATTRIBUTE_CARRIER);
            if (!TextUtils.isEmpty(carrier)) {
              jsonObject.putOpt(MapboxEvent.ATTRIBUTE_CARRIER, carrier);
            }
          }
          if (evt.containsKey(MapboxEvent.ATTRIBUTE_APPLICATION_STATE)) {
            String appState = (String) evt.get(MapboxEvent.ATTRIBUTE_APPLICATION_STATE);
            if (!TextUtils.isEmpty(appState)) {
              jsonObject.putOpt(MapboxEvent.ATTRIBUTE_APPLICATION_STATE,
                evt.get(MapboxEvent.ATTRIBUTE_APPLICATION_STATE));
            }
          }

          // Special Cases where null has to be passed if no value exists
          // Requires using put() instead of putOpt()
          String eventType = (String) evt.get(MapboxEvent.ATTRIBUTE_EVENT);
          if (!TextUtils.isEmpty(eventType) && eventType.equalsIgnoreCase(MapboxEvent.TYPE_MAP_CLICK)) {
            jsonObject.put(MapboxEvent.KEY_GESTURE_ID, evt.get(MapboxEvent.KEY_GESTURE_ID));
          }
          if (evt.containsKey(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE)) {
            String cellularNetworkType = (String) evt.get(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE);
            if (TextUtils.isEmpty(cellularNetworkType)) {
              jsonObject.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE, null);
            } else {
              jsonObject.put(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE,
                evt.get(MapboxEvent.ATTRIBUTE_CELLULAR_NETWORK_TYPE));
            }
          }

          jsonArray.put(jsonObject);
        }

        // Based on http://square.github.io/okhttp/3.x/okhttp/okhttp3/CertificatePinner.html
        CertificatePinner.Builder certificatePinnerBuilder = new CertificatePinner.Builder();
        if (stagingEnv) {
          // Staging - Geotrust
          certificatePinnerBuilder
            .add("cloudfront-staging.tilestream.net", "sha256/3euxrJOrEZI15R4104UsiAkDqe007EPyZ6eTL/XxdAY=")
            .add("cloudfront-staging.tilestream.net", "sha256/5kJvNEMw0KjrCAu7eXY5HZdvyCS13BbA0VJG1RSP91w=")
            .add("cloudfront-staging.tilestream.net", "sha256/r/mIkG3eEpVdm+u/ko/cwxzOMo1bk4TyHIlByibiA5E=");
        } else {
          certificatePinnerBuilder
            // Prod - Geotrust
            .add("events.mapbox.com", "sha256/BhynraKizavqoC5U26qgYuxLZst6pCu9J5stfL6RSYY=")
            .add("events.mapbox.com", "sha256/owrR9U9FWDWtrFF+myoRIu75JwU4sJwzvhCNLZoY37g=")
            .add("events.mapbox.com", "sha256/SQVGZiOrQXi+kqxcvWWE96HhfydlLVqFr4lQTqI5qqo=")
            // Prod - DigiCert
            .add("events.mapbox.com", "sha256/Tb0uHZ/KQjWh8N9+CZFLc4zx36LONQ55l6laDi1qtT4=")
            .add("events.mapbox.com", "sha256/RRM1dGqnDFsCJXBTHky16vi1obOlCgFFn/yOhI/y+ho=")
            .add("events.mapbox.com", "sha256/WoiWRyIOVNa9ihaBciRSC7XHjliYS9VwUGOIud4PB18=");
        }

        OkHttpClient client = new OkHttpClient.Builder()
          .certificatePinner(certificatePinnerBuilder.build())
          .addInterceptor(new GzipRequestInterceptor())
          .build();
        RequestBody body = RequestBody.create(JSON, jsonArray.toString());

        String url = eventsURL + "/events/v2?access_token=" + accessToken;

        Request request = new Request.Builder()
          .url(url)
          .header("User-Agent", userAgent)
          .post(body)
          .build();
        response = client.newCall(request).execute();
        //Timber.d("response code = " + response.code() + " for events " + events.size());

      } catch (Exception exception) {
        Timber.e("FlushTheEventsTask borked: ", exception);
      } finally {
        if (response != null && response.body() != null) {
          response.body().close();
        }
        // Reset Events
        // ============
        events.removeAllElements();
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

  private String getApplicationIdentifier() {
    try {
      PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
      return String.format(MapboxConstants.MAPBOX_LOCALE, "%s/%s/%s", context.getPackageName(),
        packageInfo.versionName, packageInfo.versionCode);
    } catch (Exception exception) {
      return "";
    }
  }

  static class ExponentialBackoffCounter {

    private static final long BASE_TIME = 30000 /*30 seconds*/;
    private int attempt;

    long getNextCount() {
      attempt++;
      return attempt * BASE_TIME;
    }
  }
}