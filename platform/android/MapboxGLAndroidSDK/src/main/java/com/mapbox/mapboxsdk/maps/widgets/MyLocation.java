package com.mapbox.mapboxsdk.maps.widgets;

import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Camera;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.os.SystemClock;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.MultiPoint;
import com.mapbox.services.commons.geojson.Polygon;
import com.mapbox.services.commons.models.Position;
import com.mapbox.services.commons.turf.TurfConstants;
import com.mapbox.services.commons.turf.TurfException;
import com.mapbox.services.commons.turf.TurfMeasurement;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconRotationAlignment;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;

public class MyLocation {

  // TODO port Turf Circle to MAS
  // TODO Check if code works in edit mode (I removed isInEditMode())
  // TODO rename source/layer to none common id so users don't accidentally use it.
  private static final String USER_LOCATION_ACCURACY_SOURCE = "user-location-accuracy-source";
  private static final String ACCURACY_LAYER = "accuracy-layer";
  private static final String USER_LOCATION_SOURCE = "user-location-source";
  private static final String LOCATION_LAYER = "location-layer";
  private static final String USER_LOCATION_ICON = "user-location-icon";
  private static final String USER_LOCATION_ARROW_ICON = "user-location-arrow-icon";

  private static final int COMPASS_UPDATE_RATE_MS = 500;

  private MyLocationBehavior myLocationBehavior;
  private GpsLocationListener userLocationListener;
  private CompassListener compassListener;
  private Context context;
  private Source userLocationSource;

  private ValueAnimator accuracyAnimator;

  private LatLng latLng;
  private Location location;
  private float accuracy;

  private Matrix matrix;
  private Camera camera;

  private MapboxMap mapboxMap;

  @MyLocationTracking.Mode
  private int myLocationTrackingMode;

  @MyBearingTracking.Mode
  private int myBearingTrackingMode;

  public MyLocation(@NonNull MapboxMap mapboxMap, Context context) {
    this.mapboxMap = mapboxMap;
    this.context = context;
    init();
  }

  private void init() {

    toggleGps(false);

    matrix = new Matrix();
    camera = new Camera();
    camera.setLocation(0, 0, -1000);

    myLocationBehavior = new MyLocationBehaviorFactory().getBehavioralModel(MyLocationTracking.TRACKING_NONE);
    compassListener = new CompassListener(context);

    addLayers();
  }

  private void addLayers() {
    if (mapboxMap.getLayer(ACCURACY_LAYER) == null) {
      FillLayer accuracyLayer = new FillLayer(ACCURACY_LAYER, USER_LOCATION_ACCURACY_SOURCE).withProperties(
        fillColor(Color.parseColor("#A44CF2")),
        fillOpacity(0.3f)
      );
      mapboxMap.addLayer(accuracyLayer);
    }

    if (mapboxMap.getLayer(LOCATION_LAYER) == null) {
      // Add the location icon image to the map
      Drawable drawable = ContextCompat.getDrawable(context, R.drawable.mapbox_user_icon);
      Bitmap icon = getBitmapFromDrawable(drawable);
      mapboxMap.addImage(USER_LOCATION_ICON, icon);

      SymbolLayer locationLayer = new SymbolLayer(LOCATION_LAYER, USER_LOCATION_SOURCE).withProperties(
        iconImage(USER_LOCATION_ICON),
        iconAllowOverlap(true),
        iconIgnorePlacement(true),
        iconRotationAlignment(Property.ICON_ROTATION_ALIGNMENT_MAP)
      );
      mapboxMap.addLayer(locationLayer);
    }
  }

