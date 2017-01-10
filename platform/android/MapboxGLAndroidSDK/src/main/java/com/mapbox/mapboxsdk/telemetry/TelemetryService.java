package com.mapbox.mapboxsdk.telemetry;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ServiceInfo;
import android.os.AsyncTask;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.v4.content.LocalBroadcastManager;

import timber.log.Timber;

/**
 * Manages Startup and Shutdown of Telemetry resources
 */
public class TelemetryService extends Service {

  private TelemetryLocationReceiver telemetryLocationReceiver = null;

  /**
   * Return the communication channel to the service.  May return null if
   * clients can not bind to the service.  The returned
   * {@link IBinder} is usually for a complex interface
   * that has been <a href="{@docRoot}guide/components/aidl.html">described using
   * aidl</a>.
   * <p>
   * <p><em>Note that unlike other application components, calls on to the
   * IBinder interface returned here may not happen on the main thread
   * of the process</em>.  More information about the main thread can be found in
   * <a href="{@docRoot}guide/topics/fundamentals/processes-and-threads.html">Processes and
   * Threads</a>.</p>
   *
   * @param intent The Intent that was used to bind to this service,
   *               as given to {@link Context#bindService
   *               Context.bindService}.  Note that any extras that were included with
   *               the Intent at that point will <em>not</em> be seen here.
   * @return Return an IBinder through which clients can call on to the
   * service.
   */
  @Nullable
  @Override
  public IBinder onBind(Intent intent) {
    return null;
  }


  /**
   * Called by the system when the service is first created.  Do not call this method directly.
   */
  @Override
  public void onCreate() {
    super.onCreate();

    Timber.i("onCreate() called");

    // Enable Location Listening for lifecycle of app
    LocalBroadcastManager.getInstance(getApplicationContext()).registerReceiver(
      new TelemetryLocationReceiver(),
      new IntentFilter(TelemetryLocationReceiver.INTENT_STRING)
    );
  }

  /**
   * Called by the system to notify a Service that it is no longer used and is being removed.  The
   * service should clean up any resources it holds (threads, registered
   * receivers, etc) at this point.  Upon return, there will be no more calls
   * in to this Service object and it is effectively dead.  Do not call this method directly.
   */
  @Override
  public void onDestroy() {
    shutdownTelemetry();
    super.onDestroy();
  }

  /**
   * This is called if the service is currently running and the user has
   * removed a task that comes from the service's application.  If you have
   * set {@link ServiceInfo#FLAG_STOP_WITH_TASK ServiceInfo.FLAG_STOP_WITH_TASK}
   * then you will not receive this callback; instead, the service will simply
   * be stopped.
   *
   * @param rootIntent The original root Intent that was used to launch
   *                   the task that is being removed.
   */
  @Override
  public void onTaskRemoved(Intent rootIntent) {
    shutdownTelemetry();
    super.onTaskRemoved(rootIntent);
  }

  /**
   * Called by the system every time a client explicitly starts the service by calling
   * {@link Context#startService}, providing the arguments it supplied and a
   * unique integer token representing the start request.  Do not call this method directly.
   * <p>
   * <p>For backwards compatibility, the default implementation calls
   * {@link #onStart} and returns either {@link #START_STICKY}
   * or {@link #START_STICKY_COMPATIBILITY}.
   * </p>
   * <p>If you need your application to run on platform versions prior to API
   * level 5, you can use the following model to handle the older {@link #onStart}
   * callback in that case.  The <code>handleCommand</code> method is implemented by
   * you as appropriate:
   * </p>
   * <p>
   * <p class="caution">Note that the system calls this on your
   * service's main thread.  A service's main thread is the same
   * thread where UI operations take place for Activities running in the
   * same process.  You should always avoid stalling the main
   * thread's event loop.  When doing long-running operations,
   * network calls, or heavy disk I/O, you should kick off a new
   * thread, or use {@link AsyncTask}.</p>
   *
   * @param intent  The Intent supplied to {@link Context#startService},
   *                as given.  This may be null if the service is being restarted after
   *                its process has gone away, and it had previously returned anything
   *                except {@link #START_STICKY_COMPATIBILITY}.
   * @param flags   Additional data about this start request.  Currently either
   *                0, {@link #START_FLAG_REDELIVERY}, or {@link #START_FLAG_RETRY}.
   * @param startId A unique integer representing this specific request to
   *                start.  Use with {@link #stopSelfResult(int)}.
   * @return The return value indicates what semantics the system should
   * use for the service's current started state.  It may be one of the
   * constants associated with the {@link #START_CONTINUATION_MASK} bits.
   * @see #stopSelfResult(int)
   */
  @Override
  public int onStartCommand(Intent intent, int flags, int startId) {

    Timber.i("onStartCommand() called");

    return START_NOT_STICKY;
  }

  private void shutdownTelemetry() {

    // Send Any Remaining events to the server
    MapboxEventManager.getMapboxEventManager().flushEventsQueueImmediately();

    // Undesired, but needed trick to keep app alive long enough for data to get to server
    try {
      Thread.sleep(1000);
    } catch (Exception exception) {
      Timber.e("Error while trying to sleep for 1 second: " + exception);
    }

    try {
      unregisterReceiver(telemetryLocationReceiver);
    } catch (IllegalArgumentException illegalArgumentException) {
      Timber.e("Error when unregisterReceiver: " + illegalArgumentException);
    }

  }
}