  //TODO move to utils?
  private static Bitmap getBitmapFromDrawable(Drawable drawable) {
    if (drawable instanceof BitmapDrawable) {
      return ((BitmapDrawable) drawable).getBitmap();
    } else {
      Bitmap bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(),
        Bitmap.Config.ARGB_8888);
      Canvas canvas = new Canvas(bitmap);
      drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
      drawable.draw(canvas);
      return bitmap;
    }
  }

  public void setCameraPosition(CameraPosition position) {
    // if (position != null) {
    //    setTilt(position.tilt);
    //     setBearing(position.bearing);
    //  }
  }

  public void onStart() {
   // if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
    //  compassListener.onResume();
  //  }
   // if (isEnabled()) {
   //   toggleGps(true);
   // }
  }

  public void onStop() {
   // compassListener.onPause();
    //toggleGps(false);
  }

  /**
   * Enabled / Disable GPS location updates along with updating the UI
   *
   * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
   */
  // TODO rename to enable?
  public void toggleGps(boolean enableGps) {
    LocationServices locationServices = LocationServices.getLocationServices(context);
    if (enableGps) {
      // Set an initial location if one available
      Location lastLocation = locationServices.getLastLocation();

      if (lastLocation != null) {
        setLocation(lastLocation);
      }

      if (userLocationListener == null) {
        userLocationListener = new MyLocation.GpsLocationListener(this);
      }

      locationServices.addLocationListener(userLocationListener);
    } else {
      // Disable location and user dot
      location = null;
      locationServices.removeLocationListener(userLocationListener);
    }

    Layer accuracy = mapboxMap.getLayer(ACCURACY_LAYER);
    if (accuracy != null) {
      accuracy.setProperties(visibility(enableGps ? Property.VISIBLE : Property.NONE));
    }

    Layer locationIcon = mapboxMap.getLayer(LOCATION_LAYER);
    if (locationIcon != null) {
      locationIcon.setProperties(visibility(enableGps ? Property.VISIBLE : Property.NONE));
    }

    locationServices.toggleGPS(enableGps);
  }

  public Location getLocation() {
    return location;
  }

  public void setLocation(Location location) {
    if (location == null) {
      this.location = null;
      return;
    }

    this.location = location;
    myLocationBehavior.updateLatLng(location);
  }

  public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
    this.myBearingTrackingMode = myBearingTrackingMode;
    if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
      compassListener.onResume();
    } else {
      compassListener.onPause();
      if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
        // always face north
        setCompass(0);
      }
    }
  }

  public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
    MyLocationBehaviorFactory factory = new MyLocationBehaviorFactory();
    myLocationBehavior = factory.getBehavioralModel(myLocationTrackingMode);

    if (location != null) {
      if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
        // center map directly
        mapboxMap.easeCamera(CameraUpdateFactory.newLatLng(new LatLng(location)), 0, false /*linear interpolator*/,
          false /*do not disable tracking*/, null);
      } else {
        // do not use interpolated location from tracking mode
        latLng = null;
      }
      myLocationBehavior.updateLatLng(location);
    }

    this.myLocationTrackingMode = myLocationTrackingMode;
  }

  @MyLocationTracking.Mode
  public int getMyLocationTrackingMode() {
    return myLocationTrackingMode;
  }


  @MyBearingTracking.Mode
  public int getMyBearingTrackingMode() {
    return myBearingTrackingMode;
  }

  private void setCompass(double bearing) {
    setCompass(bearing, 0 /* no animation */);
  }

  private void setCompass(double bearing, long duration) {
//    float oldDir = previousDirection;
//    if (directionAnimator != null) {
//      oldDir = (Float) directionAnimator.getAnimatedValue();
//      directionAnimator.end();
//      directionAnimator = null;
//    }
//
//    float newDir = (float) bearing;
//    float diff = oldDir - newDir;
//    if (diff > 180.0f) {
//      newDir += 360.0f;
//    } else if (diff < -180.0f) {
//      newDir -= 360.f;
//    }
//    previousDirection = newDir;
//
//    directionAnimator = ValueAnimator.ofFloat(oldDir, newDir);
//    directionAnimator.setDuration(duration);
//    directionAnimator.addUpdateListener(invalidateSelfOnUpdateListener);
//    directionAnimator.start();
  }

  private static class GpsLocationListener implements LocationListener {

    private WeakReference<MyLocation> userLocation;

    GpsLocationListener(MyLocation myLocation) {
      userLocation = new WeakReference<>(myLocation);
    }

    /**
     * Callback method for receiving location updates from LocationServices.
     *
     * @param location The new Location data
     */
    @Override
    public void onLocationChanged(Location location) {
      MyLocation myLocation = userLocation.get();
      if (myLocation != null) {
        myLocation.setLocation(location);
      }
    }
  }

  private class CompassListener implements SensorEventListener {

    private final SensorManager sensorManager;

    private Sensor rotationVectorSensor;
    float[] matrix = new float[9];
    float[] orientation = new float[3];

    // Compass data
    private long compassUpdateNextTimestamp = 0;

    CompassListener(Context context) {
      sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
      rotationVectorSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
    }

    public void onResume() {
      sensorManager.registerListener(this, rotationVectorSensor, SensorManager.SENSOR_DELAY_GAME);
    }

    public void onPause() {
      sensorManager.unregisterListener(this, rotationVectorSensor);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {

      // check when the last time the compass was updated, return if too soon.
      long currentTime = SystemClock.elapsedRealtime();
      if (currentTime < compassUpdateNextTimestamp) {
        return;
      }

      if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {

        // calculate the rotation matrix
        SensorManager.getRotationMatrixFromVector(matrix, event.values);
        SensorManager.getOrientation(matrix, orientation);

//        magneticHeading = (float) Math.toDegrees(SensorManager.getOrientation(matrix, orientation)[0]);
//        if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
//          // Change the user location view orientation to reflect the device orientation
//          rotateCamera(magneticHeading);
//          setCompass(0, COMPASS_UPDATE_RATE_MS);
//        } else {
//          // Change compass direction
//          setCompass(magneticHeading - bearing, COMPASS_UPDATE_RATE_MS);
//        }

        compassUpdateNextTimestamp = currentTime + COMPASS_UPDATE_RATE_MS;
      }
    }

    private void rotateCamera(float rotation) {
      CameraPosition.Builder builder = new CameraPosition.Builder();
      builder.bearing(rotation);
      mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), COMPASS_UPDATE_RATE_MS,
        false /*linear interpolator*/, false /*do not disable tracking*/, null);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

  }

  // https://github.com/mapbox/mapbox-gl-style-spec/issues/459#issuecomment-240555277
  private FeatureCollection createGeoJSONCircle(Position center, double radius) throws TurfException {
    // turf circle
    int steps = 64;
    List<Position> coordinates = new ArrayList<>();

    for (int i = 0; i < steps; i++) {
      coordinates.add(TurfMeasurement.destination(center, radius, i * 360 / steps, TurfConstants.UNIT_METERS));
    }

    List<List<Position>> a = new ArrayList<>();
    a.add(coordinates);
    Polygon polygon = Polygon.fromCoordinates(a);
    return FeatureCollection.fromFeatures(new Feature[] {Feature.fromGeometry(polygon)});
  }

  private class MyLocationBehaviorFactory {

    MyLocationBehavior getBehavioralModel(@MyLocationTracking.Mode int mode) {
      if (mode == MyLocationTracking.TRACKING_NONE) {
        return new MyLocationShowBehavior();
      } else {
        return new MyLocationTrackingBehavior();
      }
    }
  }

  private abstract class MyLocationBehavior {

    void updateLatLng(@NonNull Location newLocation) {
      location = newLocation;
      // TODO animate from location to new location

      List<Position> p = new ArrayList<>();
      p.add(Position.fromCoordinates(location.getLongitude(), location.getLatitude()));
      MultiPoint multiPoint = MultiPoint.fromCoordinates(p);
      FeatureCollection featureCollection = FeatureCollection.fromFeatures(new Feature[] {Feature.fromGeometry(multiPoint)});
      GeoJsonSource source = mapboxMap.getSourceAs(USER_LOCATION_SOURCE);
      if (source != null) {
        source.setGeoJson(featureCollection);
      } else {
        userLocationSource = new GeoJsonSource(USER_LOCATION_SOURCE, featureCollection);
        mapboxMap.addSource(userLocationSource);
      }
    }

    void updateLatLng(double lat, double lon) {
      if (latLng != null) {
        latLng.setLatitude(lat);
        latLng.setLongitude(lon);
      }
    }

    void updateAccuracy(@NonNull final Location location) {
      // TODO remove devision of 10 once https://github.com/mapbox/mapbox-java/issues/248 is resolved.
      // TODO filter out occasional flickering?
      accuracyAnimator = ValueAnimator.ofFloat(accuracy / 10, location.getAccuracy() / 10);
      accuracyAnimator.setDuration(750);
      accuracyAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
        @Override
        public void onAnimationUpdate(ValueAnimator valueAnimator) {
          try {
            FeatureCollection featureCollection = createGeoJSONCircle(Position.fromCoordinates(
              location.getLongitude(),
              location.getLatitude()),
              (float) accuracyAnimator.getAnimatedValue()
            );
            GeoJsonSource source = mapboxMap.getSourceAs(USER_LOCATION_ACCURACY_SOURCE);
            if (source != null) {
              source.setGeoJson(featureCollection);
            } else {
              userLocationSource = new GeoJsonSource(USER_LOCATION_ACCURACY_SOURCE, featureCollection);
              mapboxMap.addSource(userLocationSource);
            }
          } catch (TurfException turfException) {
            turfException.printStackTrace();
          }
        }
      });
      accuracyAnimator.start();
      accuracy = location.getAccuracy();
    }
  }

  private class MyLocationTrackingBehavior extends MyLocation.MyLocationBehavior {

    @Override
    void updateLatLng(@NonNull Location location) {
      super.updateLatLng(location);
      if (latLng == null) {
        // first location fix
        latLng = new LatLng(location);
      //  locationUpdateTimestamp = SystemClock.elapsedRealtime();
      }

      // updateLatLng timestamp
     // float previousUpdateTimeStamp = locationUpdateTimestamp;
    //  locationUpdateTimestamp = SystemClock.elapsedRealtime();

      // calculate animation duration
      float animationDuration;
     // if (previousUpdateTimeStamp == 0) {
        animationDuration = 0;
    //  } else {
     //   animationDuration = (locationUpdateTimestamp - previousUpdateTimeStamp) * 1.1f
        /*make animation slightly longer*/;
   //  }

      // calculate interpolated location
      latLng = new LatLng(location);
      CameraPosition.Builder builder = new CameraPosition.Builder().target(latLng);

      // add direction
      if (myBearingTrackingMode == MyBearingTracking.GPS) {
        if (location.hasBearing()) {
          builder.bearing(location.getBearing());
        }
        setCompass(0, COMPASS_UPDATE_RATE_MS);
      }

      // accuracy
      updateAccuracy(location);

      // ease to new camera position with a linear interpolator
      mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), (int) animationDuration,
        false /*linear interpolator*/, false /*do not disable tracking*/, null);
    }
  }

  private class MyLocationShowBehavior extends MyLocation.MyLocationBehavior {

    @Override
    void updateLatLng(@NonNull final Location location) {
      super.updateLatLng(location);

      if (latLng == null) {
        // first location update
        latLng = new LatLng(location);
        // locationUpdateTimestamp = SystemClock.elapsedRealtime();
      }

      // update LatLng location
      LatLng newLocation = new LatLng(location);

      updateAccuracy(location);
      // update LatLng accuracy
//
//      // calculate updateLatLng time + add some extra offset to improve animation
//      long previousUpdateTimeStamp = locationUpdateTimestamp;
//      locationUpdateTimestamp = SystemClock.elapsedRealtime();
//      long locationUpdateDuration = (long) ((locationUpdateTimestamp - previousUpdateTimeStamp) * 1.2f);
//
//      // animate changes
//      if (locationChangeAnimator != null) {
//        locationChangeAnimator.end();
//        locationChangeAnimator = null;
//      }
//
//      locationChangeAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
//      locationChangeAnimator.setDuration(locationUpdateDuration);
//      locationChangeAnimator.addUpdateListener(new MyLocationView.MarkerCoordinateAnimatorListener(this,
//        latLng, newLocation
//      ));
//      locationChangeAnimator.start();
      latLng = newLocation;
    }

//    @Override
//    void invalidate() {
////      if (latLng != null) {
////        screenLocation = projection.toScreenLocation(latLng);
////      }
////      MyLocationView.this.invalidate();
//    }
  }
}
